// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x005735f0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
// MATCH: 0x005735f0 FUN_005735f0
void FUN_005735f0(float *param_1,float *param_2,float *param_3,float *param_4)

{
  double fVar1;
  double fVar3;
  double fVar5;

  if (param_1 == (float *)0x0) {
    param_4[2] = 0.0;
    param_4[1] = 0.0;
    *param_4 = 0.0;
    return;
  }
  fVar1 = *param_3 - *param_2;
  fVar3 = param_3[1] - param_2[1];
  fVar5 = param_3[2] - param_2[2];
  *param_4 = param_1[5] * fVar5 - param_1[6] * fVar3;
  param_4[1] = param_1[6] * fVar1 - param_1[4] * fVar5;
  param_4[2] = param_1[4] * fVar3 - param_1[5] * fVar1;
  *param_4 = *param_1 + *param_4;
  param_4[1] = param_1[1] + param_4[1];
  param_4[2] = param_1[2] + param_4[2];
  return;
}
