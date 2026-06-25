// 큰 정수 연산 관련 헤더
#include "core/sha256.h"
#include "core/big_int.h"
#include "core/secp256k1_params.h"

// 타원곡선 관련 헤더
#include "cryptography/csprng.h"
#include "cryptography/ecc_point.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// CSPRNG 테스트 함수들
void test_csprng_initialization() {
    printf("\n========== CSPRNG 초기화 테스트 ==========\n");
    
    printf("1. csprng_init() 호출...\n");
    csprng_init();
    printf("   ✓ CSPRNG 초기화 성공\n");
    
    printf("2. csprng_cleanup() 호출...\n");
    csprng_cleanup();
    printf("   ✓ CSPRNG 정리 완료\n");
}

void test_csprng_random_bytes() {
    printf("\n========== 난수 바이트 생성 테스트 ==========\n");
    
    csprng_init();
    
    // 32바이트 난수 생성
    uint8_t random_bytes[32];
    printf("1. 32바이트 난수 생성...\n");
    csprng_random_bytes(random_bytes, 32);
    printf("   생성된 난수: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", random_bytes[i]);
    }
    printf("\n");
    
    // 16바이트 난수 생성
    uint8_t random_bytes_16[16];
    printf("2. 16바이트 난수 생성...\n");
    csprng_random_bytes(random_bytes_16, 16);
    printf("   생성된 난수: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", random_bytes_16[i]);
    }
    printf("\n");
    
    // 여러 번 생성하여 다양성 확인
    printf("3. 연속 5번 난수 생성 (다양성 확인)...\n");
    for (int iteration = 0; iteration < 5; iteration++) {
        uint8_t temp[8];
        csprng_random_bytes(temp, 8);
        printf("   %d번째: ", iteration + 1);
        for (int i = 0; i < 8; i++) {
            printf("%02x", temp[i]);
        }
        printf("\n");
    }
    
    printf("   ✓ 난수 생성 테스트 완료\n");
}

void test_csprng_random_bigint() {
    printf("\n========== BigInt 난수 생성 테스트 ==========\n");
    
    BigInt random_bigint;
    
    // 256비트 난수 생성
    printf("1. 256비트 난수 생성...\n");
    csprng_random_bigint(&random_bigint, 256);
    printf("   생성된 난수 (16진수): 0x");
    big_int_print_hex(&random_bigint);
    printf("\n");
    
    // 128비트 난수 생성
    printf("2. 128비트 난수 생성...\n");
    csprng_random_bigint(&random_bigint, 128);
    printf("   생성된 난수 (16진수): 0x");
    big_int_print_hex(&random_bigint);
    printf("\n");
    
    // 64비트 난수 생성
    printf("3. 64비트 난수 생성...\n");
    csprng_random_bigint(&random_bigint, 64);
    printf("   생성된 난수 (16진수): 0x");
    big_int_print_hex(&random_bigint);
    printf("\n");
    
    printf("   ✓ BigInt 난수 생성 테스트 완료\n");
}

void test_csprng_random_bigint_mod() {
    printf("\n========== 모듈로 난수 생성 테스트 ==========\n");
    
    BigInt modulus, random_mod;
    
    // 모듈로 설정: 2^100
    printf("1. 모듈로 = 2^100 설정\n");
    big_int_zero(&modulus);
    modulus.limbs[1] = 1;  // 2^64 = 1LL << 64, so 2^100 = 1LL << 36 in limb[1]
    modulus.limbs[1] = (1ULL << 36);  // 2^100 = 2^64 * 2^36
    
    printf("   모듈로 값: 0x");
    big_int_print_hex(&modulus);
    printf("\n");
    
    // 모듈로 범위 내 난수 생성 (5번)
    printf("2. 모듈로 범위 내 난수 생성 (5번)...\n");
    for (int i = 0; i < 5; i++) {
        csprng_random_bigint_mod(&random_mod, &modulus);
        printf("   %d번째: 0x", i + 1);
        big_int_print_hex(&random_mod);
        printf("\n");
    }
    
    printf("   ✓ 모듈로 난수 생성 테스트 완료\n");
}

void test_csprng_reseed() {
    printf("\n========== 엔트로피 재수집 테스트 ==========\n");
    
    uint8_t bytes_before[16];
    uint8_t bytes_after[16];
    
    printf("1. 재수집 전 난수 생성...\n");
    csprng_random_bytes(bytes_before, 16);
    printf("   난수: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", bytes_before[i]);
    }
    printf("\n");
    
    printf("2. csprng_reseed() 호출...\n");
    csprng_reseed();
    printf("   ✓ 엔트로피 재수집 완료\n");
    
    printf("3. 재수집 후 난수 생성...\n");
    csprng_random_bytes(bytes_after, 16);
    printf("   난수: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", bytes_after[i]);
    }
    printf("\n");
    
    // 두 난수가 다른지 확인
    int different = 0;
    for (int i = 0; i < 16; i++) {
        if (bytes_before[i] != bytes_after[i]) {
            different = 1;
            break;
        }
    }
    printf("   → 두 난수가 %s\n", different ? "다름 (정상)" : "같음 (경고)");
}

void test_csprng_stress() {
    printf("\n========== CSPRNG 스트레스 테스트 ==========\n");
    
    uint8_t buffer[1024];
    clock_t start, end;
    double cpu_time_used;
    
    printf("1. 대량 난수 생성 (1MB)...\n");
    start = clock();
    
    for (int i = 0; i < 1024; i++) {
        csprng_random_bytes(buffer, 1024);
    }
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    printf("   ✓ 1MB 난수 생성 완료\n");
    printf("   소요 시간: %.4f초\n", cpu_time_used);
    printf("   처리량: %.2f MB/s\n", 1.0 / cpu_time_used);
}

void test_csprng_comprehensive() {
    printf("\n========== 종합 테스트 ==========\n");
    
    printf("1. CSPRNG 초기화...\n");
    csprng_init();
    printf("   ✓ 초기화 완료\n");
    
    printf("2. 다양한 크기의 난수 생성...\n");
    uint8_t bytes[32];
    BigInt bigint;
    
    for (int size = 8; size <= 32; size += 8) {
        csprng_random_bytes(bytes, size);
        printf("   - %d바이트: ", size);
        for (int i = 0; i < size; i++) {
            printf("%02x", bytes[i]);
        }
        printf("\n");
    }
    
    printf("3. BigInt 난수 생성...\n");
    csprng_random_bigint(&bigint, 256);
    printf("   생성된 값: 0x");
    big_int_print_hex(&bigint);
    printf("\n");
    
    printf("4. 정리...\n");
    csprng_cleanup();
    printf("   ✓ 정리 완료\n");
    
    printf("\n✓ 종합 테스트 완료\n");
}

int main() {
    printf("╔════════════════════════════════════════╗\n");
    printf("║     CSPRNG (Cryptographically Secure   ║\n");
    printf("║  Pseudo-Random Number Generator) 테스트║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    // 모든 테스트 실행
    test_csprng_initialization();
    test_csprng_random_bytes();
    test_csprng_random_bigint();
    test_csprng_random_bigint_mod();
    test_csprng_reseed();
    test_csprng_stress();
    test_csprng_comprehensive();
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║       모든 테스트 완료!                ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    return 0;
}