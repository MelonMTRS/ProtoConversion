#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <fstream>
#include <sstream>
#include <vector>

/* misc lib of utility stuff thing */

int inverse(__int64 a, __int64 m);
void pause();
void resume();
BOOL WINAPI begin_console(LPCSTR console_title);
std::vector<std::string> split_string(const std::string &str, char delimiter);
