#include <stdio.h>
#include <string.h>

// Coefficient of Elliptic Curve
#define A 0b1000

// Point struct
typedef struct _Point {
    int x;
    int y;
}Point;

// Inverse table of each elements in GF(2^4)
// 몇 번째 index에 해당 원소의 역원이 나오는지
int gf_inverse[16] = {
    -1, // 0 has no inverse
    1,  // 1
    9,  // a
    14, // a + 1
    13, // a^2
    11, // a^2 + 1
    7,  // a^2 + a
    6,  // a^2 + a + 1
    15, // a^3
    2,  // a^3 + 1
    12, // a^3 + a
    5,  // a^3 + a + 1
    10, // a^3 + a^2
    4,  // a^3 + a^2 + 1
    3,  // a^3 + a^2 + a
    8   // a^3 + a^2 + a + 1
};

// GF(2^4) multiplication
int gf_mul(int a, int b) {
    int result = 0;
    int modulo = 0b10011;  // Reduction polynomial: x^4 + x + 1

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

    return result & 0b1111;  // keep only 4 bits
}

// Define Point Doubling
Point doubling(Point P1) {
    Point P3;

    if (P1.x == 0) {
        // P_1 * 2 = Point of infinity when P1.x = 0
        P3.x = 0;
        P3.y = 0;
        return P3;
    }
    else {
        int inv = gf_inverse[P1.x];
        int frac = gf_mul(P1.y, inv);
        int lambda = P1.x ^ frac;

        P3.x = (gf_mul(lambda, lambda)) ^ lambda ^ A;
        P3.y = (gf_mul(P1.x, P1.x)) ^ gf_mul(lambda, P3.x) ^ P3.x;
    }

    return P3;
}

// Define Point Addition
Point addition(Point P1, Point P2) {
    Point P3;

    int dx = P1.x ^ P2.x;
    int dy = P1.y ^ P2.y;

    if (P1.x == 0 && P1.y == 0) { // P1이 항등원.
        return P2;
    }

    if (P2.x == 0 && P2.y == 0) { // P2가 항등원.
        return P1;
    }

    if (P1.x == P2.x) {
        if (P1.y == P2.y) {
            // x, y좌표 모두 같을 때 -> doubling 호출
            return doubling(P1);
        }
        else {
            // x좌표만 같을 때 -> point of infinity
            P3.x = 0;
            P3.y = 0;
            return P3;
        }
    }
    else {
        int dx_inv = gf_inverse[dx];
        int lambda = gf_mul(dy, dx_inv);

        P3.x = (gf_mul(lambda, lambda)) ^ lambda ^ P1.x ^ P2.x ^ A;
        P3.y = (gf_mul(lambda, P1.x ^ P3.x)) ^ P3.x ^ P1.y;
    }

    return P3;
}

// Define Scalar Multiplication (Right-to-Left method)
Point multiplication(Point P, int d) {
    Point Q1 = P;
    Point Q2;

    Q2.x = 0b0000;
    Q2.y = 0b0000;

    while (d > 0) {
        // If d_i == 1, operate addition
        if ((d & 1) == 1) {
            Q2 = addition(Q2, Q1);
        }
        Q1 = doubling(Q1);

        // Right bit shift
        d = d >> 1;
    }

    return Q2;
}

// Print as binary
void print_bin(int num) {
    for (int cnt_i = 3; cnt_i >= 0; cnt_i--) {
        printf("%d", (num >> cnt_i) & 1);
    }
}

// Print as point coordinates
void print_point(Point point) {
    printf("(");
    print_bin(point.x);
    printf(", ");
    print_bin(point.y);
    printf(")");
}

// Print table 
void print_table(Point points[], int size)
{
    printf("[Addition Table]\n");
    Point result;

    for (int cnt_i = 0; cnt_i < size; cnt_i++) {
        for (int cnt_j = 0; cnt_j < size; cnt_j++) {
            result = addition(points[cnt_i], points[cnt_j]);

            print_point(points[cnt_i]);
            printf(" + ");
            print_point(points[cnt_j]);
            printf(" = ");
            print_point(result);
            printf("\n");
        }
    }
}

// Check E(F_2^4) is cyclic
void check_cyclic(Point points[], int size) {
    for (int i = 0; i < size; i++) {

        printf("[check if ");
        print_point(points[i]);
        printf(" is cyclic!!]\n");

        Point alpha = points[i];
        Point original = points[i];

        printf("[0 multiplication] = point of infinity\n");
        printf("[%d multiplication] = ", 1);
        print_point(alpha);

        // 0P, 1P는 이미 출력했으니 2P부터 출력
        for (int cnt_i = 2; cnt_i <= 22; cnt_i++) {
            printf("\n[%d multiplication] = ", cnt_i);
            alpha = addition(alpha, original);
            print_point(alpha);

            // Point of infinity인 경우
            if (alpha.x == 0b0000 && alpha.y == 0b0000) {
                printf(" ====> point of infinity\n\n");

                print_point(original);
                printf("'s order is %d\n", cnt_i); // print order
                if (cnt_i == 22) { // order is 22 -> generator!!
                    printf("So, It is a generator!!\n");
                }
                printf("\n");
                break;
            }
        }
    }
}

// Point Labels
// Label: 특정 항목이나 데이터를 식별하기 위해 사용하는 이름
const char* get_label(Point p, Point points[], const char* labels[], int size) {
    if (p.x == 0 && p.y == 0) return "∞"; // Point at infinity

    for (int i = 0; i < size; i++) {
        if (points[i].x == p.x && points[i].y == p.y) {
            return labels[i];
        }
    }

    // 해당 점에 대한 label을 찾지 못했음
    return "?";
}

// Print Addition Table
void print_add_table(Point points[], const char* labels[], int size) {
    printf("    |");
    for (int j = 0; j < size; j++) {
        printf(" %4s", labels[j]);
    }
    printf("\n");
    printf("----+");
    for (int j = 0; j < size; j++) {
        printf("-----");
    }
    printf("\n");

    for (int i = 0; i < size; i++) {
        printf("%3s |", labels[i]);
        for (int j = 0; j < size; j++) {
            Point result = addition(points[i], points[j]);
            const char* res_label = get_label(result, points, labels, size);
            printf(" %4s", res_label);
        }
        printf("\n");
    }

    printf("\n");
}

/*
* 다른 파일에서 Addition Table.h를 사용하려면 주석 처리
int main() {
    Point points[] =
    {
        {0b0000, 0b1011}, {0b0001, 0b0000}, {0b0001, 0b0001},
        {0b0010, 0b1101}, {0b0010, 0b1111}, {0b0011, 0b1100},
        {0b0011, 0b1111}, {0b0101, 0b0000}, {0b0101, 0b0101},
        {0b0111, 0b1011}, {0b0111, 0b1100}, {0b1000, 0b0001},
        {0b1000, 0b1001}, {0b1001, 0b0110}, {0b1001, 0b1111},
        {0b1011, 0b0010}, {0b1011, 0b1001}, {0b1100, 0b0000},
        {0b1100, 0b1100}, {0b1111, 0b0100}, {0b1111, 0b1011}
    };

    const char* labels[] = {
        "P1",  "P2",  "P3",  "P4",  "P5",  "P6",  "P7",  "P8",  "P9",  "P10",
        "P11", "P12", "P13", "P14", "P15", "P16", "P17", "P18", "P19", "P20", "P21"
    };

    int size = sizeof(points) / sizeof(points[0]);

    print_add_table(points, labels, size);

    print_table(points, size);

    check_cyclic(points, size);

    return 0;
}

*/
