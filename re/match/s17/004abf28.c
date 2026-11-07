// FLAGS: /O1
/* MATCH-S17 0x004abf28: CRT a_env.c __crtGetEnvironmentStringsA (VS.NET 2002 libcmt). /O1.
   f_use (function static in the CRT source) is DAT_00773d48. The W-path size/malloc failure is written as an
   early FreeEnvironmentStringsW + return NULL (cross-jumped with the normal exit); the CRT text form with
   aEnv = NULL scored 81.7%. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
MGLOBAL(int, DAT_00773d48, 0x00773d48);
__declspec(dllimport) unsigned short * __stdcall GetEnvironmentStringsW(void);
__declspec(dllimport) unsigned long __stdcall GetLastError(void);
__declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int CodePage, unsigned long dwFlags,
    const unsigned short *lpWideCharStr, int cchWideChar, char *lpMultiByteStr, int cbMultiByte,
    const char *lpDefaultChar, int *lpUsedDefaultChar);
__declspec(dllimport) int __stdcall FreeEnvironmentStringsW(unsigned short *p);
__declspec(dllimport) char * __stdcall GetEnvironmentStrings(void);
__declspec(dllimport) int __stdcall FreeEnvironmentStringsA(char *p);
void * __cdecl FUN_004a45fb(unsigned int size);
void __cdecl FUN_004a460d(void *p);
void * __cdecl FUN_004aaff0(void *dst, const void *src, unsigned int n);
// MATCH: 0x004abf28 FUN_004abf28
void * __cdecl FUN_004abf28(void)
{
    unsigned short *wEnv = 0;
    unsigned short *wTmp;
    char *aEnv;
    char *aTmp;
    int nSizeW;
    int nSizeA;

    if (0 == DAT_00773d48) {
        if (0 != (wEnv = GetEnvironmentStringsW()))
            DAT_00773d48 = 1;
        else if (GetLastError() == 0x78)
            DAT_00773d48 = 2;
    }

    if (DAT_00773d48 == 1) {
        if (0 == wEnv)
            if (0 == (wEnv = GetEnvironmentStringsW()))
                return 0;

        wTmp = wEnv;
        while (*wTmp != 0) {
            if (*++wTmp == 0)
                wTmp++;
        }

        nSizeW = (int)(wTmp - wEnv) + 1;

        if ((nSizeA = WideCharToMultiByte(0, 0, wEnv, nSizeW, 0, 0, 0, 0)) == 0 ||
            (aEnv = (char *)FUN_004a45fb(nSizeA)) == 0)
        {
            FreeEnvironmentStringsW(wEnv);
            return 0;
        }
        if (!WideCharToMultiByte(0, 0, wEnv, nSizeW, aEnv, nSizeA, 0, 0)) {
            FUN_004a460d(aEnv);
            aEnv = 0;
        }

        FreeEnvironmentStringsW(wEnv);
        return aEnv;
    }

    if (DAT_00773d48 == 2 || DAT_00773d48 == 0) {
        if (0 == (aEnv = GetEnvironmentStrings()))
            return 0;

        aTmp = aEnv;
        while (*aTmp != '\0') {
            if (*++aTmp == '\0')
                aTmp++;
        }

        nSizeA = (int)(aTmp - aEnv) + 1;

        if (0 == (aTmp = (char *)FUN_004a45fb(nSizeA))) {
            FreeEnvironmentStringsA(aEnv);
            return 0;
        }

        FUN_004aaff0(aTmp, aEnv, nSizeA);
        FreeEnvironmentStringsA(aEnv);
        return aTmp;
    }

    return 0;
}
