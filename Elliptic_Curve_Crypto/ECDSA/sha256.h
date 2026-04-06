#ifndef SHA256_ECDSA_H
#define SHA256_ECDSA_H

#include <stdint.h>
#include <stddef.h>

#define SHA256_DIGEST_LENGTH 32
#define SHA256_BLOCK_SIZE 64

typedef struct {
    uint32_t h[8];
    uint64_t total_length;
    uint8_t buffer[SHA256_BLOCK_SIZE];
    size_t buffer_length;
} SHA256_CTX;

void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const uint8_t *data, size_t length);
void sha256_final(SHA256_CTX *ctx, uint8_t *digest);

// Convenience function
void sha256(const uint8_t *data, size_t length, uint8_t *digest);

#endif
