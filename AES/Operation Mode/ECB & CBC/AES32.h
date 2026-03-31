#pragma once

typedef unsigned char byte;
typedef unsigned int u32;

// byte array b[4] --> u32 int
#define GETU32(b) ((u32)(b)[0] << 24) ^ ((u32)(b)[1] << 16) \
            ^ ((u32)(b)[2] << 8) ^ ((u32)(b)[3]);

// u32 int --> byte array b[4]
#define PUTU32(b, x) { \
    (b)[0] = (byte) ((x)>>24); \
    (b)[1] = (byte)((x) >> 16); \
    (b)[2] = (byte) ((x)>>8); \
    (b)[3] = (byte) (x); \
}

void print_AES_state(byte state[16], const char* pTitle = NULL);
void AES32_Enc_KeySchedule(byte k[16], u32 rk[11][4]);
void AES32_Dec_KeySchedule(byte k[16], u32 rk[11][4]);
void byte2state(byte b[16], u32 st[4]);
void state2byte(u32 st[4], byte b[16]);
void AES32_EncRound(u32 st[4], u32 rk[4]);
void AES32_EqDecRound(u32 st[4], u32 rk[4]);
void AES32_Encrypt(byte pt[16], u32 rk[11][4], byte ct[16]);
void AES32_EqDecrypt(byte ct[16], u32 rk[11][4], byte pt[16]);
