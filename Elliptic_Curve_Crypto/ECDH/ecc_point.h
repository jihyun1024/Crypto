#ifndef ECC_POINT_H
#define ECC_POINT_H

#include "big_int.h"

// secp256k1 curve parameters
// y^2 = x^3 + 7 (mod p)
extern BigInt secp256k1_p;      // Prime field
extern BigInt secp256k1_n;      // Order of generator
extern BigInt secp256k1_Gx;     // Generator x-coordinate
extern BigInt secp256k1_Gy;     // Generator y-coordinate

typedef struct {
    BigInt x;
    BigInt y;
    int is_infinity;                   // Flag for point at infinity
} ECCPoint;

// Point initialization
void ecc_point_init(ECCPoint *p);
void ecc_point_set(ECCPoint *p, const BigInt *x, const BigInt *y);
void ecc_point_set_infinity(ECCPoint *p);
void ecc_point_copy(ECCPoint *dest, const ECCPoint *src);

// Point checking
int ecc_point_is_infinity(const ECCPoint *p);
int ecc_point_is_valid(const ECCPoint *p);
int ecc_point_is_equal(const ECCPoint *p1, const ECCPoint *p2);

// Point operations on secp256k1
void ecc_point_double(ECCPoint *result, const ECCPoint *p);
void ecc_point_add(ECCPoint *result, const ECCPoint *p1, const ECCPoint *p2);
void ecc_point_scalar_mult(ECCPoint *result, const ECCPoint *p, const BigInt *scalar);

// Initialize secp256k1 parameters
void secp256k1_init();

// Utility
void ecc_point_print(const ECCPoint *p);

#endif
