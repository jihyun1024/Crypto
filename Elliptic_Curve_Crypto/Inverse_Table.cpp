#include <stdio.h>

// GF(2^4) multiplication with f(x) = x^4 + x + 1
int gf_mul(int a, int b) {
    int result = 0;
    int modulo = 0b10011; // x^4 + x + 1

    // Multiply
    for (int i = 0; i < 4; i++) {
        if ((b >> i) & 1) {
            result ^= a << i;
        }
    }

    // Modular reduction
    for (int i = 7; i >= 4; i--) {
        if ((result >> i) & 1) {
            result ^= modulo << (i - 4);
        }
    }

    return result & 0b1111;
}

int main() {
    int gf_inverse[16];

    for (int a = 0; a < 16; a++) {
        if (a == 0) {
            gf_inverse[a] = -1; // 0은 역원이 없음
            continue;
        }
        for (int b = 0; b < 16; b++) {
            if (gf_mul(a, b) == 1) {
                gf_inverse[a] = b;
                break;
            }
        }
    }

    printf("gf_inverse table:\n");
    for (int i = 0; i < 16; i++) {
        if (gf_inverse[i] == -1) {
            printf("%2d: No inverse\n", i);
        }
        else {
            printf("%2d: %2d\n", i, gf_inverse[i]);
        }
    }

    return 0;
}
