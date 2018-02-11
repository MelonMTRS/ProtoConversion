#include <Windows.h>

namespace memory {
	DWORD find_pattern(DWORD dw_address, DWORD dw_len, BYTE *b_mask, char *sz_mask);
	int scan(DWORD mode, char* content, char* mask);
}