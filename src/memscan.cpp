#include "memscan.h"

namespace memory {
	bool compare(const BYTE *p_data, const BYTE *b_mask, const char *sz_mask)
	{
		for (; *sz_mask; ++sz_mask, ++p_data, ++b_mask)
			if (*sz_mask == 'x' && *p_data != *b_mask) return FALSE;
		return (*sz_mask) == NULL;
	}

	DWORD find_pattern(DWORD dw_address, DWORD dw_len, BYTE *b_mask, char *sz_mask)
	{
		for (int i = 0; i < (int)dw_len; i++)
			if (compare((BYTE*)(dw_address + (int)i), b_mask, sz_mask))  return (int)(dw_address + i);
		return 0;
	}

	int scan(DWORD mode, char* content, char* mask)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		DWORD page_size = si.dwPageSize;
		MEMORY_BASIC_INFORMATION mi;
		for (DWORD lp_addr = 0; lp_addr < 0x7FFFFFFF; lp_addr += page_size)
		{
			DWORD vq = VirtualQuery((void*)lp_addr, &mi, page_size);
			if (vq == ERROR_INVALID_PARAMETER || vq == 0) break;
			if (mi.Type == MEM_MAPPED) continue;
			if (mi.Protect == mode)
			{
				int addr = find_pattern(lp_addr, page_size, (PBYTE)content, mask);
				if (addr != 0)
				{
					return addr;
				}
			}
		}
	}
}