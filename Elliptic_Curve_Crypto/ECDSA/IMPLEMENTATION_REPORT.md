# ECDSA 구현 완료 보고

## 프로젝트 개요

**Elliptic_Curve_Crypto/ECDSA** 폴더에 secp256k1 곡선을 사용한 **ECDSA (Elliptic Curve Digital Signature Algorithm)** 알고리즘의 C 구현이 완료되었습니다.

### ✓ 구현된 기능

1. **키 생성 (Key Generation)**
   - 개인 키 d ∈ [1, n-1] 선택
   - 공개 키 Q = d*G 계산
   - 결과: 개인/공개 키 쌍 (keypair)

2. **서명 (Digital Signing)**
   - SHA-256으로 메시지 해시
   - 임의의 k를 통한 ECDSA 서명 (r, s) 생성
   - 런타임 재시도를 통한 유효한 서명 보장

3. **검증 (Verification)**
   - 개인 키 없이 공개 키만으로 서명 검증
   - 메시지 변조 감지
   - 위조된 서명 거부

### ✓ 구현 환경

- **언어**: C (C99 표준)
- **외부 라이브러리**: 없음 (OpenSSL, GMP 등 미사용)
- **타원곡선**: secp256k1 (Bitcoin, Ethereum 표준)
- **해시**: SHA-256 (순수 C 구현)
- **정수 연산**: 256비트 (4개 64-bit limb)

## 프로젝트 구조

```
Elliptic_Curve_Crypto/ECDSA/
├── big_int.h / big_int.c           # 256비트 정수 연산 라이브러리
├── ecc_point.h / ecc_point.c       # 타원곡선 점 연산 (덧셈, 스칼라 곱셈)
├── ecdsa.h / ecdsa.c              # ECDSA 알고리즘 (키 생성, 서명, 검증)
├── sha256.h / sha256.c             # SHA-256 해시 함수
├── main.c                          # 종합 데모 프로그램
├── test_basic.c                    # 기본 기능 테스트
├── secp256k1_params.c              # secp256k1 파라미터 (참고용)
├── Makefile                        # 빌드 설정
├── run.sh                          # 빌드 & 실행 스크립트
└── README.md                       # 상세 기술 문서
```

## 파일 설명

### 1. big_int.h / big_int.c
256비트 정수 연산을 위한 라이브러리
- **구조**: 4개의 64-bit limb로 구성
- **연산**: 기본 산술(+, -, ×), 모듈로 연산, 비교
- **특수 함수**: 모듈로 역원, Hex 변환, 출력

### 2. ecc_point.h / ecc_point.c
secp256k1 타원곡선 상의 점 연산
- **곡선 방정식**: y² = x³ + 7 (mod p)
- **연산**: 점 덧셈(P+Q), 점 두배(2P), 스칼라 곱셈(k*P)
- **검증**: 점이 곡선 위에 있는지 확인

### 3. ecdsa.h / ecdsa.c
ECDSA 알고리즘 구현
- **ecdsa_key_generate()**: 키쌍 생성
- **ecdsa_sign()**: 메시지 서명
- **ecdsa_verify()**: 서명 검증

### 4. sha256.h / sha256.c
SHA-256 해시 알고리즘의 순수 C 구현
- 표준 FIPS 180-4 준수
- 메시지 업데이트 방식 지원
- 32바이트(256비트) 다이제스트 생성

### 5. main.c
ECDSA의 모든 기능을 보여주는 종합 데모
- 8개의 상세한 테스트 케이스
- 키 생성부터 검증까지 전체 과정 실행
- 정상 케이스와 오류 케이스 모두 확인

### 6. test_basic.c
기본 기능의 간단한 테스트
- BigInt 연산 검증
- secp256k1 파라미터 확인
- 점 연산 기능 확인

## 빌드 및 실행

### 1. 간단한 실행 (권장)
```bash
cd Elliptic_Curve_Crypto/ECDSA
./run.sh
```

### 2. 수동 빌드
```bash
cd Elliptic_Curve_Crypto/ECDSA
make              # 전체 빌드
./ecdsa_demo      # 데모 실행
./test_basic      # 기본 테스트
```

### 3. 청소
```bash
make clean        # 빌드 결과 삭제
```

## 구현 특징

### ✓ 강점

1. **완전 자체 구현**
   - 외부 라이브러리 미사용
   - 모든 알고리즘을 명확하게 볼 수 있음

2. **표준 준수**
   - secp256k1: Bitcoin/Ethereum 표준
   - SHA-256: FIPS 180-4 표준
   - ECDSA: FIPS 186-4 표준

3. **교육 목표**
   - 각 모듈이 명확히 분리됨
   - 알고리즘의 각 단계를 이해하기 용이
   - 확장 및 수정이 간단함

### ⚠ 제한사항

1. **보안**
   - 난수 생성이 기본적 (암호학적 안전성 미흡)
   - Side-channel 공격 대응 미흡
   - 테스트/학습 목적 권장

2. **성능**
   - 최적화 미흡 (Karatsuba 곱셈 등 미실장)
   - 프로덕션 환경에는 OpenSSL 등 권장

3. **기능**
   - secp256k1만 지원 (다른 곡선 미지원)
   - Batch 처리 미지원

## 사용 예제

### 기본 사용법
```c
#include "ecdsa.h"

int main() {
    secp256k1_init();  // 파라미터 초기화
    
    // 1. 키 생성
    BigInt private_key;
    big_int_from_hex_string(&private_key, "0xABCD...EF");
    
    ECDSAKeyPair keypair;
    ecdsa_key_generate(&keypair, &private_key);
    
    // 2. 메시지 서명
    const char *msg = "Hello, ECDSA!";
    ECDSASignature sig;
    ecdsa_sign(&sig, (const uint8_t *)msg, strlen(msg), &keypair);
    
    // 3. 서명 검증
    if (ecdsa_verify((const uint8_t *)msg, strlen(msg), 
                    &sig, &keypair.public_key)) {
        printf("✓ 서명 유효\n");
    }
    
    return 0;
}
```

## 파일 크기 및 성능

| 파일 | 크기 | 용도 |
|------|------|------|
| big_int.o | 45KB | 큰 정수 연산 |
| ecc_point.o | 26KB | 타원곡선 연산 |
| ecdsa.o | 22KB | ECDSA 알고리즘 |
| sha256.o | 16KB | SHA-256 해시 |
| **ecdsa_demo** | 85KB | 최종 실행 파일 |

## 다음 단계 (향후 개선)

1. **보안 강화**
   - 암호학적 난수 생성기 추가 (getrandom, /dev/urandom)
   - Constant-time 연산 구현

2. **성능 최적화**
   - Montgomery 곱셈
   - Karatsuba 알고리즘
   - Jacobian 좌표 사용

3. **기능 확장**
   - P-256, P-384 곡선 지원
   - Batch 서명/검증
   - ECDH 키 교환

4. **테스트**
   - 표준 테스트 벡터 검증
   - 위험 분석 및 Fuzz 테스트
   - Side-channel 공격 분석

## 참고 자료

- **FIPS 186-4**: Digital Signature Standard (DSS)
- **FIPS 180-4**: Secure Hash Standard (SHS)
- **SECP256k1**: https://en.bitcoin.it/wiki/Secp256k1
- **RFC 6090**: Fundamentals of ECC

## 작성 정보

- **작성일**: 2026년 4월 6일
- **언어**: C (C99)
- **고도사항**: Ubuntu 24.04 Linux
- **컴파일러**: GCC
- **용도**: 교육 및 학습

---

**주의**: 이 구현은 교육 목적입니다. 실제 암호화 통신에는 OpenSSL 등의 검증된 라이브러리 사용을 권장합니다.
