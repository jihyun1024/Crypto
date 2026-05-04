# 파일 구조 (README 제외)

```
project/
├── for_test.c            # 전체 테스트 및 통합 실행부
├── Makefile              # 컴파일 자동화 스크립트
│
├── core/                 # [1단계] 최하위 산술 연산
│   ├── big_int.h / .c
|   ├── secp256k1_params.h / .c
│   └── sha256.h / .c     
│
├── crypto/               # [2단계] 타원 곡선 수학
│   ├── ecc_point.h / .c  
│   ├── csprng.h / .c    
|   └── cng_rng.h / .c   
│
└── protocols/            # [3단계] 상위 암호 프로토콜
    ├── ecdsa.h / .c      
    └── ecdh.h / .c       
```

# 각 파일별 역할
## Core 연산 계층
- `big_int.h / .c`: 이미 구현한 큰 수 연산기
- `secp256k1_params.h / .c`: secp256k1 타원곡선의 파라미터 초기화
- `sha256.h / .c`: ECDSA의 메시지 해싱, ECDH의 키 유도 과정에서 사용

## Crypto 계층
- `ecc_point.h / .c`: big_int로 타원 곡선 수식 풀이
    - ECDSA와 ECDH가 이 모듈을 공유
    - secp256k1 곡선에서의 연산 함수를 중심으로 구현
- `csprng.h / .c`: 직접 구현한 난수 생성 기능
- `cng_rng./ .c`: CNG(Cryptography Next Generation)을 이용한 난수 생성 (MSVC 환경에서만 동작)

## Protocol 계층
- `ecdsa.h / .c`: ECDSA 알고리즘으로 서명 생성 및 검증 수행
- `ecdh.h / .c`: ECDH 알고리즘으로 키 교환 수행 (키 유도 포함)

# 파일 분리 시 핵심
중복 포함(Redefinition) 방지로, 모든 헤더 파일에 반드시 다음처럼 가드 사용

```
// ecdh.h 예시
#ifndef SECP256K1_ECDH_H
#define SECP256K1_ECDH_H

#include "ecc_core.h"
#include "sha256.h"

// ECDH 전용 함수 선언
int ecdh_compute_key(uint8_t *out_key, const uint8_t *priv_key, const uint8_t *peer_pub_key);

#endif
```

# 컴파일 및 관리 방법
파일이 많아지면 매번 gcc로 하나씩 컴파일하기 어려우니, Makefile을 만들어 `make` 명령어 한 줄로 컴파일

# 구현 및 실행 환경
- 언어: C/C++
- IDE: Visual Studio Code
- 컴파일러: GCC 10.3.0
- Build Tool: Make