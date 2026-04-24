#include "ecc_point.h"

// secp256k1 parameter initialization
void secp256k1_init_params() {
    // p = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
    big_int_from_hex_string(&secp256k1_p, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
    
    // n = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141
    big_int_from_hex_string(&secp256k1_n, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");
    
    // Gx = 0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
    big_int_from_hex_string(&secp256k1_Gx, "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798");
    
    // Gy = 0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8
    big_int_from_hex_string(&secp256k1_Gy, "483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8");

    // Note: The above hex strings are the standard parameters for secp256k1 curve
    
    // Implementation Note
    // p, n, Gx, Gy를 Little-Endian으로 4덩어리씩 끊어서 저장할 수도 있지만, 
    // 9C47D08FFB10D4B8
    // A6855419FD17B448
    // 0E1108A85DA4FBFC
    // 26A3C465483ADA77 -> 이렇게 저장하면 limbs[0] = 9C47D08FFB10D4B8로 구현되어
    // 정수 크기 관련 오류가 생길 수 있음.
    // 따라서, big_int_from_hex_string 함수로 전체를 한 번에 처리하는 것이 구현상 더 좋음.
}
