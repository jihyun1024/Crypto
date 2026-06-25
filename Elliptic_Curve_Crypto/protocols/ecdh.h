#ifndef ECDH_H
#define ECDH_H


#include "../core/big_int.h"            // 큰 정수 연산용
#include "../core/secp256k1_params.h"   // secp256k1 파라미터 참고용
#include "../core/sha256.h"             // 메시지 변조 검증용으로 사용

#include "../cryptography/csprng.h"     // 난수 생성용
#include "../cryptography/ecc_point.h"  // 타원곡선 점 연산용

#include "ecdsa.h"                      // 혹시 몰라서 넣어둠


// 함수 정의 시작

#endif