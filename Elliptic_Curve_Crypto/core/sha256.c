#include "sha256.h"
#include <string.h>

// SHA-256 constants
static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// Helper macros
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SIGMA0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SIGMA1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define gamma0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ ((x) >> 3))
#define gamma1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ ((x) >> 10))

// Initialize SHA-256 context
void sha256_init(SHA256_CTX *ctx) {
    ctx->h[0] = 0x6a09e667;
    ctx->h[1] = 0xbb67ae85;
    ctx->h[2] = 0x3c6ef372;
    ctx->h[3] = 0xa54ff53a;
    ctx->h[4] = 0x510e527f;
    ctx->h[5] = 0x9b05688c;
    ctx->h[6] = 0x1f83d9ab;
    ctx->h[7] = 0x5be0cd19;
    
    ctx->total_length = 0;
    ctx->buffer_length = 0;
}

// Process a single 512-bit block
static void sha256_process_block(SHA256_CTX *ctx, const uint8_t *block) {
    uint32_t w[64];
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t t1, t2;
    
    // Prepare message schedule
    for (int i = 0; i < 16; i++) {
        w[i] = ((uint32_t)block[i * 4] << 24) |
               ((uint32_t)block[i * 4 + 1] << 16) |
               ((uint32_t)block[i * 4 + 2] << 8) |
               ((uint32_t)block[i * 4 + 3]);
    }
    
    for (int i = 16; i < 64; i++) {
        w[i] = gamma1(w[i - 2]) + w[i - 7] + gamma0(w[i - 15]) + w[i - 16];
    }
    
    // Initialize working variables
    a = ctx->h[0];
    b = ctx->h[1];
    c = ctx->h[2];
    d = ctx->h[3];
    e = ctx->h[4];
    f = ctx->h[5];
    g = ctx->h[6];
    h = ctx->h[7];
    
    // Main loop
    for (int i = 0; i < 64; i++) {
        t1 = h + SIGMA1(e) + CH(e, f, g) + K[i] + w[i];
        t2 = SIGMA0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    
    // Add to hash values
    ctx->h[0] += a;
    ctx->h[1] += b;
    ctx->h[2] += c;
    ctx->h[3] += d;
    ctx->h[4] += e;
    ctx->h[5] += f;
    ctx->h[6] += g;
    ctx->h[7] += h;
}

// Update context with input data
void sha256_update(SHA256_CTX *ctx, const uint8_t *data, size_t length) {
    size_t i = 0;
    
    ctx->total_length += length;
    
    // Fill the buffer
    while (i < length) {
        size_t remaining = SHA256_BLOCK_SIZE - ctx->buffer_length;
        size_t to_copy = (length - i < remaining) ? (length - i) : remaining;
        
        memcpy(ctx->buffer + ctx->buffer_length, data + i, to_copy);
        ctx->buffer_length += to_copy;
        i += to_copy;
        
        // Process block if buffer is full
        if (ctx->buffer_length == SHA256_BLOCK_SIZE) {
            sha256_process_block(ctx, ctx->buffer);
            ctx->buffer_length = 0;
        }
    }
}

// Finalize and produce digest
void sha256_final(SHA256_CTX *ctx, uint8_t *digest) {
    uint64_t total_bits = ctx->total_length * 8;
    unsigned int padlen = (ctx->buffer_length < 56) ? (56 - ctx->buffer_length) : (120 - ctx->buffer_length);
    
    uint8_t pad[1] = {0x80};
    sha256_update(ctx, pad, 1);
    
    // Pad with zeros
    uint8_t zero = 0;
    for (unsigned int i = 1; i < padlen; i++) {
        sha256_update(ctx, &zero, 1);
    }
    
    // Append length
    uint8_t len_bytes[8];
    for (int i = 7; i >= 0; i--) {
        len_bytes[i] = (uint8_t)(total_bits & 0xff);
        total_bits >>= 8;
    }
    sha256_update(ctx, len_bytes, 8);
    
    // Output hash
    for (int i = 0; i < 8; i++) {
        for (int j = 3; j >= 0; j--) {
            *digest++ = (uint8_t)((ctx->h[i] >> (j * 8)) & 0xff);
        }
    }
}

// Convenience function for one-shot hashing
void sha256(const uint8_t *data, size_t length, uint8_t *digest) {
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, length);
    sha256_final(&ctx, digest);
}
