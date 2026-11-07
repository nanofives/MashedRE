// FLAGS: /O1
/* MATCH-S17 0x004af31a: CRT unhandld.cpp __CxxSetUnhandledExceptionFilter. No row in re/console/pc_funcs.csv (match.py
   refuses: no Ghidra size); scored with match.py score_function/byte_compare and an explicit size of 19 B (odis shows
   the ret at +18). pOldExceptFilter = DAT_00773d74. */
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
typedef long (__stdcall *PTOP_LEVEL)(void *);
MGLOBAL(PTOP_LEVEL, DAT_00773d74, 0x00773d74);
__declspec(dllimport) PTOP_LEVEL __stdcall SetUnhandledExceptionFilter(PTOP_LEVEL f);
long __stdcall FUN_004af2d4(void *pExPtrs);
// MATCH: 0x004af31a FUN_004af31a
int FUN_004af31a(void)
{
    DAT_00773d74 = SetUnhandledExceptionFilter(FUN_004af2d4);
    return 0;
}
