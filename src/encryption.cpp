#include "encryption.h"

uint32_t dax_encode_op(uint32_t x, uint32_t mul_even, uint32_t add_even, uint32_t mul_odd, uint32_t add_odd) {
	uint32_t result = 0;
	uint32_t mask = 1;
	for (size_t i = 0; i < 8 * sizeof(uint32_t); ++i) {
		uint32_t bitDesired = mask & x;
		uint32_t bitOdd = mask & (result*mul_odd + add_odd);
		uint32_t bitEven = mask & (result*mul_even + add_even);
		if ((bitEven ^ bitOdd) != bitDesired) {
			result |= mask;
		}
		mask <<= 1;
	}
	return result;
}

int call_encryption(int a1, int a2) {
	signed int v2;
	int v3;
	signed int v4;
	int v5;
	int v6;
	int v7;
	int v8;
	int v10;

	v2 = 1;
	v10 = a2;
	v3 = 0;
	v4 = 0;
	v5 = a1;
	do {
		v6 = ((v2 & (v10 + 32757935 * v3)) != 0) ^ ((v2 & (a2 - 29799480 * v3 - 5512095)) != 0);
		v7 = -((v5 & v2) != 0);
		v2 = _rotl(v2, 1);
		v8 = (-v7 ^ v6) << v4++;
		v3 |= v8;
		a2 = v10;
	} while (v4 < 26);
	return v3 | v5 & 0xFC000000;
}