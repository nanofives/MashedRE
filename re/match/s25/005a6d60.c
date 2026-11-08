// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a6d60: hand-matched (h1). */
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

/* best 50.0% (regs 72.7%): residual = scheduling. Original loads param_4 into edi first and *param_1 into edx before the
   table row address; ours interleaves differently. Tried ushort/int arg type, temps, inline expression, permuter (no gain).
   The (x!=0)?p:0 form drops the double neg of -(uint)(x!=0)&p. Class (b). */
typedef int (__cdecl *fn3)(undefined4, int, undefined4);
// MATCH: 0x005a6d60 FUN_005a6d60
uint FUN_005a6d60(undefined4 *param_1,int param_2,undefined4 param_3,undefined4 param_4)
{
  int iVar2;
  struct E { fn3 f; unsigned short s; } *e;
  e = (struct E *)(*(int *)(param_1[1] + 4) + param_2 * 8);
  iVar2 = e->f(*param_1,e->s,param_4);
  return (iVar2 != 0) ? (uint)param_1 : 0;
}
