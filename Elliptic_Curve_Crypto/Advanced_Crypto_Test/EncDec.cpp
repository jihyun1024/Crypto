#include <stdio.h>
#include "Addition Table.h"

// Ciphertext struct
typedef struct _CipherText {
	Point C1;
	Point C2;
}CipherText;

// Define Elgamal-type Encryption
CipherText encrypt(Point M, Point P, Point Q, int k) {
	CipherText cipher;

	cipher.C1 = multiplication(P, k);
	Point kQ = multiplication(Q, k);
	cipher.C2 = addition(M, kQ);

	return cipher;
}

// Define Elgamal-type Decryption
Point decrypt(CipherText cipher, int d) {
	Point dC1 = multiplication(cipher.C1, d);
	dC1.y = dC1.x ^ dC1.y; // negative of P: if P = (x,y), -P = (x,x+y)
	Point M = addition(cipher.C2, dC1);

	return M;
}

int main() {
	// Encryption
	// P: Point in E, d: Private key
	Point P = { 0b0101, 0b0000 };
	int d = 7;
	Point Q = multiplication(P, d);

	// M:A point on the elliptic curve E, corresponding to the plaintext.
	Point M = { 0b1011, 0b1001 };

	int k = 5;

	CipherText ct = encrypt(M, P, Q, k);

	printf("[Encryption Input]\n");
	printf("n = #E(F2^4)\n");
	printf("Generator = ");
	print_point(P);
	printf("\n");
	printf("Public Key = ");
	print_point(Q);
	printf("\n");
	printf("M = ");
	print_point(M);
	printf("\n\n");

	printf("(Choose k in [1,n-1])\n");
	printf("k = %d\n\n", k);

	printf("[Encryption Output]\n");
	printf("C1 = ");
	print_point(ct.C1);
	printf("\nC2 = ");
	print_point(ct.C2);
	printf("\n\n");

	// Decryption
	Point m = decrypt(ct, d);
	printf("[Decryption Input]\n");
	printf("n = #E(F2^4)\n");
	printf("Generator = ");
	print_point(P);
	printf("\n");
	printf("Private Key = %d\n", d);
	printf("C1 = ");
	print_point(ct.C1);
	printf("\nC2 = ");
	print_point(ct.C2);
	printf("\n");

	printf("\n[Decrypted Output]\n");
	printf("Dec_M = ");
	print_point(m);

	return 0;
}
