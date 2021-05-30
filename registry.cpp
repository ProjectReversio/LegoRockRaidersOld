#include "registry.h"

char* getSubKey(LPCSTR type, char* subKey)
{
    char *v2;
    char* result;
    char i;

    v2 = subKey;
    result = (char*)type;
    *subKey = 0;
    for (i = *type; *result; i = *result)
    {
        if (i == '\\')
            break;
        ++result;
        *v2++ = i;
    }
    if (*result == '\\')
        ++result;
    *v2 = 0;

    return result;
}

HKEY getOrCreateRegKeyLM(LPCSTR type, LPCSTR lpValueName, int a3, LPBYTE lpData, DWORD cbData)
{
    return getOrCreateRegKey(HKEY_LOCAL_MACHINE, type, lpValueName, a3, lpData, cbData);
}

HKEY getOrCreateRegKey(HKEY hKey, LPCSTR type, LPCSTR lpValueName, int a4, LPBYTE outputData, DWORD bufSize)
{
    char subKey[100];
    char* v6 = getSubKey(type, subKey);

    if (subKey[0])
    {
        HKEY phkResult;
        RegCreateKey(hKey, subKey, &phkResult);
        HKEY result = phkResult;
        if (phkResult)
        {
            result = getOrCreateRegKey(phkResult, v6, lpValueName, a4, outputData, bufSize);
            RegCloseKey(phkResult);
        }
        return result;
    } else {
        type = nullptr;
        if (a4 == 0 || a4 == 1)
            return (HKEY)(RegQueryValueEx(hKey, lpValueName, 0, (LPDWORD)&type, outputData, &bufSize) == 0);
        else
            return nullptr;
    }
}
