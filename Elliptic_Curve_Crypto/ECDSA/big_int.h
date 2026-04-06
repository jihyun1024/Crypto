#ifndef BIG_INT_H
#define BIG_INT_H

#include <stdint.h>
#include <string.h>

#define LIMB_SIZE 8          // 64-bit limbs
#define NUM_LIMBS 4          // 4 * 64 = 256 bits
#define LIMB_BITS 64

typedef uint64_t limb_t;

// 256-bit unsigned integer represented as array of 64-bit limbs
typedef struct {
    limb_t limbs[NUM_LIMBS];
} BigInt;

// Initialization
void big_int_zero(BigInt *a);
void big_int_from_hex_string(BigInt *a, const char *hex_str);
void big_int_from_uint64(BigInt *a, uint64_t val);
void big_int_copy(BigInt *dest, const BigInt *src);

// Comparison
int big_int_compare(const BigInt *a, const BigInt *b);  // returns -1, 0, 1
int big_int_is_zero(const BigInt *a);
int big_int_is_equal(const BigInt *a, const BigInt *b);
int big_int_is_less(const BigInt *a, const BigInt *b);

// Arithmetic operations
void big_int_add(BigInt *result, const BigInt *a, const BigInt *b);
void big_int_sub(BigInt *result, const BigInt *a, const BigInt *b);
void big_int_mul(BigInt *result, const BigInt *a, const BigInt *b);
void big_int_mod(BigInt *result, const BigInt *a, const BigInt *modulus);
void big_int_mod_add(BigInt *result, const BigInt *a, const BigInt *b, const BigInt *modulus);
void big_int_mod_sub(BigInt *result, const BigInt *a, const BigInt *b, const BigInt *modulus);
void big_int_mod_mul(BigInt *result, const BigInt *a, const BigInt *b, const BigInt *modulus);

// Modular inverse using Extended Euclidean Algorithm
int big_int_mod_inv(BigInt *result, const BigInt *a, const BigInt *modulus);

// Bit operations
void big_int_left_shift(BigInt *result, const BigInt *a, int shift_bits);
void big_int_right_shift(BigInt *result, const BigInt *a, int shift_bits);

// Conversion and printing
void big_int_to_hex_string(const BigInt *a, char *hex_str, int max_len);
void big_int_print(const BigInt *a);
void big_int_print_hex(const BigInt *a);

#endif
