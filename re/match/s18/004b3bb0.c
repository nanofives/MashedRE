// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b3bb0 (helper h1): 100% (regs 100%). Fix: callee pointer types, one reused local p for both results (cc160 takes cbe80's result). Residual: none. */
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
typedef unsigned int uint;
undefined4 * FUN_004cbe80(undefined4 * p0, undefined4 * p1, uint p2);
bool FUN_004cc160(undefined4 * p0, undefined4 * p1);
undefined4 * FUN_004cc230(undefined4 p0, int p1, undefined4 * p2);
// MATCH: 0x004b3bb0 FUN_004b3bb0
undefined4 * FUN_004b3bb0(undefined4 *param_1,undefined4 *param_2,uint param_3)
{
  undefined4 *p;

  p = FUN_004cc230(2,2,param_1);
  if (p != 0) {
    p = FUN_004cbe80(p,param_2,param_3);
    FUN_004cc160(p,0);
  }
  return p;
}
