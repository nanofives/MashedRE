// FLAGS: /O2 /Ob1 /G5
// best 95.7 (regs 95.7). Residual: store [esi+0x13c]=0 is scheduled after the reload of [esi+0x130] (original: store first). Class (b); permuter 77 compiles no gain.
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
typedef struct _CRITICAL_SECTION CRITICAL_SECTION;
typedef unsigned int undefined4;
__declspec(dllimport) void __stdcall EnterCriticalSection(CRITICAL_SECTION *p0);
__declspec(dllimport) void __stdcall LeaveCriticalSection(CRITICAL_SECTION *p0);
undefined4 __stdcall FUN_005c0730(int p0);
// MATCH: 0x005bf8a0 FUN_005bf8a0
int __stdcall FUN_005bf8a0(int param_1)
{
  int iVar1;

  iVar1 = FUN_005c0730(param_1);
  if (iVar1 >= 0) {
    EnterCriticalSection(*(CRITICAL_SECTION **)(param_1 + 0x130));
    (**(void (__stdcall **)(int *))(**(int **)(param_1 + 0x13c) + 8))(*(int **)(param_1 + 0x13c));
    *(undefined4 *)(param_1 + 0x13c) = 0;
    LeaveCriticalSection(*(CRITICAL_SECTION **)(param_1 + 0x130));
  }
  return iVar1;
}
