// FLAGS: /O1
/* MATCH-S17 0x004a5e35: CRT testfdiv.c _ms_p5_mp_test_fdiv: IsProcessorFeaturePresent(0) via
   GetProcAddress(GetModuleHandleA("KERNEL32")), else _ms_p5_test_fdiv FUN_004a5df5. */
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
typedef int (__stdcall *ISPFP)(unsigned long);
__declspec(dllimport) void *__stdcall GetModuleHandleA(const char *lpModuleName);
__declspec(dllimport) void *__stdcall GetProcAddress(void *hModule, const char *lpProcName);
int FUN_004a5df5(void);
// MATCH: 0x004a5e35 FUN_004a5e35
int FUN_004a5e35(void)
{
    void *LibInst;
    ISPFP IsProcessorFeaturePresent;

    LibInst = GetModuleHandleA("KERNEL32");
    if (LibInst) {
        IsProcessorFeaturePresent = (ISPFP)GetProcAddress(LibInst, "IsProcessorFeaturePresent");
        if (IsProcessorFeaturePresent)
            return IsProcessorFeaturePresent(0);
    }
    return FUN_004a5df5();
}
