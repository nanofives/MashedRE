// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h3 0x00566830: best 48.4% (regs 75.8%, ops 85.5%), class (b). fabs intrinsic ok; original keeps param_2 in ebx and builds the selector with add/or/add/or/shl 2 while the compiler distributes the shift (shl 3 + shl 2); permuter variants change semantics and are not used. */
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
typedef unsigned int uint;
typedef signed char sbyte;
double fabs(double);
#pragma intrinsic(fabs)
// MATCH: 0x00566830 FUN_00566830
void FUN_00566830(float *param_1,float *param_2)

{
  float fVar1;
  uint uVar2;
  uint uVar3;

  *param_1 = (float)fabs(*param_2);
  param_1[1] = (float)fabs(param_2[1]);
  fVar1 = (float)fabs(param_2[2]);
  param_1[2] = fVar1;
  uVar2 = 0x21312300 >>
          ((((param_1[0] > param_1[1]) * 2 | fVar1 < *param_1) * 2 | fVar1 < param_1[1]) << 2) & 3;
  param_1[uVar2] = 0.0;
  uVar2 = 1 << (int)uVar2 & 3;
  uVar3 = 1 << (sbyte)uVar2 & 3;
  param_1[uVar2] = param_2[uVar3];
  param_1[uVar3] = -param_2[uVar2];
  return;
}
