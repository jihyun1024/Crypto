#include "ecc_point.h"
#include <stdio.h>
#include <string.h>

// secp256k1 curve parameters
// y^2 = x^3 + 7 (mod p)

BigInt secp256k1_p;
BigInt secp256k1_n;
BigInt secp256k1_Gx;
BigInt secp256k1_Gy;

// Initialize secp256k1 parameters
void secp256k1_init() {
    static int initialized = 0;
    if (initialized) return;
    initialized = 1;
    
    // p = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
    big_int_from_hex_string(&secp256k1_p, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFF FC2F");
    
    // n = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141
    big_int_from_hex_string(&secp256k1_n, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");
    
    // Gx = 0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
    big_int_from_hex_string(&secp256k1_Gx, "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798");
    
    // Gy = 0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8
    big_int_from_hex_string(&secp256k1_Gy, "483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8");
}

// Initialize point to infinity
void ecc_point_init(ECCPoint *p) {
    big_int_zero(&p->x);
    big_int_zero(&p->y);
    p->is_infinity = 1;
}

// Set point coordinates
void ecc_point_set(ECCPoint *p, const BigInt *x, const BigInt *y) {
    big_int_copy(&p->x, x);
    big_int_copy(&p->y, y);
    p->is_infinity = 0;
}

// Set point to infinity
void ecc_point_set_infinity(ECCPoint *p) {
    big_int_zero(&p->x);
    big_int_zero(&p->y);
    p->is_infinity = 1;
}

// Copy point
void ecc_point_copy(ECCPoint *dest, const ECCPoint *src) {
    big_int_copy(&dest->x, &src->x);
    big_int_copy(&dest->y, &src->y);
    dest->is_infinity = src->is_infinity;
}

// Check if point is at infinity
int ecc_point_is_infinity(const ECCPoint *p) {
    return p->is_infinity;
}

// Check if point is on the curve: y^2 = x^3 + 7 (mod p)
int ecc_point_is_valid(const ECCPoint *p) {
    if (p->is_infinity) {
        return 1;
    }
    
    // Calculate y^2 mod p
    BigInt y_squared;
    big_int_mod_mul(&y_squared, &p->y, &p->y, &secp256k1_p);
    
    // Calculate x^3 mod p
    BigInt x_squared;
    big_int_mod_mul(&x_squared, &p->x, &p->x, &secp256k1_p);
    BigInt x_cubed;
    big_int_mod_mul(&x_cubed, &x_squared, &p->x, &secp256k1_p);
    
    // Calculate x^3 + 7 mod p
    BigInt seven;
    big_int_from_uint64(&seven, 7);
    BigInt rhs;
    big_int_mod_add(&rhs, &x_cubed, &seven, &secp256k1_p);
    
    // Check if y^2 = x^3 + 7
    return big_int_is_equal(&y_squared, &rhs);
}

// Check if two points are equal
int ecc_point_is_equal(const ECCPoint *p1, const ECCPoint *p2) {
    if (p1->is_infinity && p2->is_infinity) {
        return 1;
    }
    if (p1->is_infinity || p2->is_infinity) {
        return 0;
    }
    return big_int_is_equal(&p1->x, &p2->x) && big_int_is_equal(&p1->y, &p2->y);
}

// Point doubling: P + P = 2P
// For P = (x, y), 2P = (x', y') where
// lambda = (3*x^2) / (2*y)
// x' = lambda^2 - 2*x
// y' = lambda*(x - x') - y
void ecc_point_double(ECCPoint *result, const ECCPoint *p) {
    if (p->is_infinity) {
        ecc_point_set_infinity(result);
        return;
    }
    
    // Check if y = 0
    if (big_int_is_zero(&p->y)) {
        ecc_point_set_infinity(result);
        return;
    }
    
    // Calculate 3*x^2
    BigInt three;
    big_int_from_uint64(&three, 3);
    BigInt x_squared;
    big_int_mod_mul(&x_squared, &p->x, &p->x, &secp256k1_p);
    BigInt three_x_squared;
    big_int_mod_mul(&three_x_squared, &three, &x_squared, &secp256k1_p);
    
    // Calculate 2*y
    BigInt two;
    big_int_from_uint64(&two, 2);
    BigInt two_y;
    big_int_mod_mul(&two_y, &two, &p->y, &secp256k1_p);
    
    // Calculate lambda = (3*x^2) / (2*y) = (3*x^2) * (2*y)^(-1)
    BigInt two_y_inv;
    big_int_mod_inv(&two_y_inv, &two_y, &secp256k1_p);
    BigInt lambda;
    big_int_mod_mul(&lambda, &three_x_squared, &two_y_inv, &secp256k1_p);
    
    // Calculate lambda^2
    BigInt lambda_squared;
    big_int_mod_mul(&lambda_squared, &lambda, &lambda, &secp256k1_p);
    
    // Calculate 2*x
    BigInt two_x;
    big_int_mod_mul(&two_x, &two, &p->x, &secp256k1_p);
    
    // x' = lambda^2 - 2*x
    BigInt new_x;
    big_int_mod_sub(&new_x, &lambda_squared, &two_x, &secp256k1_p);
    
    // y' = lambda*(x - x') - y
    BigInt x_minus_new_x;
    big_int_mod_sub(&x_minus_new_x, &p->x, &new_x, &secp256k1_p);
    BigInt lambda_times_diff;
    big_int_mod_mul(&lambda_times_diff, &lambda, &x_minus_new_x, &secp256k1_p);
    BigInt new_y;
    big_int_mod_sub(&new_y, &lambda_times_diff, &p->y, &secp256k1_p);
    
    ecc_point_set(result, &new_x, &new_y);
}

// Point addition: P + Q
// When x1 != x2:
// lambda = (y2 - y1) / (x2 - x1)
// x3 = lambda^2 - x1 - x2
// y3 = lambda*(x1 - x3) - y1
void ecc_point_add(ECCPoint *result, const ECCPoint *p1, const ECCPoint *p2) {
    if (p1->is_infinity) {
        ecc_point_copy(result, p2);
        return;
    }
    if (p2->is_infinity) {
        ecc_point_copy(result, p1);
        return;
    }
    
    // Check if x-coordinates are equal
    if (big_int_is_equal(&p1->x, &p2->x)) {
        if (big_int_is_equal(&p1->y, &p2->y)) {
            // P + P = 2P
            ecc_point_double(result, p1);
        } else {
            // P + (-P) = O
            ecc_point_set_infinity(result);
        }
        return;
    }
    
    // Calculate lambda = (y2 - y1) / (x2 - x1)
    BigInt dy;
    big_int_mod_sub(&dy, &p2->y, &p1->y, &secp256k1_p);
    
    BigInt dx;
    big_int_mod_sub(&dx, &p2->x, &p1->x, &secp256k1_p);
    
    BigInt dx_inv;
    big_int_mod_inv(&dx_inv, &dx, &secp256k1_p);
    
    BigInt lambda;
    big_int_mod_mul(&lambda, &dy, &dx_inv, &secp256k1_p);
    
    // Calculate lambda^2
    BigInt lambda_squared;
    big_int_mod_mul(&lambda_squared, &lambda, &lambda, &secp256k1_p);
    
    // x3 = lambda^2 - x1 - x2
    BigInt new_x;
    big_int_mod_sub(&new_x, &lambda_squared, &p1->x, &secp256k1_p);
    big_int_mod_sub(&new_x, &new_x, &p2->x, &secp256k1_p);
    
    // y3 = lambda*(x1 - x3) - y1
    BigInt x1_minus_x3;
    big_int_mod_sub(&x1_minus_x3, &p1->x, &new_x, &secp256k1_p);
    BigInt lambda_times_diff;
    big_int_mod_mul(&lambda_times_diff, &lambda, &x1_minus_x3, &secp256k1_p);
    BigInt new_y;
    big_int_mod_sub(&new_y, &lambda_times_diff, &p1->y, &secp256k1_p);
    
    ecc_point_set(result, &new_x, &new_y);
}

// Scalar multiplication: result = k * P using binary method
void ecc_point_scalar_mult(ECCPoint *result, const ECCPoint *p, const BigInt *scalar) {
    ecc_point_set_infinity(result);
    
    if (p->is_infinity) {
        return;
    }
    
    BigInt k;
    big_int_copy(&k, scalar);
    
    
    ECCPoint temp;
    ecc_point_copy(&temp, p);
    
    // Process each bit of scalar from LSB to MSB
    for (int i = 0; i < 256; i++) {
        if ((k.limbs[i / 64] >> (i % 64)) & 1) {
            ecc_point_add(result, result, &temp);
        }
        ecc_point_double(&temp, &temp);
    }
}

// Print point
void ecc_point_print(const ECCPoint *p) {
    if (p->is_infinity) {
        printf("(O)");
    } else {
        printf("(");
        big_int_print_hex(&p->x);
        printf(", ");
        big_int_print_hex(&p->y);
        printf(")");
    }
}
