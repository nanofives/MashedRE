// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00566200: hand-matched (h2). 39.7% regs 39.7% ops 89.0% under /O2 /Ob1 /G5 (raw /O1 file 14.5%). float, (float)fabs, 0x005cc32c as float. Box vectors e[3], s[3] as float arrays (x87 values spilled between steps, as in the original); s scaled in place. Residual: original evaluates the three e/s pairs interleaved with loads (e kept on the x87 stack) and a different frame (0x24 vs 0x20); source-order changes tried only partly. Class b. */
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
typedef unsigned char undefined1;
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
double fabs(double x);
#pragma intrinsic(fabs)
// MATCH: 0x00566200 FUN_00566200
void FUN_00566200(float *param_1,float *param_2,float *param_3)
{
  float e[3];
  float s[3];
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;

  e[0] = *param_2 - param_2[4];
  e[1] = param_2[1] - param_2[5];
  e[2] = param_2[2] - param_2[6];
  s[0] = param_2[4] + *param_2;
  s[1] = param_2[5] + param_2[1];
  s[2] = param_2[6] + param_2[2];
  fVar1 = e[0] * DAT_005cc32c;
  fVar2 = e[1] * DAT_005cc32c;
  fVar3 = e[2] * DAT_005cc32c;
  s[0] = s[0] * DAT_005cc32c;
  s[1] = s[1] * DAT_005cc32c;
  s[2] = s[2] * DAT_005cc32c;
  fVar8 = (float)fabs(*param_3) * fVar1 + (float)fabs(param_3[2]) * fVar3 + (float)fabs(param_3[1]) * fVar2;
  fVar7 = (float)fabs(param_3[4]) * fVar1 + (float)fabs(param_3[6]) * fVar3 + (float)fabs(param_3[5]) * fVar2;
  fVar1 = (float)fabs(param_3[8]) * fVar1 + (float)fabs(param_3[10]) * fVar3 + (float)fabs(param_3[9]) * fVar2;
  fVar6 = s[0] - param_3[0xc];
  fVar2 = s[1] - param_3[0xd];
  fVar4 = s[2] - param_3[0xe];
  fVar5 = param_3[2] * fVar4 + fVar6 * *param_3 + param_3[1] * fVar2;
  fVar3 = param_3[6] * fVar4 + param_3[4] * fVar6 + param_3[5] * fVar2;
  fVar2 = param_3[10] * fVar4 + param_3[8] * fVar6 + param_3[9] * fVar2;
  param_1[4] = fVar5 - fVar8;
  param_1[5] = fVar3 - fVar7;
  param_1[6] = fVar2 - fVar1;
  *param_1 = fVar8 + fVar5;
  param_1[1] = fVar7 + fVar3;
  param_1[2] = fVar1 + fVar2;
}
