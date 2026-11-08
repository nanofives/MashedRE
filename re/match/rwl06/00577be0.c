// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00577be0: hand-matched (h2). 60.8% regs 60.8% ops. Second compare is f1*p4 > f2 (Ghidra's '< == ==' pair), which fixed the branch layout. Residual: the original loads the three components of p2 once into x87 registers (y, x, z) and keeps them live across both dot products; ours reloads and orders the terms differently, and no spelling (36 term-order permutations, local copies of the components and deltas) moved it. Class b. */
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
// MATCH: 0x00577be0 FUN_00577be0
undefined4
FUN_00577be0(float *param_1,float *param_2,float param_3,float param_4,float *param_5,float *param_6)
{
  float fVar1;
  float fVar2;
  undefined4 uVar3;

  fVar1 = param_2[2] * param_2[2] + param_2[1] * param_2[1] + *param_2 * *param_2;
  fVar2 = param_2[2] * (param_5[2] - param_1[2]) + param_2[1] * (param_5[1] - param_1[1]) + (param_5[0] - param_1[0]) * *param_2;
  if (fVar1 * param_3 < fVar2) {
    if (fVar1 * param_4 > fVar2) {
      param_3 = fVar2 / fVar1;
      uVar3 = 1;
    }
    else {
      uVar3 = 0;
      param_3 = param_4;
    }
  }
  else {
    uVar3 = 0;
  }
  *param_6 = param_3 * *param_2 + *param_1;
  param_6[1] = param_3 * param_2[1] + param_1[1];
  param_6[2] = param_3 * param_2[2] + param_1[2];
  return uVar3;
}
