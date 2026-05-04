#ifndef ECDSA_H
#define ECDSA_H

#include "big_int.h"
#include "ecc_point.h"
#include "sha256.h"

// ECDSA Key Pair structure
typedef struct {
    BigInt private_key;                // d (private key)
    ECCPoint public_key;              // Q = d*G (public key)
} ECDSAKeyPair;

// ECDSA Signature structure (r, s)
typedef struct {
    BigInt r;
    BigInt s;
} ECDSASignature;

// Key generation
// Input: None (uses random generation or fixed seed for testing)
// Output: KeyPair (private_key d, public_key Q = d*G)
void ecdsa_key_generate(ECDSAKeyPair *keypair, const BigInt *private_key);

// Signature generation
// Input: message (as bytes), message_len (length in bytes), keypair
// Output: signature (r, s)
int ecdsa_sign(ECDSASignature *signature, const uint8_t *message, size_t message_len, const ECDSAKeyPair *keypair);

// Signature verification
// Input: message, message_len, signature, public_key
// Output: 1 if valid, 0 if invalid
int ecdsa_verify(const uint8_t *message, size_t message_len, const ECDSASignature *signature, const ECCPoint *public_key);

// Utility functions
void ecdsa_signature_print(const ECDSASignature *sig);
void ecdsa_keypair_print(const ECDSAKeyPair *keypair);

#endif
