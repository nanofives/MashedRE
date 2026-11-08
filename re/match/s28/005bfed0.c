// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005bfed0 : 100.0% (regs 100). By hand (raw did not compile: LPCRITICAL_SECTION global). Enter/LeaveCriticalSection imports at 0x5cc068/0x5cc06c, stdcall ret 0xc. Residual: none. */

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

__declspec(dllimport) void __stdcall EnterCriticalSection(void *p);
__declspec(dllimport) void __stdcall LeaveCriticalSection(void *p);
// MATCH: 0x005bfed0 FUN_005bfed0
int __stdcall FUN_005bfed0(int param_1,int *param_2,int param_3)
{
  int *piVar1;

  EnterCriticalSection((void *)*(int *)(param_1 + 0x134));
  (**(int (__stdcall **)(int *))(*param_2 + 4))(param_2);
  piVar1 = *(int **)(param_1 + 0x138);
  if (piVar1 != (int *)0) {
    (**(int (__stdcall **)(int *))(*piVar1 + 8))(piVar1);
  }
  *(int **)(param_1 + 0x138) = param_2;
  *(int *)(param_1 + 0x140) = param_3;
  LeaveCriticalSection((void *)*(int *)(param_1 + 0x134));
  return 0;
}
