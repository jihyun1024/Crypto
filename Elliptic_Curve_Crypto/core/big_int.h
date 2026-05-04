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
void big_int_zero(BigInt *a); // 0으로 상수 초기화
void big_int_from_hex_string(BigInt *a, const char *hex_str); // 16진수 문자열에서 추출해서 a로 초기화
void big_int_from_uint64(BigInt *a, uint64_t val); // 64비트 정수에서 추출해서 a로 초기화
void big_int_copy(BigInt *dest, const BigInt *src); // src에서 dest로 복사

// Comparison
int big_int_compare(const BigInt *a, const BigInt *b);  // returns -1, 0, 1
int big_int_is_zero(const BigInt *a); // a가 0인지 확인 -> 1, 아니면 0
int big_int_is_equal(const BigInt *a, const BigInt *b); // a와 b가 같은지 확인 -> 1, 아니면 0
int big_int_is_less(const BigInt *a, const BigInt *b); // a가 b보다 작은지 확인 -> 1, 아니면 0

// Arithmetic operations (modular 연산은 별도의 함수로 구현)
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
void big_int_left_shift(BigInt *result, const BigInt *a, int shift_bits); // a를 shift_bits만큼 왼쪽으로 시프트해서 result에 저장
void big_int_right_shift(BigInt *result, const BigInt *a, int shift_bits); // a를 shift_bits만큼 오른쪽으로 시프트해서 result에 저장

// Conversion and printing
void big_int_to_hex_string(const BigInt *a, char *hex_str, int max_len); // a에서 추출해서 16진수 문자열로 저장
void big_int_print(const BigInt *a); // a를 정수 형태로 출력
void big_int_print_hex(const BigInt *a); // a를 16진수 형태로 출력

#endif
