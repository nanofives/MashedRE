// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h3 0x0056bb80: best 41.3% (regs 43.1%, ops 60.6%), class (b). fabs/sin/cos intrinsics ok; compare-with-memory form and temp scheduling differ (4 compare rewrites tried). */
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
typedef long double float10;
float10 FUN_004c3b30(float p0);
double sin(double);
double cos(double);
double fabs(double);
#pragma intrinsic(sin, cos, fabs)
MGLOBAL(float, _DAT_005cd03c, 0x005cd03c);
MGLOBAL(float, _DAT_005cc320, 0x005cc320);
MGLOBAL(float, _DAT_005cc32c, 0x005cc32c);
// MATCH: 0x0056bb80 FUN_0056bb80
void FUN_0056bb80(float *param_1,float *param_2,float param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  double ang;

  fVar9 = (float)FUN_004c3b30(param_2[2] * param_2[2] +
                                param_2[1] * param_2[1] + *param_2 * *param_2);
  if (fabs(fVar9) >= _DAT_005cd03c) {
    fVar6 = _DAT_005cc320 / fVar9;
    fVar3 = param_1[1];
    fVar4 = *param_1;
    fVar5 = param_1[2];
    fVar7 = fVar6 * *param_2;
    fVar8 = fVar6 * param_2[1];
    fVar6 = fVar6 * param_2[2];
    ang = fVar9 * param_3 * _DAT_005cc32c;
    fVar1 = (float)sin(ang);
    fVar2 = (float)cos(ang);
    *param_1 = fVar4 * fVar2 + (fVar7 * param_1[3] + (fVar5 * fVar8 - fVar3 * fVar6)) * fVar1;
    param_1[1] = fVar3 * fVar2 + (fVar8 * param_1[3] + (fVar4 * fVar6 - fVar5 * fVar7)) * fVar1;
    param_1[2] = fVar5 * fVar2 + (fVar6 * param_1[3] + (fVar3 * fVar7 - fVar4 * fVar8)) * fVar1;
    param_1[3] = fVar2 * param_1[3] - (fVar5 * fVar6 + fVar4 * fVar7 + fVar3 * fVar8) * fVar1;
  }
  return;
}
