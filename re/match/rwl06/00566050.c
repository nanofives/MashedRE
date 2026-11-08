// FLAGS: /O2 /Ob1 /G5
/* RESULT: pending */
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
double fabs(double);
#pragma intrinsic(fabs)
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
// MATCH: 0x00566050 FUN_00566050
void FUN_00566050(float *param_1,float *param_2,float *param_3)
{
  float e[3];
  float c[3];
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;

  e[0] = param_2[0] - param_2[4];
  e[1] = param_2[1] - param_2[5];
  e[2] = param_2[2] - param_2[6];
  c[0] = param_2[4] + param_2[0];
  c[1] = param_2[5] + param_2[1];
  c[2] = param_2[6] + param_2[2];
  e[0] = e[0] * DAT_005cc32c;
  e[1] = e[1] * DAT_005cc32c;
  e[2] = e[2] * DAT_005cc32c;
  c[0] = c[0] * DAT_005cc32c;
  c[1] = c[1] * DAT_005cc32c;
  c[2] = c[2] * DAT_005cc32c;
  fVar6 = (float)fabs(param_3[0]) * e[0] + (float)fabs(param_3[8]) * e[2] + (float)fabs(param_3[4]) * e[1];
  fVar5 = (float)fabs(param_3[1]) * e[0] + (float)fabs(param_3[9]) * e[2] + (float)fabs(param_3[5]) * e[1];
  fVar4 = (float)fabs(param_3[2]) * e[0] + (float)fabs(param_3[10]) * e[2] + (float)fabs(param_3[6]) * e[1];
  fVar1 = param_3[0xc] + c[0] * param_3[0] + c[2] * param_3[8] + c[1] * param_3[4];
  fVar2 = param_3[0xd] + c[0] * param_3[1] + c[2] * param_3[9] + c[1] * param_3[5];
  fVar3 = param_3[0xe] + c[0] * param_3[2] + c[2] * param_3[10] + c[1] * param_3[6];
  param_1[4] = fVar1 - fVar6;
  param_1[5] = fVar2 - fVar5;
  param_1[6] = fVar3 - fVar4;
  param_1[0] = fVar6 + fVar1;
  param_1[1] = fVar5 + fVar2;
  param_1[2] = fVar4 + fVar3;
}
