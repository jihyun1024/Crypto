#include <stdio.h>

unsigned long long int Plaintext = 0x4E6F772069732074;
unsigned long long int Key1 = 0x0123456789ABCDEF;

int Initial_Permutation[64] = {
    58, 50, 42, 34, 26, 18, 10,  2,
    60, 52, 44, 36, 28, 20, 12,  4,
    62, 54, 46, 38, 30, 22, 14,  6,
    64, 56, 48, 40, 32, 24, 16,  8,
    57, 49, 41, 33, 25, 17,  9,  1,
    59, 51, 43, 35, 27, 19, 11,  3,
    61, 53, 45, 37, 29, 21, 13,  5,
    63, 55, 47, 39, 31, 23, 15,  7
};

int Inverse_Initial_Permutation[64] = {
    40,  8, 48, 16, 56, 24, 64, 32,
    39,  7, 47, 15, 55, 23, 63, 31,
    38,  6, 46, 14, 54, 22, 62, 30,
    37,  5, 45, 13, 53, 21, 61, 29,
    36,  4, 44, 12, 52, 20, 60, 28,
    35,  3, 43, 11, 51, 19, 59, 27,
    34,  2, 42, 10, 50, 18, 58, 26,
    33,  1, 41,  9, 49, 17, 57, 25
};

int Expansion[48] = {
    32,1,2,3,4,5,
    4,5,6,7,8,9,
    8,9,10,11,12,13,
    12,13,14,15,16,17,
    16,17,18,19,20,21,
    20,21,22,23,24,25,
    24,25,26,27,28,29,
    28,29,30,31,32,01
};

int StraightPermutationTable[32] = { // 
    16,  7, 20, 21, 29, 12, 28, 17,
    1, 15, 23, 26,  5, 18, 31, 10,
    2,  8, 24, 14, 32, 27,  3,  9,
    19, 13, 30,  6, 22, 11,  4, 25
};

int SubstituteTables[8][4][16] = { // 
    // s1 [0]
    {
        { 14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7 },
        { 0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8 },
        { 4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0 },
        { 15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13 }
    },
    // s2 [1]
    {
        { 15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10 },
        { 3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5 },
        { 0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15 },
        { 13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9 }
    },
    // s3 [2]
    {
        { 10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8 },
        { 13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1 },
        { 13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7 },
        { 1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12 }
    },
    // s4 [3]
    {
        { 7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15 },
        { 13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9 },
        { 10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4 },
        { 3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14 }
    },
    // s5 [4]
    {
        { 2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9 },
        { 14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6 },
        { 4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14 },
        { 11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3 }
    },
    // s6 [5]
    {
        { 12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11 },
        { 10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8 },
        { 9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6 },
        { 4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13 }
    },
    // s7 [6]
    {
        { 4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1 },
        { 13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6 },
        { 1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2 },
        { 6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12 }
    },
    // s8 [7]
    {
        { 13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7 },
        { 1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2 },
        { 7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8 },
        { 2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11 }
    }
};

int ParityDropTable[56] = {
    57,49,41,33,25,17,9,1,
    58,50,42,34,26,18,10,2,
    59,51,43,35,27,19,11,3,
    60,52,44,36,63,55,47,39,
    31,23,15,7,62,54,46,38,
    30,22,14,6,61,53,45,37,
    29,21,13,5,28,20,12,4
};

int ShiftTable[16] = {
    1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};

int KeyCompressionTable[48] = {
    14,17,11,24,1,5,3,28,
    15,6,21,10,23,19,12,4,
    26,8,16,7,27,20,13,2,
    41,52,31,37,47,55,30,40,
    51,45,33,48,44,49,39,56,
    34,53,46,42,50,36,29,32
};

void permute(int n1, int n2, int a[], int b[], int c[]) { //  64 -> 64,   32 -> 48,   32 -> 32,   64 -> 56,   56 -> 48
    a[n1];
    b[n2];
    for (int i = 0; i < n2; i++) {
        b[i] = a[c[i] - 1];
    }
}

void split(int n1, int n2, int inBlock[], int leftBlock[], int rightBlock[]) {
    for (int i = n2 - 1; i >= 0; i--) {
        leftBlock[i] = inBlock[i];
    }
    for (int i = n1 - 1; i >= n2; i--) {
        rightBlock[i - n2] = inBlock[i];
    }
}

void copy(int n, int* a, int* b) {
    for (int i = 0; i < n; i++) {
        *(b + i) = *(a + i);
    }
}

void exclusiveOr(int n, int leftBlock[], int T2[], int T3[]) {
    for (int i = 0; i < n; i++) { // XOR ^
        T3[i] = leftBlock[i] ^ T2[i];
    }
}

void substitute(int inBlock[48], int outBlock[32], int SubstituteTables[8][4][16]) { // 6bit -> 4bit, S-BOX
    int row, col, value;
    for (int i = 0; i < 8; i++) {
        row = 2 * inBlock[i * 6 + 0] + inBlock[i * 6 + 5]; // 
        col = 8 * inBlock[i * 6 + 1] + 4 * inBlock[i * 6 + 2] + 2 * inBlock[i * 6 + 3] + inBlock[i * 6 + 4];
        value = SubstituteTables[i][row][col];
        outBlock[i * 4 + 3] = value % 2; // 0
        value = value / 2;
        outBlock[i * 4 + 2] = value % 2; // 0
        value = value / 2;
        outBlock[i * 4 + 1] = value % 2; // 1
        value = value / 2;
        outBlock[i * 4] = value; // 1
    }
}

void function(int inBlock[32], int RoundKey[48], int outBlock[32]) {
    int T1[48], T2[48], T3[32];
    permute(32, 48, inBlock, T1, Expansion);
    exclusiveOr(48, T1, RoundKey, T2);
    substitute(T2, T3, SubstituteTables);
    permute(32, 32, T3, outBlock, StraightPermutationTable);
}

void mixer(int leftBlock[48], int rightBlock[48], int RoundKey[48]) {
    int T1[48], T2[48], T3[48];
    copy(32, rightBlock, T1);
    function(T1, RoundKey, T2);
    exclusiveOr(32, leftBlock, T2, T3);
    copy(32, T3, leftBlock);
}

void swapper(int leftBlock[32], int rightBlock[32]) {
    int T[32];
    copy(32, leftBlock, T);
    copy(32, rightBlock, leftBlock);
    copy(32, T, rightBlock);
}

void shiftLeft(int block[28], int numOfShifts) {
    for (int i = 0; i < numOfShifts; i++) {
        int T; // shiftLeft
        T = block[0];
        for (int j = 1; j < 28; j++) {
            block[j - 1] = block[j];
        }
        block[27] = T;
    }
}

void combine(int n1, int n2, int a[], int b[], int c[]) {
    for (int i = n1 - 1; i >= 0; i--) { // left, right
        c[i] = a[i];
    }
    for (int i = n2 - 1; i >= n1; i--) {
        c[i] = b[i - n1];
    }
}

void Key_Genenrator(int keyWithParities[64], int RoundKeys[16][48], int ShiftTable[16]) {
    int cipherKey[56], leftKey[28], rightKey[28], preRoundKey[56];
    permute(64, 56, keyWithParities, cipherKey, ParityDropTable);
    split(56, 28, cipherKey, leftKey, rightKey);
    for (int round = 0; round < 16; round++) {
        shiftLeft(leftKey, ShiftTable[round]);
        shiftLeft(rightKey, ShiftTable[round]);
        combine(28, 56, leftKey, rightKey, preRoundKey);
        permute(56, 48, preRoundKey, RoundKeys[round], KeyCompressionTable);
    }
}

void Cipher(int plainBlock[64], int RoundKeys[16][48], int cipherBlock[64]) {
    int inBlock[64], leftBlock[32], rightBlock[32], outBlock[64];
    permute(64, 64, plainBlock, inBlock, Initial_Permutation);
    split(64, 32, inBlock, leftBlock, rightBlock);
    for (int round = 0; round < 16; round++) {
        mixer(leftBlock, rightBlock, RoundKeys[round]);
        if (round != 15) {
            swapper(leftBlock, rightBlock);
        }
    }
    combine(32, 64, leftBlock, rightBlock, outBlock);
    permute(64, 64, outBlock, cipherBlock, Inverse_Initial_Permutation);
}

void Hex(int n1, int plainBlock[], int value[]) { // 2 -> 16
    n1 = n1 / 4;
    for (int i = 0; i < n1; i++) {
        value[i] = 8 * plainBlock[4 * i] + 4 * plainBlock[4 * i + 1] + 2 * plainBlock[4 * i + 2] + plainBlock[4 * i + 3];
    }
    for (int i = 0; i < n1; i++) {
        if (value[i] <= 9) {
            value[i] = value[i] + 48;
        }
        else {
            value[i] = value[i] + 55;
        }
    }
}

int main() {
    int plainBlock[64], cipherBlock[64];
    int RoundKeys1[16][48], keyWithParities1[64], keyWithParities2[64];
    int inBlock[64];
    int leftblock[32];
    int rightblock[32];

    for (int i = 63; i >= 0; i--) {
        plainBlock[i] = Plaintext % 2;
        Plaintext = Plaintext / 2;
    }

    int Hex1[16] = { 0, };
    Hex(64, plainBlock, Hex1);
    printf("Plaintext : ");
    for (int i = 0; i < 16; i++) {
        printf("%c", Hex1[i]);
    }

    for (int i = 63; i >= 0; i--) {
        keyWithParities1[i] = Key1 % 2;
        Key1 = Key1 / 2;
    }
    int Hex2[16] = { 0, };
    Hex(64, keyWithParities1, Hex2);
    printf("\n\nKey : ");
    for (int i = 0; i < 16; i++) {
        printf("%c", Hex2[i]);
    }
    printf("\n");
    printf("\nAfter intial permutation : ");
    int Hex6[16] = { 0, };
    permute(64, 64, plainBlock, inBlock, Initial_Permutation);
    Hex(64, inBlock, Hex6);

    for (int i = 0; i < 16; i++)
    {
        printf("%c", Hex6[i]);
    }
    printf("\n");


    split(64, 32, inBlock, leftblock, rightblock);
    int Hex7[8] = { 0, }; // leftblock
    int Hex8[8] = { 0, }; // rightblock
    Hex(32, leftblock, Hex7);
    Hex(32, rightblock, Hex8);
    printf("\nAfter splitting : L0 = ");
    for (int i = 0; i < 8; i++)
    {
        printf("%c", Hex7[i]);
    }
    printf(" R0 = ");
    for (int i = 0; i < 8; i++)
    {
        printf("%c", Hex8[i]);
    }
    printf("\n");


    Key_Genenrator(keyWithParities1, RoundKeys1, ShiftTable);
    int Hex3[16][12] = { 0, };
    for (int i = 0; i < 16; i++) {
        Hex(48, RoundKeys1[i], Hex3[i]);
    }

    int Hex9[8] = { 0, };
    int Hex10[8] = { 0, };

    for (int i = 0; i < 15; i++)
    {
        printf("\nRound : %d", i + 1);
        mixer(leftblock, rightblock, RoundKeys1[i]);
        swapper(leftblock, rightblock);
        printf("\nleftblock : ");
        Hex(32, leftblock, Hex9);
        Hex(32, rightblock, Hex10);

        for (int i = 0; i < 8; i++)
        {
            printf("%c", Hex9[i]);
        }
        printf("\nrightBlock : ");

        for (int i = 0; i < 8; i++)
        {
            printf("%c", Hex10[i]);
        }
        printf("\nRound Key : ", i + 1);
        for (int j = 0; j < 12; j++) {
            printf("%c", Hex3[i][j]);
        }
        printf("\n");
    }
    int Hex11[8] = { 0, };
    int Hex12[8] = { 0, };
    mixer(leftblock, rightblock, RoundKeys1[15]); 
    Hex(32, leftblock, Hex11);
    Hex(32, rightblock, Hex12);
    printf("\nRound : 16\n");
    printf("leftblock : ");
    for (int i = 0; i < 8; i++)
    {
        printf("%c", Hex11[i]);
    }
    printf("\nrightblock : ");
    for (int i = 0; i < 8; i++)
    {
        printf("%c", Hex12[i]);
    }
    printf("\nRound key : ");
    for (int j = 0; j < 12; j++) {
        printf("%c", Hex3[15][j]);
    }


    printf("\n\nAfter combination : "); // final ġȯ
    int Hex15[16] = { 0, };

    for (int i = 0; i < 8; i++)
    {
        printf("%c", Hex11[i]);
    }

    for (int i = 0; i < 8; i++)
    {
        printf("%c", Hex12[i]);
    }
    printf("\n");


    Cipher(plainBlock, RoundKeys1, cipherBlock);

    int Hex4[16] = { 0, };
    Hex(64, cipherBlock, Hex4);
    printf("\nChipertext : ");
    for (int i = 0; i < 16; i++) {
        printf("%c", Hex4[i]);
    }
    printf("\n");
}