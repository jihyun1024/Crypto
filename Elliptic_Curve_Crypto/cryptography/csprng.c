#include "csprng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// ============================================================================
// CSPRNG (Cryptographically Secure Pseudo-Random Number Generator)
// Unix/Linux 환경에 최적화된 구현
// ============================================================================

#define ENTROPY_POOL_SIZE 512   // 엔트로피 풀 크기 (바이트)
#define RESEED_THRESHOLD 256    // 엔트로피 풀 소진 임계값

typedef struct {
    uint8_t entropy_pool[ENTROPY_POOL_SIZE];
    size_t entropy_index;
    uint32_t reseed_count;
} CSPRNG_State;

static CSPRNG_State csprng_state = {0};
static int csprng_initialized = 0;

// /dev/urandom에서 난수 읽기 (Unix 전용)
static int read_from_urandom(uint8_t *buffer, size_t length) {
    if (buffer == NULL || length == 0) {
        return -1;
    }
    
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open /dev/urandom");
        return -1;
    }
    
    ssize_t bytes_read = 0;
    size_t total_read = 0;
    
    while (total_read < length) {
        bytes_read = read(fd, buffer + total_read, length - total_read);
        if (bytes_read < 0) {
            perror("Failed to read from /dev/urandom");
            close(fd);
            return -1;
        }
        if (bytes_read == 0) {
            break;
        }
        total_read += bytes_read;
    }
    
    close(fd);
    return (total_read == length) ? 0 : -1;
}

// 시스템 엔트로피 수집 (프로세스 정보, 시간 등)
static void collect_system_entropy(uint8_t *buffer, size_t length) {
    if (buffer == NULL || length == 0) {
        return;
    }
    
    memset(buffer, 0, length);
    size_t offset = 0;
    
    // 1. 현재 시간 (마이크로초 단위)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    if (offset + sizeof(ts.tv_sec) < length) {
        memcpy(buffer + offset, &ts.tv_sec, sizeof(ts.tv_sec));
        offset += sizeof(ts.tv_sec);
    }
    if (offset + sizeof(ts.tv_nsec) < length) {
        memcpy(buffer + offset, &ts.tv_nsec, sizeof(ts.tv_nsec));
        offset += sizeof(ts.tv_nsec);
    }
    
    // 2. 프로세스 정보
    pid_t pid = getpid();
    if (offset + sizeof(pid) < length) {
        memcpy(buffer + offset, &pid, sizeof(pid));
        offset += sizeof(pid);
    }
    
    // 3. 메모리 주소 (ASLR 엔트로피)
    uint64_t addr = (uint64_t)buffer;
    if (offset + sizeof(addr) < length) {
        memcpy(buffer + offset, &addr, sizeof(addr));
        offset += sizeof(addr);
    }
    
    // 4. 고해상도 시간 (실제 시간)
    time_t real_time = time(NULL);
    if (offset + sizeof(real_time) < length) {
        memcpy(buffer + offset, &real_time, sizeof(real_time));
        offset += sizeof(real_time);
    }
}

// CSPRNG 초기화
void csprng_init(void) {
    if (csprng_initialized) {
        return;
    }
    
    memset(&csprng_state, 0, sizeof(CSPRNG_State));
    csprng_state.entropy_index = 0;
    csprng_state.reseed_count = 0;
    csprng_initialized = 1;
    
    // 초기 엔트로피 수집
    csprng_collect_entropy();
}

// CSPRNG 정리 및 메모리 해제
void csprng_cleanup(void) {
    if (!csprng_initialized) {
        return;
    }
    
    // 엔트로피 풀 안전하게 삭제 (보안: 민감한 메모리 초기화)
    memset(csprng_state.entropy_pool, 0, ENTROPY_POOL_SIZE);
    memset(&csprng_state, 0, sizeof(CSPRNG_State));
    
    csprng_initialized = 0;
}

// 초기 시드 설정
void csprng_seed_init(void) {
    if (!csprng_initialized) {
        csprng_init();
    }
    
    uint8_t seed_buffer[64];
    collect_system_entropy(seed_buffer, sizeof(seed_buffer));
    
    // 엔트로피 풀에 혼합
    for (size_t i = 0; i < sizeof(seed_buffer) && i < ENTROPY_POOL_SIZE; i++) {
        csprng_state.entropy_pool[i] ^= seed_buffer[i];
    }
}

// 엔트로피 수집 (/dev/urandom + 시스템 정보)
void csprng_collect_entropy(void) {
    if (!csprng_initialized) {
        return;
    }
    
    uint8_t temp_buffer[ENTROPY_POOL_SIZE];
    memset(temp_buffer, 0, ENTROPY_POOL_SIZE);
    
    // 1. /dev/urandom에서 엔트로피 수집
    if (read_from_urandom(temp_buffer, ENTROPY_POOL_SIZE) == 0) {
        // XOR을 사용하여 새로운 엔트로피와 기존 엔트로피 혼합
        for (size_t i = 0; i < ENTROPY_POOL_SIZE; i++) {
            csprng_state.entropy_pool[i] ^= temp_buffer[i];
        }
    } else {
        fprintf(stderr, "Warning: Failed to read from /dev/urandom\n");
    }
    
    // 2. 시스템 엔트로피 추가
    uint8_t sys_entropy[128];
    collect_system_entropy(sys_entropy, sizeof(sys_entropy));
    for (size_t i = 0; i < sizeof(sys_entropy); i++) {
        csprng_state.entropy_pool[i] ^= sys_entropy[i];
    }
    
    csprng_state.reseed_count++;
}

// 주기적으로 엔트로피 재수집
void csprng_reseed(void) {
    if (!csprng_initialized) {
        csprng_init();
    }
    
    csprng_collect_entropy();
    csprng_state.entropy_index = 0;  // 인덱스 리셋
}

// 난수 바이트 생성
void csprng_random_bytes(uint8_t *out, size_t out_len) {
    if (!csprng_initialized) {
        csprng_init();
    }
    
    if (out == NULL || out_len == 0) {
        return;
    }
    
    // 엔트로피 풀이 거의 소진되었으면 재수집
    if (csprng_state.entropy_index + out_len > ENTROPY_POOL_SIZE - RESEED_THRESHOLD) {
        csprng_reseed();
    }
    
    // 엔트로피 풀에서 난수 추출
    for (size_t i = 0; i < out_len; i++) {
        out[i] = csprng_state.entropy_pool[(csprng_state.entropy_index + i) % ENTROPY_POOL_SIZE];
    }
    
    csprng_state.entropy_index = (csprng_state.entropy_index + out_len) % ENTROPY_POOL_SIZE;
}

// 지정된 길이의 BigInt 난수 생성
void csprng_random_bigint(BigInt *out, size_t out_len) {
    if (out == NULL) {
        return;
    }
    
    // out_len은 비트 단위, 바이트 단위로 변환
    size_t byte_len = (out_len + 7) / 8;
    
    // 최대 32바이트(256비트)로 제한
    if (byte_len > 32) {
        byte_len = 32;
    }
    
    uint8_t random_bytes[32];
    memset(random_bytes, 0, 32);
    
    csprng_random_bytes(random_bytes, byte_len);
    
    // 바이트 배열을 BigInt로 변환
    big_int_zero(out);
    for (size_t i = 0; i < byte_len; i++) {
        out->limbs[i / 8] |= ((uint64_t)random_bytes[i]) << ((i % 8) * 8);
    }
}

// 모듈로 난수 생성 (0 ~ mod-1 범위)
void csprng_random_bigint_mod(BigInt *out, const BigInt *mod) {
    if (out == NULL || mod == NULL || big_int_is_zero(mod)) {
        return;
    }
    
    BigInt random, temp;
    
    // mod과 같은 크기의 난수 생성
    csprng_random_bigint(&random, 256);
    
    // temp = random mod mod
    big_int_mod(&temp, &random, mod);
    
    // 결과를 out에 복사
    big_int_copy(out, &temp);
}