// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005be5f0: 100% (regs 100%). Compile-fail raw fixed by hand: plain types for HANDLE/LONG, stdcall vtable call. Class: none. */
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
typedef int (__stdcall *vfn1)(int *);
int FUN_005bcca0(int p0, int *p1);
__declspec(dllimport) int __stdcall ReleaseSemaphore(HANDLE p0, long p1, long *p2);
// MATCH: 0x005be5f0 FUN_005be5f0
int FUN_005be5f0(int param_1, int *param_2)
{
  int iVar1;

  iVar1 = FUN_005bcca0(param_1 + 0x340, param_2);
  if (iVar1 == 0) {
    return 0;
  }
  ((vfn1)*(int *)(*param_2 + 8))(param_2);
  ReleaseSemaphore(*(HANDLE *)(param_1 + 0x36c), 1, (long *)0);
  return 1;
}
