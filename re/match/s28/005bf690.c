// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005bf690: 100% (regs 100%). Ghidra callees took 0 args; real ones take 1 (param_1), and the 0x4522d0 branch falls through to the shared return. Class: none. */
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
typedef unsigned int undefined4;
typedef long LONG;
void FUN_004522d0(int p0);
void FUN_005bf660(int p0);
__declspec(dllimport) LONG __stdcall InterlockedDecrement(LONG * p0);// MATCH: 0x005bf690 FUN_005bf690
LONG __stdcall FUN_005bf690(int param_1)
{
  LONG LVar1;

  LVar1 = InterlockedDecrement((LONG *)(param_1 + 4));
  if (LVar1 == 0) {
    if (*(int *)(param_1 + 8) != 0) {
      FUN_004522d0(param_1);
    } else {
      FUN_005bf660(param_1);
    }
  }
  return LVar1;
}
