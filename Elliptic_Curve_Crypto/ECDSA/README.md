# ECDSA (Elliptic Curve Digital Signature Algorithm) Implementation

secp256k1 곡선을 사용한 ECDSA 알고리즘의 C 구현입니다. 외부 라이브러리 없이 순수 C로 키 생성, 서명, 검증 기능을 구현했습니다.

## 개요

### 지원 곡선
- **secp256k1**: `y² = x³ + 7 (mod p)`
  - 소수 p: `2²⁵⁶ - 2³² - 977`
  - 생성점 G와 위수 n 정의됨
  - Bitcoin, Ethereum 등에서 사용

### 해시 알고리즘
- **SHA-256**: 메시지 다이제스트 생성 및 서명에 사용

### 인스턴스중 구성요소
- **big_int.h/c**: 256비트 정수 연산 (4개의 64비트 limb 사용)
- **ecc_point.h/c**: 타원곡선 점 연산 (덧셈, 스칼라 곱셈)
- **ecdsa.h/c**: ECDSA 알고리즘 (키 생성, 서명, 검증)
- **sha256.h/c**: SHA-256 해시 구현
- **main.c**: 테스트 및 데모

## 파일 구조

```
ECDSA/
├── Makefile                 # 빌드 파일
├── big_int.h/c             # 큰 정수 연산 라이브러리
├── ecc_point.h/c           # 타원곡선 점 연산
├── ecdsa.h/c              # ECDSA 알고리즘
├── sha256.h/c              # SHA-256 해시
├── main.c                  # 테스트 프로그램
├── test_basic.c            # 기본 테스트
└── README.md               # 이 파일
```

## 빌드 방법

### 컴파일
```bash
cd ECDSA
make          # 전체 빌드
make clean    # 빌드 결과 삭제
```

### 실행
```bash
./ecdsa_demo  # 전체 데모 실행
./test_basic  # 기본 기능 테스트
```

## 주요 함수

### BigInt 연산 (big_int.h)
```c
void big_int_zero(BigInt *a);                                    // 0으로 초기화
void big_int_from_hex_string(BigInt *a, const char *hex_str);   // 16진수 문자열에서 생성
void big_int_from_uint64(BigInt *a, uint64_t val);              // uint64 값에서 생성
void big_int_add(BigInt *result, const BigInt *a, const BigInt *b);    // 덧셈
void big_int_sub(BigInt *result, const BigInt *a, const BigInt *b);   // 뺄셈
void big_int_mul(BigInt *result, const BigInt *a, const BigInt *b);   // 곱셈
void big_int_mod(BigInt *result, const BigInt *a, const BigInt *modulus); // 모듈로
void big_int_mod_add(BigInt *result, const BigInt *a, const BigInt *b, const BigInt *modulus);  // 모듈로 덧셈
void big_int_mod_sub(BigInt *result, const BigInt *a, const BigInt *b, const BigInt *modulus);  // 모듈로 뺄셈
void big_int_mod_mul(BigInt *result, const BigInt *a, const BigInt *b, const BigInt *modulus);  // 모듈로 곱셈
int big_int_mod_inv(BigInt *result, const BigInt *a, const BigInt *modulus);  // 모듈로 역원
```

### 타원곡선 점 연산 (ecc_point.h)
```c
void ecc_point_set(ECCPoint *p, const BigInt *x, const BigInt *y);     // 점 설정
void ecc_point_set_infinity(ECCPoint *p);                               // 무한원점 설정
void ecc_point_copy(ECCPoint *dest, const ECCPoint *src);               // 점 복사
int ecc_point_is_valid(const ECCPoint *p);                              // 곡선 위의 점인지 확인
void ecc_point_double(ECCPoint *result, const ECCPoint *p);             // 점 두배 (2P)
void ecc_point_add(ECCPoint *result, const ECCPoint *p1, const ECCPoint *p2);  // 점 덧셈 (P+Q)
void ecc_point_scalar_mult(ECCPoint *result, const ECCPoint *p, const BigInt *scalar);  // 스칼라 곱셈 (k*P)
void secp256k1_init();                                                   // secp256k1 파라미터 초기화
```

### ECDSA 알고리즘 (ecdsa.h)
```c
// 키 생성: 개인 키 d를 이용해 공개 키 Q = d*G 생성
void ecdsa_key_generate(ECDSAKeyPair *keypair, const BigInt *private_key);

// 디지털 서명: 메시지 m의 서명 (r, s) 생성
int ecdsa_sign(ECDSASignature *signature, const uint8_t *message, 
               size_t message_len, const ECDSAKeyPair *keypair);

// 서명 검증: 서명 (r, s)이 유효한지 확인 (반환값: 1=유효, 0=무효)
int ecdsa_verify(const uint8_t *message, size_t message_len, 
                 const ECDSASignature *signature, const ECCPoint *public_key);
```

## 사용 예제

### 1. 키 생성
```c
ECDSAKeyPair keypair;
BigInt private_key;
big_int_from_hex_string(&private_key, "0123456789ABCDEF...");

ecdsa_key_generate(&keypair, &private_key);
// keypair.private_key: 개인 키
// keypair.public_key: 공개 키
```

### 2. 메시지 서명
```c
const char *message = "Hello, ECDSA!";
ECDSASignature signature;

if (ecdsa_sign(&signature, (const uint8_t *)message, 
               strlen(message), &keypair)) {
    printf("서명 생성 성공\n");
    printf("r = "); big_int_print_hex(&signature.r);
    printf("s = "); big_int_print_hex(&signature.s);
}
```

### 3. 서명 검증
```c
if (ecdsa_verify((const uint8_t *)message, strlen(message), 
                 &signature, &keypair.public_key)) {
    printf("✓ 서명이 유효합니다\n");
} else {
    printf("✗ 서명이 유효하지 않습니다\n");
}
```

## 알고리즘 설명

### ECDSA 서명 생성
1. 메시지 m을 해시: `z = SHA-256(m)`
2. 임의의 k ∈ [1, n-1] 선택
3. 점 (x, y) = k*G 계산
4. r = x mod n
5. s = k⁻¹(z + r*d) mod n
6. r과 s가 0이 아니면 서명 (r, s) 반환

### ECDSA 서명 검증
1. r과 s가 [1, n-1] 범위인지 확인
2. 메시지를 해시: `z = SHA-256(m)`
3. w = s⁻¹ mod n 계산
4. u₁ = z*w mod n, u₂ = r*w mod n 계산
5. 점 (x, y) = u₁*G + u₂*Q 계산
6. v = x mod n
7. v == r이면 서명 유효

## 주요 특징

✓ **외부 라이브러리 사용 안 함**: OpenSSL 등의 외부 라이브러리 없이 순수 C로 구현  
✓ **256비트 정수 연산**: 64비트 limbs를 이용한 효율적인 큰 정수 연산  
✓ **secp256k1 곡선**: Bitcoin과 Ethereum에서 사용하는 표준 곡선  
✓ **SHA-256 내장**: 메시지 다이제스트에 대한 순수 C 구현  
✓ **완전한 ECDSA**: 키 생성, 서명, 검증의 전체 기능 제공

## 테스트 프로그램

### main.c
전체 ECDSA 기능을 테스트하는 종합 데모:
- 키 생성
- 단일 메시지 서명 및 검증
- 수정된 메시지 검증 (실패 확인)
- 다중 메시지 처리
- 다른 공개 키로 검증 (실패 확인)

### test_basic.c
기본 기능 테스트:
- BigInt 연산
- secp256k1 파라미터 검증
- 점 연산 (점 두배, 곡선 확인)

## 제한사항 및 향후 개선사항

### 현재 제한사항
- 난수 생성이 기본적인 구현 (암호학적으로 안전하지 않음)
- Modular inverse 계산이 단순 구현 (성능 개선 필요)
- Big integer 연산이 기본 수준 (최적화 필요)

### 향후 개선사항
- 암호학적으로 안전한 난수 생성기 추가
- Big integer 연산 최적화 (Karatsuba, Montgomery 곱셈 등)
- Batch 서명/검증 지원
- 다중군 곡선 지원 (P-256, P-384 등)
- Side-channel 공격 대응

## 참고 자료

- secp256k1 사양: https://en.bitcoin.it/wiki/Secp256k1
- ECDSA 표준: FIPS 186-4
- SHA-256: FIPS 180-4
- Elliptic Curve Cryptography 이론: "Handbook of Elliptic and Hyperelliptic Curve Cryptography"

## 라이선스

이 구현은 교육 및 학습 목적으로 제공됩니다.

## 작성 정보

- 작성일: 2026년 4월
- 언어: C (C99 표준)
- 플랫폼: Linux/Unix 호환 시스템
