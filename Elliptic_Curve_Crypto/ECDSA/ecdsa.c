#include "ecdsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Simple pseudo-random number generator (for demonstration)
// In production, use a cryptographically secure RNG
static uint64_t prng_state = 0;

static uint64_t simple_rand() {
    prng_state = prng_state * 6364136223846793005ULL + 1442695040888963407ULL;
    return prng_state;
}

static void seed_rand(uint64_t seed) {
    prng_state = seed;
}

// Generate random BigInt less than n
static void random_bigint_less_than(BigInt *result, const BigInt *n) {
    // Simple method: generate random and take modulo
    for (int i = 0; i < NUM_LIMBS; i++) {
        result->limbs[i] = simple_rand();
    }
    big_int_mod(result, result, n);
    
    // Ensure it's not zero
    if (big_int_is_zero(result)) {
        big_int_from_uint64(result, 1);
    }
}

// Hash message and convert to BigInt
static void hash_to_bigint(BigInt *hash_int, const uint8_t *message, size_t message_len) {
    uint8_t hash_digest[SHA256_DIGEST_LENGTH];
    sha256(message, message_len, hash_digest);
    
    // Convert first 32 bytes of hash to BigInt
    // Hash is in big-endian format, we need to convert to BigInt format
    big_int_zero(hash_int);
    
    for (int i = 0; i < 32; i++) {
        int limb_idx = i / 8;
        int byte_pos = (7 - (i % 8));
        hash_int->limbs[limb_idx] |= ((limb_t)hash_digest[i]) << (byte_pos * 8);
    }
}

// Key generation: Generate random d and compute Q = d*G
void ecdsa_key_generate(ECDSAKeyPair *keypair, const BigInt *private_key) {
    // Set private key
    big_int_copy(&keypair->private_key, private_key);
    
    // Compute public key Q = d*G
    ECCPoint generator;
    ecc_point_set(&generator, &secp256k1_Gx, &secp256k1_Gy);
    
    ecc_point_scalar_mult(&keypair->public_key, &generator, private_key);
}

// ECDSA Signing
// Input: Hash of message (should be hashed externally or here)
// Algorithm:
// 1. z = hash(m)
// 2. k = random number in [1, n-1]
// 3. (x, y) = k*G
// 4. r = x mod n
// 5. s = k^(-1) * (z + r*d) mod n
// 6. If r or s is 0, go to step 2
int ecdsa_sign(ECDSASignature *signature, const uint8_t *message, size_t message_len, const ECDSAKeyPair *keypair) {
    BigInt z;
    hash_to_bigint(&z, message, message_len);
    
    ECCPoint generator;
    ecc_point_set(&generator, &secp256k1_Gx, &secp256k1_Gy);
    
    // Retry loop for signature generation
    for (int attempt = 0; attempt < 10; attempt++) {
        // Generate random k
        BigInt k;
        random_bigint_less_than(&k, &secp256k1_n);
        
        // Compute k*G
        ECCPoint kg_point;
        ecc_point_scalar_mult(&kg_point, &generator, &k);
        
        if (kg_point.is_infinity) {
            continue;
        }
        
        // r = x mod n
        big_int_mod(&signature->r, &kg_point.x, &secp256k1_n);
        
        if (big_int_is_zero(&signature->r)) {
            continue;
        }
        
        // Compute k^(-1)
        BigInt k_inv;
        if (!big_int_mod_inv(&k_inv, &k, &secp256k1_n)) {
            continue;
        }
        
        // Compute r*d
        BigInt rd;
        big_int_mod_mul(&rd, &signature->r, &keypair->private_key, &secp256k1_n);
        
        // Compute z + r*d
        BigInt z_plus_rd;
        big_int_mod_add(&z_plus_rd, &z, &rd, &secp256k1_n);
        
        // Compute s = k^(-1) * (z + r*d) mod n
        big_int_mod_mul(&signature->s, &k_inv, &z_plus_rd, &secp256k1_n);
        
        if (big_int_is_zero(&signature->s)) {
            continue;
        }
        
        return 1;  // Success
    }
    
    return 0;  // Failed to generate valid signature
}

// ECDSA Verification
// Algorithm:
// 1. Check if r and s are in [1, n-1]
// 2. z = hash(m)
// 3. w = s^(-1) mod n
// 4. u1 = z*w mod n
// 5. u2 = r*w mod n
// 6. (x, y) = u1*G + u2*Q
// 7. v = x mod n
// 8. Accept if v == r
int ecdsa_verify(const uint8_t *message, size_t message_len, const ECDSASignature *signature, const ECCPoint *public_key) {
    // Check if r and s are in valid range [1, n-1]
    BigInt one;
    big_int_from_uint64(&one, 1);
    
    if (big_int_is_less(&signature->r, &one) || big_int_compare(&signature->r, &secp256k1_n) >= 0) {
        return 0;
    }
    if (big_int_is_less(&signature->s, &one) || big_int_compare(&signature->s, &secp256k1_n) >= 0) {
        return 0;
    }
    
    // Hash the message
    BigInt z;
    hash_to_bigint(&z, message, message_len);
    
    // Compute s^(-1)
    BigInt s_inv;
    if (!big_int_mod_inv(&s_inv, &signature->s, &secp256k1_n)) {
        return 0;
    }
    
    // Compute u1 = z * s^(-1) mod n
    BigInt u1;
    big_int_mod_mul(&u1, &z, &s_inv, &secp256k1_n);
    
    // Compute u2 = r * s^(-1) mod n
    BigInt u2;
    big_int_mod_mul(&u2, &signature->r, &s_inv, &secp256k1_n);
    
    // Compute u1*G
    ECCPoint generator;
    ecc_point_set(&generator, &secp256k1_Gx, &secp256k1_Gy);
    
    ECCPoint u1g;
    ecc_point_scalar_mult(&u1g, &generator, &u1);
    
    // Compute u2*Q
    ECCPoint u2q;
    ecc_point_scalar_mult(&u2q, public_key, &u2);
    
    // Compute (x, y) = u1*G + u2*Q
    ECCPoint pt;
    ecc_point_add(&pt, &u1g, &u2q);
    
    if (pt.is_infinity) {
        return 0;
    }
    
    // Compute v = x mod n
    BigInt v;
    big_int_mod(&v, &pt.x, &secp256k1_n);
    
    // Check if v == r
    return big_int_is_equal(&v, &signature->r);
}

// Print signature
void ecdsa_signature_print(const ECDSASignature *sig) {
    printf("Signature:\n");
    printf("  r = ");
    big_int_print_hex(&sig->r);
    printf("\n");
    printf("  s = ");
    big_int_print_hex(&sig->s);
    printf("\n");
}

// Print keypair
void ecdsa_keypair_print(const ECDSAKeyPair *keypair) {
    printf("KeyPair:\n");
    printf("  Private Key (d) = ");
    big_int_print_hex(&keypair->private_key);
    printf("\n");
    printf("  Public Key (Q) = ");
    ecc_point_print(&keypair->public_key);
    printf("\n");
}
