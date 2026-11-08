// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00565d50: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef long double float10;
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
MGLOBAL(undefined *, PTR_DAT_005ceabc, 0x005ceabc);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(float, _DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, _DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005ceabc, 0x005ceabc);
double sqrt(double);
#pragma intrinsic(sqrt)
// MATCH: 0x00565d50 FUN_00565d50
/* [C1 2026-05-19] Signature: `float10 FUN_00565d50(float *param_1, float *param_2, float *param_3,
   float *param_4)`. */

float10 FUN_00565d50(float *param_1,float *param_2,float *param_3,float *param_4)

{
  float fVar1;
  float fVar2;
  float10 fVar3;
  float fVar4;
  float10 fVar5;
  float10 fVar6;
  float10 fVar7;
  
  fVar5 = sqrt(((float10)param_3[2] - (float10)param_4[2]) *
               ((float10)param_3[2] - (float10)param_4[2]) +
               ((float10)param_3[1] - (float10)param_4[1]) *
               ((float10)param_3[1] - (float10)param_4[1]) +
               ((float10)*param_3 - (float10)*param_4) * ((float10)*param_3 - (float10)*param_4));
  fVar4 = sqrt((param_4[2] - param_2[2]) * (param_4[2] - param_2[2]) +
               (param_4[1] - param_2[1]) * (param_4[1] - param_2[1]) +
               (*param_4 - *param_2) * (*param_4 - *param_2));
  fVar6 = sqrt(((float10)param_2[2] - (float10)param_3[2]) *
               ((float10)param_2[2] - (float10)param_3[2]) +
               ((float10)param_2[1] - (float10)param_3[1]) *
               ((float10)param_2[1] - (float10)param_3[1]) +
               ((float10)*param_2 - (float10)*param_3) * ((float10)*param_2 - (float10)*param_3));
  fVar1 = (float)fVar6;
  fVar7 = (fVar6 + (float10)fVar4 + fVar5) * (float10)_DAT_005cc32c;
  fVar6 = (fVar7 - fVar5) * (fVar7 - (float10)fVar4) * (fVar7 - (float10)fVar1) * fVar7;
  if (fVar7 < (float10)DAT_005ceabc) {
    fVar7 = (float10)_DAT_005cc320;
  }
  fVar3 = (float10)_DAT_005cc320;
  param_1[3] = (float)((float10)(float)sqrt(fVar6) * (fVar3 / fVar7));
  *param_1 = (float)(fVar5 * (float10)*param_2);
  param_1[1] = (float)((float10)param_2[1] * fVar5);
  param_1[2] = (float)((float10)param_2[2] * fVar5);
  *param_1 = fVar4 * *param_3 + *param_1;
  param_1[1] = param_3[1] * fVar4 + param_1[1];
  param_1[2] = param_3[2] * fVar4 + param_1[2];
  *param_1 = fVar1 * *param_4 + *param_1;
  param_1[1] = param_4[1] * fVar1 + param_1[1];
  param_1[2] = param_4[2] * fVar1 + param_1[2];
  fVar6 = (fVar3 / fVar7) * (float10)_DAT_005cc32c;
  *param_1 = (float)(fVar6 * (float10)*param_1);
  param_1[1] = (float)((float10)param_1[1] * fVar6);
  param_1[2] = (float)((float10)param_1[2] * fVar6);
  fVar2 = fVar1;
  if (fVar1 <= fVar4) {
    fVar2 = fVar4;
  }
  if (fVar5 < (float10)fVar2) {
    if (fVar1 <= fVar4) {
      return (float10)fVar4;
    }
    fVar5 = (float10)fVar1;
  }
  return fVar5;
}
