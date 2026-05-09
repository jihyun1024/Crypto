// 큰 정수 연산 관련 헤더
#include "core/sha256.h"
#include "core/big_int.h"
#include "core/secp256k1_params.h"

// 타원곡선 관련 헤더
#include "cryptography/csprng.h"
#include "cryptography/cng_rng.h"
#include "cryptography/ecc_point.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    // 여기에 테스트하고 싶은 코드의 함수 호출 추가
    // SHA-256 테스트
    const char *test_str = "hello world";
    uint8_t digest[SHA256_DIGEST_LENGTH];
    sha256((const uint8_t*)test_str, strlen(test_str), digest);
    printf("SHA-256(\"%s\") = ", test_str);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        printf("%02x", digest[i]);
    printf("\n");

    // 100초동안 대기
    printf("Waiting for 100 seconds...\n");
    sleep(100);

    return 0;
}