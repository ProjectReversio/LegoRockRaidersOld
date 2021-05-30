#pragma once

#include <Windows.h>

extern HKEY getOrCreateRegKeyLM(LPCSTR type, LPCSTR lpValueName, int a3, LPBYTE lpData, DWORD cbData);
extern HKEY getOrCreateRegKey(HKEY hKey, LPCSTR type, LPCSTR lpValueName, int a4, LPBYTE outputData, DWORD bufSize);
