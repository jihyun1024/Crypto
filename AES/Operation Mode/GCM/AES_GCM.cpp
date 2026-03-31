#include <iostream>
#include "AES32.h"
#include "GHASH.h"

extern byte R0[256], R1[256]; 

//GCM 모드에서 사용하는 카운터 증가 함수 Inc_32()
// counter: c[0] c[1] ... c[14] c[15] (MSB가 왼쪽, LSB가 오른쪽)
void counter_inc(byte counter[16]) {
    for (int i = 15; i >= 0; i--) {
        if (counter[i] != 0xff) { 
            counter[i]++;
            break;
        }
        else { // 자리올림이 발생하는 경우: counter[i] == 0xff인 경우
            counter[i] = 0x00; // 0xff + 0x01 = 0x00 + (carry)
        }
    }
}

// AES CTR mode
// PT[], CT[] :  평문, 암호문 메모리는 미리 준비한다.
// 평문 사이즈 == 암호문 사이즈 (CTR mode는 Padding이 없음)
// (파일 암호화 방법은 CBC 모드를 조금 바꾸면 됨)
void AES_CTR(byte PT[], int pt_bytes, byte key[16], byte CTR[16], byte CT[]) {

    int num_blocks, remainder;
    num_blocks = pt_bytes / 16; // 블록의 개수
    remainder = pt_bytes - num_blocks * 16; // 0,1,...,15 -> 자투리 사이즈

    byte pt[16], ctr_ct[16];
    u32 rk[11][4];
    byte current_ctr[16];

    // 입력받은 키로부터 키스케줄 수행, 라운드키 생성
    AES32_Enc_KeySchedule(key, rk);

    copy_b_array(CTR, 16, current_ctr);
    for (int i = 0; i < num_blocks; i++) {
        for (int j = 0; j < 16; j++) pt[j] = PT[16 * i + j];
        AES32_Encrypt(current_ctr, rk, ctr_ct);
        xor_b_array(pt, 16, ctr_ct); // pt -> pt xor E(ctr)
        for (int j = 0; j < 16; j++) {
            CT[16 * i + j] = pt[j]; // 대응되는 암호문 자리에 업데이트
        }
        counter_inc(current_ctr);
    }
    //나머지 평문 암호화
    AES32_Encrypt(current_ctr, rk, ctr_ct);
    for (int i = 0; i < remainder; i++) {
        pt[i] = PT[16 * num_blocks + i];
        pt[i] ^= ctr_ct[i]; // pt -> pt xor E(ctr)
        CT[16 * num_blocks + i] = pt[i];
    }
}

// GCM mode (parameter: PT, pt_length, Counter, key, 부가 정보, 부가 정보 길이, CT, Tag)
void AES_GCM(byte PT[], int pt_bytes, byte CTR[16], byte key[16],
    byte A[], int A_bytes, byte CT[], byte Tag[16]) {
    long long int Alen, Clen; // 64비트 변수 (int만으로는 범위가 부족함)
    Alen = (long long int) A_bytes * 8; // 부가정보 A의 비트 수 (바이트 수 * 8)
    Clen = (long long int) pt_bytes * 8; // 암호문 C의 비트 수 (바이트 수 * 8)

    byte zero_block[16] = { 0, };
    byte first_block[16] = { 0, };
    byte last_block[16];
    // 첫번째 블록 (A||0..0)
    // 부가정보 A는 한블록 이내로 가정 : A_bytes : 0,1,...,16 (바이트)
    if (A_bytes > 0) { // 부가정보 A를 사용함
        for (int j = 0; j < A_bytes; j++) {
            first_block[j] = A[j];
        }
    }
    // 마지막 블록 (Alen || Clen)
    for (int j = 0; j < 8; j++) {
        last_block[j] = (Alen >> (8 * (7 - j))) & 0xff;
        last_block[8 + j] = (Clen >> (8 * (7 - j))) & 0xff;
    }

    byte Enc_counter[16]; // CTR 모드 암호화를 위한 카운터
    byte H[16];
    u32 rk[11][4];

    // 키스케줄 실행 후 zero_block을 라운드키로 암호화
    AES32_Enc_KeySchedule(key, rk);
    AES32_Encrypt(zero_block, rk, H);

    copy_b_array(CTR, 16, Enc_counter);
    counter_inc(Enc_counter);
    AES_CTR(PT, pt_bytes, key, Enc_counter, CT);

    int msg_bytes, remainder;
    remainder = (pt_bytes % 16) == 0 ? 0 : 16 - (pt_bytes % 16); // pt_bytes가 16의 배수인 경우 대비
    msg_bytes = (pt_bytes % 16) == 0 ? pt_bytes + 2 * 16 : 
        (pt_bytes / 16) * 16 + 3 * 16;

    //GHASH 입력 블록 만들기 (AES_GCM 그림 참고)
    byte* MSG = (byte*)malloc(msg_bytes); // 에러처리 필요
    for (int i = 0; i < 16; i++) MSG[i] = first_block[i];
    for (int i = 0; i < pt_bytes; i++) MSG[i + 16] = CT[i]; // first_block 다음부터 채우기
    for (int i = 0; i < remainder; i++) MSG[16 + pt_bytes + i] = 0x00; // 그 다음부터 
    for (int i = 0; i < 16; i++) MSG[16 + pt_bytes + remainder + i] = last_block[i]; 

    // GHASH 연산
    byte HT[256][16]; // 사전계산용 테이블
    Make_GHASH_H_table(H, HT); // HT 만들기
    GHASH(MSG, msg_bytes / 16, HT, R0, R1, Tag);
    byte Y[16];
    AES32_Encrypt(CTR, rk, Y);
    xor_b_array(Tag, 16, Y);

    free(MSG);
}

void AES_GCM_testvector0() {  
    const char* hex_key = "11754cd72aec309bf52f7687212e8957";	
    const char* hex_iv = "3c819d9a9bed087615030b65";			
    const char* hex_pt = "";	
    const char* hex_aad = "";	
    const char* hex_ct = "";	
    const char* hex_tag = "250327c674aaf477aef2675748cf6971";

    byte key[16], iv[16], pt[16], ct[16], aad[16], tag[16];

    Hex2Array(hex_key, 32, key);
    Hex2Array(hex_iv, 24, iv);

    printf("TestVector-GCM... \n");

    byte CTR0[16] = { 0, };
    for (int i = 0; i < 12; i++) CTR0[i] = iv[i];
    CTR0[15] = 0x01;

    AES_GCM(pt, 0, CTR0, key, aad, 0, ct, tag);

    print_b_array(tag, 16, "(calculated) tag");
    printf("(expected) tag = %s\n\n", hex_tag);
}

void AES_GCM_testvector1() {
    const char* hex_key = "77be63708971c4e240d1cb79e8d77feb";	
    const char* hex_iv = "e0e00f19fed7ba0136a797f3";			
    const char* hex_pt = "";	
    const char* hex_aad = "7a43ec1d9c0a5a78a0b16533a6213cab";	
    const char* hex_ct = "";		
    const char* hex_tag = "209fcc8d3675ed938e9c7166709dd946";

    byte key[16], iv[16], pt[16], ct[16], aad[16], tag[16];

    Hex2Array(hex_key, 32, key);
    Hex2Array(hex_iv, 24, iv);
    //Hex2Array(hex_pt, 32, pt);
    Hex2Array(hex_aad, 32, aad);

    printf("TestVector-GCM... \n");

    byte CTR0[16] = { 0, };
    for (int i = 0; i < 12; i++) CTR0[i] = iv[i];
    CTR0[15] = 0x01;

    AES_GCM(pt, 0, CTR0, key, aad, 16, ct, tag);

    print_b_array(tag, 16, "(calculated) tag");
    printf("(expected) tag = %s\n\n", hex_tag);
}

void AES_GCM_testvector2() {
    const char* hex_key = "c939cc13397c1d37de6ae0e1cb7c423c";	
    const char* hex_iv = "b3d8cc017cbb89b39e0f67e2";			
    const char* hex_pt = "c3b3c41f113a31b73d9a5cd432103069";	
    const char* hex_aad = "24825602bd12a984e0092d3e448eda5f";	
    const char* hex_ct = "93fe7d9e9bfd10348a5606e5cafa7354";	
    const char* hex_tag = "0032a1dc85f1c9786925a2e71d8272dd";	

    byte key[16], iv[16], pt[16], ct[16], aad[16], tag[16];

    Hex2Array(hex_key, 32, key);
    Hex2Array(hex_iv, 24, iv);
    Hex2Array(hex_pt, 32, pt);
    Hex2Array(hex_aad, 32, aad);

    printf("TestVector-GCM... \n");

    byte CTR0[16] = { 0, };
    for (int i = 0; i < 12; i++) CTR0[i] = iv[i];
    CTR0[15] = 0x01;

    AES_GCM(pt, 16, CTR0, key, aad, 16, ct, tag);

    print_b_array(ct, 16, "(calculated) ct");
    print_b_array(tag, 16, "(calculated) tag");
    printf("(expected) ct = %s\n", hex_ct);
    printf("(expected) tag = %s\n\n", hex_tag);
}

void AES_CTR_testvector() { //CTR 모드 확인용
    const char* hex_key = "2b7e151628aed2a6abf7158809cf4f3c";
    const char* hex_iv = "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";
    const char* hex_pt = "6bc1bee22e409f96e93d7e117393172a";
    const char* hex_ct = "874d6191b620e3261bef6864990db6ce";

    byte key[16], iv[16], pt[16], ct[16];

    Hex2Array(hex_key, 32, key);
    Hex2Array(hex_iv, 32, iv);
    Hex2Array(hex_pt, 32, pt);

    printf("TestVector-CTR... \n");

    AES_CTR(pt, 16, key, iv, ct);

    print_b_array(ct, 16, "ct");
}

void AES_GCM_testvector3() {
    const char* hex_key = "7fddb57453c241d03efbed3ac44e371c";
    const char* hex_iv = "ee283a3fc75575e33efd4887";
    const char* hex_pt = "d5de42b461646c255c87bd2962d3b9a2";
    const char* hex_aad = "";
    const char* hex_ct = "2ccda4a5415cb91e135c2a0f78c9b2fd";
    const char* hex_tag = "b36d1df9b9d5e596f83e8b7f52971cb3";


    byte key[16], iv[16], pt[16], ct[16], aad[16], tag[16];

    Hex2Array(hex_key, 32, key);
    Hex2Array(hex_iv, 24, iv);  
    Hex2Array(hex_pt, 32, pt);

    printf("TestVector-GCM... \n");

    byte CTR[16] = { 0, };
    for (int i = 0; i < 12; i++) CTR[i] = iv[i];
    CTR[15] = 0x01;

    AES_GCM(pt, 16, CTR, key, aad, 0, ct, tag);

    print_b_array(ct, 16, "(calculated) ct = ");
    printf("(expected) ct = %s\n", hex_ct);
    print_b_array(tag, 16, "(calculated) tag = ");
    printf("(expected) tag = %s\n\n", hex_tag);
}

int main()
{
    AES_GCM_testvector0();
    AES_GCM_testvector1();
    AES_GCM_testvector2();
    AES_GCM_testvector3();
}

