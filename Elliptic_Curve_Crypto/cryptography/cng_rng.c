#include "cng_rng.h"
#include "big_int.h"

// CNG(Cryptography Next Generation)의 RNG 기능으로 256bit 난수 생성

void gen_random_bytes(unsigned char* buffer, size_t length) {
    BCRYPT_ALG_HANDLE hAlgorithm = NULL;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_RNG_ALGORITHM, NULL, 0);
    if (!NT_SUCCESS(status)) {
        fprintf(stderr, "BCryptOpenAlgorithmProvider failed: 0x%08x\n", status);
        exit(EXIT_FAILURE);
    }

    status = BCryptGenRandom(hAlgorithm, buffer, (ULONG)length, 0);
    if (!NT_SUCCESS(status)) {
        fprintf(stderr, "BCryptGenRandom failed: 0x%08x\n", status);
        BCryptCloseAlgorithmProvider(hAlgorithm, 0);
        exit(EXIT_FAILURE);
    }

    BCryptCloseAlgorithmProvider(hAlgorithm, 0);
}

void for_test_cng_rng() {
    BigInt random_value;
    unsigned char buffer[32]; // 256 bits = 32 bytes

    gen_random_bytes(buffer, sizeof(buffer));
    big_int_from_hex_string(&random_value, (char*)buffer);
    printf("Generated random value: ");
    big_int_print_hex(&random_value);
    printf("\n");
    return 0;
}