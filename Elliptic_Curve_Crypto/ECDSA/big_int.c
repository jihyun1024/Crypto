#include "big_int.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Initialize BigInt to zero
void big_int_zero(BigInt *a) {
    for (int i = 0; i < NUM_LIMBS; i++) {
        a->limbs[i] = 0;
    }
}

// Convert hex string to BigInt
// Hex string should be in big-endian format (most significant digit first)
void big_int_from_hex_string(BigInt *a, const char *hex_str) {
    big_int_zero(a);
    
    int len = strlen(hex_str);
    
    // Skip "0x" prefix if present
    int start = 0;
    if (len > 2 && hex_str[0] == '0' && (hex_str[1] == 'x' || hex_str[1] == 'X')) {
        start = 2;
        len -= 2;
    }
    
    // Process hex string from left to right (big-endian)
    for (int i = 0; i < len; i++) {
        char c = hex_str[start + i];
        int digit;
        
        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            digit = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            digit = c - 'A' + 10;
        } else {
            continue;
        }
        
        // Shift left and add digit
        int carry = digit;
        for (int j = 0; j < NUM_LIMBS; j++) {
            __uint128_t temp = ((__uint128_t)a->limbs[j] << 4) + carry;
            a->limbs[j] = (limb_t)temp;
            carry = (int)(temp >> 64);
        }
    }
}

// Initialize from uint64
void big_int_from_uint64(BigInt *a, uint64_t val) {
    big_int_zero(a);
    a->limbs[0] = val;
}

// Copy
void big_int_copy(BigInt *dest, const BigInt *src) {
    for (int i = 0; i < NUM_LIMBS; i++) {
        dest->limbs[i] = src->limbs[i];
    }
}

// Compare: returns -1 if a < b, 0 if a == b, 1 if a > b
int big_int_compare(const BigInt *a, const BigInt *b) {
    for (int i = NUM_LIMBS - 1; i >= 0; i--) {
        if (a->limbs[i] < b->limbs[i]) return -1;
        if (a->limbs[i] > b->limbs[i]) return 1;
    }
    return 0;
}

int big_int_is_zero(const BigInt *a) {
    for (int i = 0; i < NUM_LIMBS; i++) {
        if (a->limbs[i] != 0) return 0;
    }
    return 1;
}

int big_int_is_equal(const BigInt *a, const BigInt *b) {
    return big_int_compare(a, b) == 0;
}

int big_int_is_less(const BigInt *a, const BigInt *b) {
    return big_int_compare(a, b) < 0;
}

// Addition with carry
void big_int_add(BigInt *result, const BigInt *a, const BigInt *b) {
    limb_t carry = 0;
    
    for (int i = 0; i < NUM_LIMBS; i++) {
        __uint128_t sum = (__uint128_t)a->limbs[i] + b->limbs[i] + carry;
        result->limbs[i] = (limb_t)sum;
        carry = (limb_t)(sum >> 64);
    }
}

// Subtraction with borrow
void big_int_sub(BigInt *result, const BigInt *a, const BigInt *b) {
    limb_t borrow = 0;
    
    for (int i = 0; i < NUM_LIMBS; i++) {
        limb_t a_val = a->limbs[i];
        limb_t b_val = b->limbs[i] + borrow;
        
        if (a_val < b_val) {
            // Underflow: borrow from next limb
            result->limbs[i] = (~b_val) + a_val + 1;  // Two's complement way
            borrow = 1;
        } else {
            result->limbs[i] = a_val - b_val;
            borrow = 0;
        }
    }
}

// Multiplication with result reduction (keeps only lower 256 bits)
void big_int_mul(BigInt *result, const BigInt *a, const BigInt *b) {
    limb_t res[2 * NUM_LIMBS];
    
    // Initialize result array
    for (int i = 0; i < 2 * NUM_LIMBS; i++) {
        res[i] = 0;
    }
    
    // School multiplication
    for (int i = 0; i < NUM_LIMBS; i++) {
        limb_t carry = 0;
        for (int j = 0; j < NUM_LIMBS; j++) {
            __uint128_t prod = (__uint128_t)a->limbs[i] * b->limbs[j];
            prod += res[i + j] + carry;
            res[i + j] = (limb_t)prod;
            carry = (limb_t)(prod >> 64);
        }
        res[i + NUM_LIMBS] = carry;
    }
    
    // Keep only lower 256 bits
    for (int i = 0; i < NUM_LIMBS; i++) {
        result->limbs[i] = res[i];
    }
}

// Modulo operation
void big_int_mod(BigInt *result, const BigInt *a, const BigInt *modulus) {
    BigInt temp;
    big_int_copy(&temp, a);
    
    // Simple modulo for small modulus (works for secp256k1 prime)
    // For large numbers, we need multiple subtractions
    while (big_int_compare(&temp, modulus) >= 0) {
        BigInt temp2;
        big_int_sub(&temp2, &temp, modulus);
        big_int_copy(&temp, &temp2);
    }
    
    big_int_copy(result, &temp);
}

// Modular addition
void big_int_mod_add(BigInt *result, const BigInt *a, const BigInt *b, const BigInt *modulus) {
    BigInt sum;
    big_int_add(&sum, a, b);
    big_int_mod(result, &sum, modulus);
}

// Modular subtraction
void big_int_mod_sub(BigInt *result, const BigInt *a, const BigInt *b, const BigInt *modulus) {
    if (big_int_compare(a, b) >= 0) {
        big_int_sub(result, a, b);
    } else {
        BigInt temp;
        big_int_sub(&temp, b, a);
        big_int_sub(result, modulus, &temp);
    }
}

// Modular multiplication
void big_int_mod_mul(BigInt *result, const BigInt *a, const BigInt *b, const BigInt *modulus) {
    BigInt prod;
    big_int_mul(&prod, a, b);
    big_int_mod(result, &prod, modulus);
}

// Extended Euclidean Algorithm for modular inverse
int big_int_mod_inv(BigInt *result, const BigInt *a, const BigInt *modulus) {
    BigInt m, x, y;
    big_int_copy(&m, modulus);
    big_int_zero(&x);
    big_int_from_uint64(&x, 1);
    big_int_zero(&y);
    
    BigInt a_copy, m_copy;
    big_int_copy(&a_copy, a);
    big_int_copy(&m_copy, modulus);
    
    if (big_int_is_zero(&a_copy)) {
        return 0;  // No inverse
    }
    
    while (!big_int_is_zero(&a_copy)) {
        // For simplicity, we'll use a simpler approach
        // This is a basic implementation - for production, use better algorithms
        
        // Calculate quotient q = m / a (simplified - works for our use case)
        BigInt q, r;
        big_int_copy(&r, &m_copy);
        big_int_zero(&q);
        
        while (big_int_compare(&r, &a_copy) >= 0) {
            BigInt temp;
            big_int_sub(&temp, &r, &a_copy);
            big_int_copy(&r, &temp);
            
            BigInt one;
            big_int_from_uint64(&one, 1);
            BigInt temp2;
            big_int_add(&temp2, &q, &one);
            big_int_copy(&q, &temp2);
        }
        
        // Update m and x
        BigInt temp;
        big_int_mul(&temp, &q, &x);
        BigInt new_m;
        big_int_sub(&new_m, &m_copy, &temp);
        big_int_copy(&m_copy, &x);
        big_int_copy(&x, &new_m);
        
        // Update a and m
        big_int_copy(&m_copy, &a_copy);
        big_int_copy(&a_copy, &r);
    }
    
    if (!big_int_is_equal(&m_copy, modulus->limbs[0] == 1 ? modulus : &m_copy)) {
        return 0;  // No inverse
    }
    
    if (big_int_compare(&x, &m_copy) < 0) {
        BigInt temp;
        big_int_add(&temp, &x, modulus);
        big_int_copy(result, &temp);
    } else {
        big_int_copy(result, &x);
    }
    
    return 1;
}

// Left shift
void big_int_left_shift(BigInt *result, const BigInt *a, int shift_bits) {
    if (shift_bits <= 0) {
        big_int_copy(result, a);
        return;
    }
    
    int limb_shift = shift_bits / 64;
    int bit_shift = shift_bits % 64;
    
    big_int_zero(result);
    
    for (int i = 0; i < NUM_LIMBS; i++) {
        if (i + limb_shift < NUM_LIMBS) {
            result->limbs[i + limb_shift] |= a->limbs[i] << bit_shift;
        }
        if (bit_shift > 0 && i + limb_shift + 1 < NUM_LIMBS) {
            result->limbs[i + limb_shift + 1] |= a->limbs[i] >> (64 - bit_shift);
        }
    }
}

// Right shift
void big_int_right_shift(BigInt *result, const BigInt *a, int shift_bits) {
    if (shift_bits <= 0) {
        big_int_copy(result, a);
        return;
    }
    
    int limb_shift = shift_bits / 64;
    int bit_shift = shift_bits % 64;
    
    big_int_zero(result);
    
    for (int i = limb_shift; i < NUM_LIMBS; i++) {
        result->limbs[i - limb_shift] |= a->limbs[i] >> bit_shift;
        if (bit_shift > 0 && i + 1 < NUM_LIMBS) {
            result->limbs[i - limb_shift] |= a->limbs[i + 1] << (64 - bit_shift);
        }
    }
}

// Convert to hex string
void big_int_to_hex_string(const BigInt *a, char *hex_str, int max_len) {
    int pos = 0;
    int found_nonzero = 0;
    
    for (int i = NUM_LIMBS - 1; i >= 0 && pos < max_len - 1; i--) {
        uint64_t limb = a->limbs[i];
        for (int j = 60; j >= 0; j -= 4) {
            uint8_t digit = (limb >> j) & 0xF;
            if (digit != 0) found_nonzero = 1;
            if (found_nonzero) {
                if (pos < max_len - 1) {
                    hex_str[pos++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
                }
            }
        }
    }
    
    if (pos == 0) {
        hex_str[pos++] = '0';
    }
    hex_str[pos] = '\0';
}

// Print in decimal
void big_int_print(const BigInt *a) {
    char hex_str[100];
    big_int_to_hex_string(a, hex_str, 100);
    printf("%s", hex_str);
}

// Print in hex
void big_int_print_hex(const BigInt *a) {
    printf("0x");
    big_int_print(a);
}
