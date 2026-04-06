#include "ecc_point.h"

// secp256k1 parameter initialization
void secp256k1_init_params() {
    // p = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
    // Split as limbs[0..3] from LSB to MSB
    secp256k1_p.limbs[0] = 0xFFFFFFFFEFFFFFC2FUL;
    secp256k1_p.limbs[1] = 0xFFFFFFFFFFFFFFFFUL;
    secp256k1_p.limbs[2] = 0xFFFFFFFFFFFFFFFFUL;
    secp256k1_p.limbs[3] = 0xFFFFFFFFFFFFFFFFUL;
    
    // n = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141
    // limbs[0] = 0xBFD25E8CD0364141 (bits 0-63)
    // limbs[1] = 0xBAAEDCE6AF48A03B (bits 64-127)
    // limbs[2] = 0xFFFFFFFFFFFFFFFF (bits 128-191)
    // limbs[3] = 0xFFFFFFFFFFFFFFFF (bits 192-255)
    // Wait, let's recalculate:
    // The upper part FFFFFFFE should be split across limbs[1] and limbs[2]
    // Actually looking at the hex: ...FFFFFFFE BAAEDCE6 AF48A03B BFD25E8C D0364141
    // 64-bit boundaries: D0364141 | BFD25E8C | AF48A03B | BAAEDCE6 | FFFFFFFE | (continues...)
    // No wait, we have exactly 8 groups of 8 hex digits (256 bits total)
    // From right to left: D0364141 BFD25E8C AF48A03B BAAEDCE6 FFFFFFFE FFFFFFFF FFFFFFFF FFFFFFFF
    // In 64-bit limbs (right to left): 
    // limbs[0] = BFD25E8C D0364141
    // limbs[1] = BAAEDCE6 AF48A03B (hmm still not quite right)
    
    // Let me think differently. The n value has 32 hex digits (256 bits, 8 digits per 64-bit)
    // FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE BAAEDCE6 AF48A03B BFD25E8C D0364141
    // Grouping by 64-bit (16 hex digits):
    // Group 0 (limbs[0]): BFD25E8C D0364141
    // Group 1 (limbs[1]): BAAEDCE6 AF48A03B
    // Group 2 (limbs[2]): FFFFFFFF FFFFFFFE
    // Group 3 (limbs[3]): FFFFFFFF FFFFFFFF
    
    secp256k1_n.limbs[0] = 0xBFD25E8CD0364141UL;
    secp256k1_n.limbs[1] = 0xBAAEDCE6AF48A03BUL;
    secp256k1_n.limbs[2] = 0xFFFFFFFFFFFFFFFEUL;
    secp256k1_n.limbs[3] = 0xFFFFFFFFFFFFFFFFUL;
    
    // Gx = 0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
    // 79BE667E F9DCBBAC 55A06295 CE870B07 029BFCDB 2DCE28D9 59F2815B 16F81798
    // Group 0 (limbs[0]): 59F2815B 16F81798
    // Group 1 (limbs[1]): 029BFCDB 2DCE28D9
    // Group 2 (limbs[2]): CE870B07 55A06295 - wait, this doesn't align
    
    // Let me re-organize thinking left to right in groups of 16 hex digits:
    // 79BE667E F9DCBBAC | 55A06295 CE870B07 | 029BFCDB 2DCE28D9 | 59F2815B 16F81798
    // High to low: 79BE667EF9DCBBAC | 55A06295CE870B07 | 029BFCDB2DCE28D9 | 59F2815B16F81798
    // In little-endian (limbs[0] is LSB):
    // limbs[0] = 0x59F2815B16F81798
    // limbs[1] = 0x029BFCDB2DCE28D9 - but this is only 29 bits...wait
    
    // I need to be more careful. Each limb is 64 bits = 16 hex digits
    // 79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
    // Split into 4 groups of 16 hex digits from right:
    // 59F2815B16F81798 (rightmost 16 digits)
    // 2DCE28D9029BFCDB (next 16 digits)
    // CE870B0755A06295 (next 16 digits)
    // 79BE667EF9DCBBAC (leftmost 16 digits)
    
    secp256k1_Gx.limbs[0] = 0x59F2815B16F81798UL;
    secp256k1_Gx.limbs[1] = 0x2DCE28D9029BFCDBUL;
    secp256k1_Gx.limbs[2] = 0xCE870B0755A06295UL;
    secp256k1_Gx.limbs[3] = 0x79BE667EF9DCBBACUL;
    
    // Gy = 0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8
    // 483ADA77 26A3C465 | 5DA4FBFC 0E1108A8 | FD17B448 A6855419 | 9C47D08F FB10D4B8
    // Split into 4 groups of 16 hex from right:
    // 9C47D08FFB10D4B8 (rightmost 16)
    // A6855419FD17B448 (next 16)
    // 0E1108A85DA4FBFC (next 16)
    // 26A3C465483ADA77 (leftmost 16)
    
    secp256k1_Gy.limbs[0] = 0x9C47D08FFB10D4B8UL;
    secp256k1_Gy.limbs[1] = 0xA6855419FD17B448UL;
    secp256k1_Gy.limbs[2] = 0x0E1108A85DA4FBFCUL;
    secp256k1_Gy.limbs[3] = 0x26A3C465483ADA77UL;
}
