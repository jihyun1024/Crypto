#ifndef CNG_RNG_H
#define CNG_RNG_H

#include <Windows.h>
#include <bcrypt.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "bcrypt.lib")

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

void gen_random_bytes(unsigned char* buffer, size_t length);
void for_test_cng_rng();

#endif