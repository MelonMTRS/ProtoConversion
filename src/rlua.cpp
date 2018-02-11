#include "rlua.h"

void set_encode_key(int rl, DWORD* key) { // string "local", decompile for decode key
	DWORD decode_key = rl + RL_L_G_CKEY + *(DWORD *)(rl + RL_L_G) + *(DWORD *)(rl + RL_L_G_CKEY + *(DWORD *)(rl + RL_L_G));

	*key = inverse(decode_key, 4294967296);
}

static VOID rlua_pushlclosure(int rl, rl_closure* rlcl) {
	r_tvalue* top = *(r_tvalue**)(rl + RL_TOP);
	top->tt = RLUA_TFUNCTION;
	*(int*)(top) = (int)(rlcl);
	R_API_INCR_TOP(rl);
}

static double r_xor_number(double* number) {
	int rxor = addresses::xorconst;
	PVOID old_number = number;

	double new_number;

	__asm {
		mov edx, old_number
		mov eax, rxor
		movsd xmm1, qword ptr[edx]
		xorpd xmm1, [eax]
		movsd new_number, xmm1
	}

	return new_number;
}