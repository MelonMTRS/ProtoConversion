#include "utilities.h"

int inverse(__int64 a, __int64 m) {
	__int64 m0 = m, t, q;
	__int64 x0 = 0, x1 = 1;
	while (a > 1)
	{
		q = a / m;
		t = m;
		m = a % m, a = t;
		t = x0;
		x0 = x1 - q * x0;
		x1 = t;
	}

	if (x1 < 0)
		x1 += m0;
	return x1;
}

void pause() {
	THREADENTRY32 te32;
	te32.dwSize = sizeof(THREADENTRY32);
	HANDLE h_threads = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (Thread32First(h_threads, &te32)) {
		while (Thread32Next(h_threads, &te32)) {
			if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId()) {
				HANDLE h_thread = OpenThread(THREAD_SUSPEND_RESUME, false, te32.th32ThreadID);
				SuspendThread(h_thread);
				CloseHandle(h_thread);
			}
		}
	}
	CloseHandle(h_threads);
}

void resume() {
	THREADENTRY32 te32;
	te32.dwSize = sizeof(THREADENTRY32);
	HANDLE h_threads = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (Thread32First(h_threads, &te32)) {
		while (Thread32Next(h_threads, &te32)) {
			if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId()) {
				HANDLE h_thread = OpenThread(THREAD_SUSPEND_RESUME, false, te32.th32ThreadID);
				ResumeThread(h_thread);
				CloseHandle(h_thread);
			}
		}
	}
	CloseHandle(h_threads);
}

BOOL WINAPI begin_console(LPCSTR console_title)
{
	/* i eat ass */
	DWORD n_old_protect;
	if (!VirtualProtect(FreeConsole, 1, PAGE_EXECUTE_READWRITE, &n_old_protect))
		return FALSE;

	*(BYTE*)(FreeConsole) = 0xC3;
	if (!VirtualProtect(FreeConsole, 1, n_old_protect, &n_old_protect))
		return FALSE;

	SetConsoleTitleA(console_title);
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdout);
	return TRUE;
}

std::vector<std::string> split_string(const std::string &str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string tok;

	while (std::getline(ss, tok, delimiter))
		tokens.push_back(tok);

	return tokens;
}