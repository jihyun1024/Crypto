#include <iostream>
#include <fstream>
using namespace std;

// AES256 header
#include "AES32.h"

void XOR_blocks(byte* out, byte* in1, byte* in2, int length) {
    for (int i = 0; i < length; i++) {
        out[i] = in1[i] ^ in2[i];
    }
}

void AES_Enc_ECB(const char* filePT, byte key[16], const char* fileCT) {
    ifstream fin;
    ofstream fout;

    fin.open(filePT, ios::binary);
    if (fin.fail()) {
        cout << "Plaintext File Open Error!" << endl;
        return;
    }
    int file_len;
    fin.seekg(0, fin.end); // get-pointer를 맨 뒤로 이동
    file_len = fin.tellg();
    cout << "file size(plaintext) = " << file_len << " bytes" << endl;
    fin.seekg(0, fin.beg); // get-pointer를 맨 앞으로 이동

    fout.open(fileCT, ios::binary);
    if (fout.fail()) {
        cout << "Ciphertext File Open Error!" << endl;
        return;
    }

    int num_block = file_len / 16;

    cout << "file size(ciphertext) = " << num_block * 16 << " bytes" << endl;

    //AES256 KeySchedule
    u32 rk[11][4]; // RoundKey
    AES32_Enc_KeySchedule(key, rk);

    //AES256 ECB Encryption
    byte pt[16]; // 1 Block Plaintext
    byte ct[16]; // 1 Block Ciphertext

    for (int i = 0; i < num_block; i++) {
        fin.read((char*)pt, 16);
        AES32_Encrypt(pt, rk, ct);
        fout.write((char*)ct, 16);
    }

    fin.close();
    fout.close();
}

void AES_Enc_CBC(const char* filePT, byte key[16], const char* fileCT)
{
    ifstream fin;
    ofstream fout;

    byte IV[16] = { 0xaa, 0xd1, 0x58, 0x3c, 0xd9, 0x13, 0x65, 0xe3,
    0xbb, 0x2f, 0x0c, 0x34, 0x30, 0xd0, 0x65, 0xbb };

    fin.open(filePT, ios::binary);
    if (fin.fail()) {
        cout << "Plaintext File Open Error!" << endl;
        return;
    }

    int file_len;
    fin.seekg(0, fin.end); // get-pointer를 맨 뒤로 이동
    file_len = fin.tellg(); // get-pointer의 현재 위치 = file의 길이
    cout << "file size(plaintext) = " << file_len << "bytes" << endl;
    fin.seekg(0, fin.beg); // get-pointer를 맨 앞으로 이동

    fout.open(fileCT, ios::binary);
    if (fout.fail()) {
        cout << "Ciphertext File Open Error!" << endl;
        return;
    }

    // Block의 개수
    int num_block = file_len / 16;
    cout << "file size(ciphertext) = " << num_block * 16 << "bytes" << endl;

    // AES256 KeySchedule and CBC Encryption
    u32 rk[11][4];
    AES32_Enc_KeySchedule(key, rk);

    byte buffer[16];
    byte ct[16];
    byte pt[16];
    byte previous_ct[16];

    memcpy(previous_ct, IV, 16);

    for (int i = 0; i < num_block; i++) {
        fin.read((char*)pt, 16);
        XOR_blocks(buffer, pt, previous_ct, 16);
        AES32_Encrypt(buffer, rk, ct);
        fout.write((char*)ct, 16);
        memcpy(previous_ct, ct, 16);
    }

    // Close File Stream
    fin.close();
    fout.close();
}

void File_Enc_ECB()
{
    const char* PT = "PT-ECB.bin";
    const char* CT = "CT-ECB.bin";

    byte HWkey[16] = { 0x77, 0x23, 0xd8, 0x7d, 0x77, 0x3a, 0x8b, 0xbf,
    0xe1, 0xae, 0x5b, 0x08, 0x12, 0x35, 0xb5, 0x66 };

    cout << "AES ECB Encrypt..." << endl;
    AES_Enc_ECB(PT, HWkey, CT);
}

void File_Enc_CBC()
{
    const char* PT = "PT-CBC.bin";
    const char* CT = "CT-CBC.bin";

    byte HWkey[16] = { 0x07, 0x00, 0xd6, 0x03, 0xa1, 0xc5, 0x14, 0xe4,
    0x6b, 0x61, 0x91, 0xba, 0x43, 0x0a, 0x3a, 0x0c };

    cout << "AES CBC Encrypt..." << endl;
    AES_Enc_CBC(PT, HWkey, CT);
}

int main() {
    File_Enc_ECB();
    File_Enc_CBC();

    return 0;
}
