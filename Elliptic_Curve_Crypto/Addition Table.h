#ifndef ADDITION_TABLE_H
#define ADDITION_TABLE_H

// Coefficient of Elliptic Curve
#define A 0b1000

// Point struct
typedef struct _Point {
    int x;
    int y;
} Point;

// GF(2^4) Inverse Table
extern int gf_inverse[16];

// GF(2^4) multiplication
int gf_mul(int a, int b);

// Elliptic curve operations
Point doubling(Point P1);
Point addition(Point P1, Point P2);
Point multiplication(Point P, int d);

// Utility functions
void print_bin(int num);
void print_point(Point point);
void print_table(Point points[], int size);
void check_cyclic(Point alpha);

#endif
