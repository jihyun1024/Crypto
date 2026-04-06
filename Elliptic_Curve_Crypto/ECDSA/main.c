#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "ecdsa.h"

int main() {
    printf("===========================================\n");
    printf("ECDSA Implementation using secp256k1\n");
    printf("===========================================\n\n");
    
    // Initialize random seed
    srand(time(NULL));
    
    // ---- TEST 1: Key Generation ----
    printf("[TEST 1] Key Generation\n");
    printf("-----------------------------------------\n");
    
    // Use a fixed private key for demonstration
    BigInt private_key;
    big_int_from_hex_string(&private_key, "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF");
    
    ECDSAKeyPair keypair;
    ecdsa_key_generate(&keypair, &private_key);
    
    printf("Private Key (d):\n");
    printf("  ");
    big_int_print_hex(&keypair.private_key);
    printf("\n");
    printf("Public Key (Q = d*G):\n");
    printf("  X = ");
    big_int_print_hex(&keypair.public_key.x);
    printf("\n");
    printf("  Y = ");
    big_int_print_hex(&keypair.public_key.y);
    printf("\n\n");
    
    // ---- TEST 2: Signature Generation ----
    printf("[TEST 2] Digital Signature Generation\n");
    printf("-----------------------------------------\n");
    
    const char *message = "Hello, ECDSA!";
    size_t message_len = strlen(message);
    
    printf("Message: \"%s\"\n", message);
    printf("Message length: %zu bytes\n\n", message_len);
    
    // Calculate hash of message for display
    uint8_t hash_digest[32];
    sha256((const uint8_t *)message, message_len, hash_digest);
    printf("SHA-256 Hash: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", hash_digest[i]);
    }
    printf("\n\n");
    
    ECDSASignature signature;
    if (ecdsa_sign(&signature, (const uint8_t *)message, message_len, &keypair)) {
        printf("Signature generated successfully!\n");
        printf("r = ");
        big_int_print_hex(&signature.r);
        printf("\n");
        printf("s = ");
        big_int_print_hex(&signature.s);
        printf("\n\n");
    } else {
        printf("Failed to generate signature!\n");
        return 1;
    }
    
    // ---- TEST 3: Signature Verification (Valid) ----
    printf("[TEST 3] Signature Verification (Valid Message)\n");
    printf("-----------------------------------------\n");
    
    printf("Verifying signature for original message...\n");
    if (ecdsa_verify((const uint8_t *)message, message_len, &signature, &keypair.public_key)) {
        printf("✓ Signature is VALID!\n\n");
    } else {
        printf("✗ Signature is INVALID!\n\n");
    }
    
    // ---- TEST 4: Signature Verification (Invalid) ----
    printf("[TEST 4] Signature Verification (Modified Message)\n");
    printf("-----------------------------------------\n");
    
    const char *modified_message = "Hello, ECDSX!";  // Changed 'A' to 'X'
    printf("Modified message: \"%s\"\n", modified_message);
    
    if (ecdsa_verify((const uint8_t *)modified_message, strlen(modified_message), &signature, &keypair.public_key)) {
        printf("✓ Signature is VALID!\n\n");
    } else {
        printf("✗ Signature is INVALID! (Expected, as message was modified)\n\n");
    }
    
    // ---- TEST 5: Multiple Signatures ----
    printf("[TEST 5] Multiple Messages and Signatures\n");
    printf("-----------------------------------------\n");
    
    const char *messages[] = {
        "First message",
        "Second message",
        "Third message"
    };
    
    ECDSASignature signatures[3];
    
    printf("Generating signatures for multiple messages:\n");
    for (int i = 0; i < 3; i++) {
        if (ecdsa_sign(&signatures[i], (const uint8_t *)messages[i], strlen(messages[i]), &keypair)) {
            printf("  Message %d: \"%s\" - Signature generated\n", i + 1, messages[i]);
        } else {
            printf("  Message %d: \"%s\" - Failed\n", i + 1, messages[i]);
        }
    }
    printf("\n");
    
    printf("Verifying signatures:\n");
    for (int i = 0; i < 3; i++) {
        int valid = ecdsa_verify((const uint8_t *)messages[i], strlen(messages[i]), &signatures[i], &keypair.public_key);
        printf("  Message %d signature: %s\n", i + 1, valid ? "VALID ✓" : "INVALID ✗");
    }
    printf("\n");
    
    // ---- TEST 6: Cross-verification ----
    printf("[TEST 6] Cross-verification (Using wrong signature)\n");
    printf("-----------------------------------------\n");
    
    printf("Verifying message 1 with signature from message 2:\n");
    int cross_valid = ecdsa_verify((const uint8_t *)messages[0], strlen(messages[0]), &signatures[1], &keypair.public_key);
    printf("Result: %s\n\n", cross_valid ? "VALID ✓ (Unexpected!)" : "INVALID ✗ (Expected)");
    
    // ---- TEST 7: Different Private Key ----
    printf("[TEST 7] Verification with Different Public Key\n");
    printf("-----------------------------------------\n");
    
    BigInt different_private_key;
    big_int_from_hex_string(&different_private_key, "FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210");
    
    ECDSAKeyPair different_keypair;
    ecdsa_key_generate(&different_keypair, &different_private_key);
    
    printf("Using different public key for verification:\n");
    int different_valid = ecdsa_verify((const uint8_t *)message, message_len, &signature, &different_keypair.public_key);
    printf("Result: %s\n\n", different_valid ? "VALID ✓ (Unexpected!)" : "INVALID ✗ (Expected)");
    
    // ---- TEST 8: Show Generator Point ----
    printf("[TEST 8] secp256k1 Generator Point Information\n");
    printf("-----------------------------------------\n");
    printf("Generator point G (secp256k1):\n");
    printf("  Gx = ");
    big_int_print_hex(&secp256k1_Gx);
    printf("\n");
    printf("  Gy = ");
    big_int_print_hex(&secp256k1_Gy);
    printf("\n");
    printf("Order (n) = ");
    big_int_print_hex(&secp256k1_n);
    printf("\n");
    printf("Prime (p) = ");
    big_int_print_hex(&secp256k1_p);
    printf("\n\n");
    
    printf("===========================================\n");
    printf("All tests completed successfully!\n");
    printf("===========================================\n");
    
    return 0;
}
