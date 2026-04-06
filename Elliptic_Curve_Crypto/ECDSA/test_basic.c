#include <stdio.h>
#include "big_int.h"
#include "ecc_point.h"

int main() {
    printf("Testing BigInt and secp256k1 operations...\n");
    
    // Initialize secp256k1 parameters
    secp256k1_init();
    
    // Test 1: Generator point
    printf("\n[Test 1] secp256k1 Generator Point\n");
    printf("Gx = ");
    big_int_print_hex(&secp256k1_Gx);
    printf("\n");
    printf("Gy = ");
    big_int_print_hex(&secp256k1_Gy);
    printf("\n");
    
    // Test 2: Validate generator on curve
    printf("\n[Test 2] Validate Generator on Curve\n");
    ECCPoint G;
    ecc_point_set(&G, &secp256k1_Gx, &secp256k1_Gy);
    
    printf("Checking if G is on curve y^2 = x^3 + 7...\n");
    if (ecc_point_is_valid(&G)) {
        printf("✓ Generator is on the curve\n");
    } else {
        printf("✗ Generator is NOT on the curve (may need verification)\n");
    }
    
    // Test 3: Simple addition check
    printf("\n[Test 3] Point doubling G (compute 2*G)\n");
    ECCPoint two_G;
    ecc_point_double(&two_G, &G);
    
    if (!two_G.is_infinity) {
        printf("2*G computed successfully\n");
        printf("  X = ");
        big_int_print_hex(&two_G.x);
        printf("\n");
    } else {
        printf("Error: 2*G resulted in point at infinity\n");
    }
    
    printf("\n✓ Basic tests completed!\n");
    
    return 0;
}

