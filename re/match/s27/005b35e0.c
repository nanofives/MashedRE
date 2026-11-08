// FLAGS: /O2 /GS /Ob1
/* MATCH-S27 h1: 30.8% (regs 69.2%, ops 88.5%), /O2 /GS /Ob1 (best of G5/G6/O1). Tried: flag sets, condition order, while/break form, permuter 200 s. Residual: loop head/param register order (param_2 loaded first, extra push/pop edi). Class: b. */
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
// MATCH: 0x005b35e0 FUN_005b35e0
void FUN_005b35e0(int *param_1,int *param_2)

{
  int *piVar1;
  
  for (piVar1 = (int *)param_1[1]; (piVar1 != param_1 + 1 && (piVar1 + -8 != param_2));
      piVar1 = (int *)*piVar1) {
  }
  *param_1 = *param_1 + -1;
  *(int *)param_2[9] = param_2[8];
  *(int *)(param_2[8] + 4) = param_2[9];
  return;
}
