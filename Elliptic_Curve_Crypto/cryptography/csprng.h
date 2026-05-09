#ifndef CSPRNG_H
#define CSPRNG_H

#include "big_int.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

// 초기화 및 종료 함수
void csprng_init(void);     // 내부 상태 초기화 (예: 엔트로피 풀 초기화)
void csprng_cleanup(void);  // 내부 상태 메모리 삭제

// 엔트로피 수집 함수 (출처: 시스템 타이머, 마우스 움직임, 키보드 입력 등등)
void csprng_seed_init(void);        // 초기 시드 설정
void csprng_collect_entropy(void);  // 엔트로피 수집
void csprng_reseed(void);           // 주기적으로 엔트로피를 재수집

// 난수 생성 함수
void csprng_random_bytes(uint8_t *out, size_t out_len);             // bytes 단위로 난수 생성
void csprng_random_bigint(BigInt *out, size_t out_len);       // 지정된 비트 길이의 난수를 BigInt 기반으로 생성
void csprng_random_bigint_mod(BigInt *out, const BigInt *mod);   // 지정된 모듈로 난수를 생성 (0 ~ mod-1 범위)


#endif CSPRNG_H