// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c75c0: see score. Compile-fail raw rewritten by hand (plain types, stdcall vtable calls). */
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
typedef void *HANDLE;
__declspec(dllimport) int __stdcall ReleaseSemaphore(HANDLE p0, long p1, long *p2);
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(HANDLE p0, unsigned long p1);
// MATCH: 0x005c75c0 FUN_005c75c0
void FUN_005c75c0(int param_1, unsigned int param_2)
{
  WaitForSingleObject(*(HANDLE *)(*(int *)(param_1 + 0x9c) + 4), 0xffffffff);
  *(unsigned int *)(param_1 + 0x98) = param_2;
  *(unsigned int *)(param_1 + 0xcc) = *(unsigned int *)(param_1 + 0xcc) & 0xfffffffb | 1;
  ReleaseSemaphore(*(HANDLE *)(*(int *)(param_1 + 0x9c) + 4), 1, (long *)0);
}
