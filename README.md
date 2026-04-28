# Crypto Repository

이 리포지토리는 고전적인 암호화 알고리즘과 현대 암호화 알고리즘의 구현을 포함합니다. AES, DES, 그리고 타원 곡선 암호화 (ECC) 관련 알고리즘들이 구현되어 있습니다. 각 구현은 교육 목적으로 작성되었으며, 외부 라이브러리 없이 순수 C/C++로 구현되었습니다.

## 프로젝트 구조

### AES (Advanced Encryption Standard)
AES 암호화 알고리즘의 구현입니다. 128비트 키를 사용하는 AES-128을 지원합니다.

- **AES32.cpp / AES32.h**: 기본 AES 암호화/복호화 함수들
- **Operation Mode/**: 다양한 운영 모드 구현
  - **ECB & CBC/**: ECB (Electronic Codebook)와 CBC (Cipher Block Chaining) 모드
    - `Run_AES_ECB_CBC.cpp`: ECB와 CBC 모드의 테스트 프로그램
    - 여러 .bin 파일들: 테스트 데이터와 출력 파일들
  - **GCM/**: GCM (Galois/Counter Mode) 모드 구현
    - `AES_GCM.cpp`: GCM 모드 암호화
    - `GHASH.cpp / GHASH.h`: GHASH 함수 구현
    - `HexByte.cpp / HexByte.h`: 헥사바이트 유틸리티

### DES (Data Encryption Standard)
DES 암호화 알고리즘의 구현입니다.

- **Data_Encryption_Standard.cpp**: DES 암호화/복호화의 완전한 구현 (초기 순열, 확장, S-박스, P-박스 등 포함)

### Elliptic_Curve_Crypto (타원 곡선 암호화)
ECC(Elliptic Curve Crypto) implementation learned in Advanced Cryptography and Practice (Kookmin University)

- **Addition Table.cpp / Addition Table.h**: 타원 곡선 덧셈 테이블 생성 (Toy Example)
- **Inverse_Table.cpp**: GF(2^4) 필드의 역원 테이블 생성
- **EncDec.cpp**: 타원 곡선 ElGamal 암호화/복호화 Toy Example

#### ECDH (Elliptic Curve Diffie-Hellman)
ECDH 키 교환 프로토콜의 구현입니다.

- **big_int.c / big_int.h**: 256비트 정수 연산 라이브러리
- **csprng.c / csprng.h**: 암호학적으로 안전한 난수 생성기
- **ecc_point.c / ecc_point.h**: 타원 곡선 점 연산 (덧셈, 스칼라 곱셈)
- **secp256k1_params.c**: secp256k1 곡선 파라미터 정의

#### ECDSA (Elliptic Curve Digital Signature Algorithm)
secp256k1 곡선을 사용한 ECDSA 디지털 서명 알고리즘의 완전한 C 구현입니다.

- **big_int.c / big_int.h**: 256비트 정수 연산
- **ecc_point.c / ecc_point.h**: 타원 곡선 점 연산
- **ecdsa.c / ecdsa.h**: ECDSA 키 생성, 서명, 검증 함수
- **sha256.c / sha256.h**: SHA-256 해시 함수 구현
- **main.c**: 데모 프로그램
- **test_basic.c**: 기본 기능 테스트
- **Makefile**: 빌드 설정
- **run.sh**: 빌드 및 실행 스크립트
- **IMPLEMENTATION_REPORT.md**: 구현 완료 보고서

빌드 및 실행 방법:
```bash
cd Elliptic_Curve_Crypto/ECDSA
make
./ecdsa_demo  # 데모 실행
./test_basic  # 테스트 실행
```

## 사용된 언어 및 환경
- **C/C++**: 모든 구현은 표준 C/C++로 작성됨
- **외부 라이브러리**: 없음 (OpenSSL, GMP 등 미사용)
- **표준 곡선**: secp256k1 (Bitcoin, Ethereum 등에서 사용)

## 참고 사항
- 이 코드는 교육 및 학습 목적으로 작성되었으며, 실제 상용 환경에서는 검증된 암호화 라이브러리를 사용하는 것을 권장합니다.
- 각 구현은 해당 알고리즘의 기본 개념을 이해하기 위한 참고 자료로 활용될 수 있습니다.

## 라이선스
이 리포지토리의 코드는 공개되어 있으며, 교육 목적으로 자유롭게 사용할 수 있습니다.