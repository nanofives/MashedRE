// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00559040: hand-matched (h1). 11.4% (regs 26.9%, ops 56.2%), 218/219 ins. Triangle frame (normal, tangents, centroid). Float globals typed float (were byte/int), inline sqrt via #pragma intrinsic. Not worked past that: x87 operand/scheduling differences throughout (class a/b), no spelling tried moved the stream. */
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
#pragma intrinsic(sqrt)
double __cdecl sqrt(double);
#define SQRT(x) ((float)sqrt((double)(x)))
typedef unsigned char undefined1;
MGLOBAL(float, DAT_005e4e28, 0x005e4e28);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005ccac8, 0x005ccac8);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
// MATCH: 0x00559040 FUN_00559040
/* [C1 2026-05-19] Computes three triangle edge vectors: */

bool FUN_00559040(float *param_1,float *param_2,float *param_3,float *param_4,float *param_5)

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
  bool bVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  
  fVar14 = DAT_005d757c;
  fVar1 = *param_4 - *param_5;
  fVar2 = param_4[1] - param_5[1];
  fVar3 = param_4[2] - param_5[2];
  fVar4 = *param_5 - *param_3;
  fVar5 = param_5[1] - param_3[1];
  fVar6 = param_5[2] - param_3[2];
  fVar11 = fVar6 * fVar2 - fVar5 * fVar3;
  fVar12 = fVar4 * fVar3 - fVar6 * fVar1;
  fVar13 = fVar5 * fVar1 - fVar4 * fVar2;
  fVar7 = fVar13 * fVar13 + fVar11 * fVar11 + fVar12 * fVar12;
  bVar10 = fVar7 <= DAT_005e4e28;
  if (bVar10) {
    *param_2 = 0.0;
  }
  else {
    *param_2 = SQRT(fVar7);
    fVar14 = DAT_005cc320 / SQRT(fVar7);
  }
  fVar11 = fVar11 * fVar14;
  fVar12 = fVar14 * fVar12;
  fVar13 = fVar14 * fVar13;
  fVar18 = (fVar2 * fVar13 - fVar12 * fVar3) * fVar14;
  fVar19 = (fVar11 * fVar3 - fVar13 * fVar1) * fVar14;
  fVar15 = (fVar12 * fVar1 - fVar11 * fVar2) * fVar14;
  fVar16 = (fVar13 * fVar5 - fVar12 * fVar6) * fVar14;
  fVar17 = (fVar11 * fVar6 - fVar13 * fVar4) * fVar14;
  fVar14 = (fVar12 * fVar4 - fVar11 * fVar5) * fVar14;
  fVar1 = *param_3;
  fVar2 = *param_4;
  fVar3 = param_3[1];
  fVar4 = param_4[1];
  fVar5 = param_3[2];
  fVar6 = param_4[2];
  fVar7 = *param_5;
  fVar8 = param_5[1];
  fVar9 = param_5[2];
  param_1[1] = fVar16;
  param_1[5] = fVar17;
  param_1[9] = fVar14;
  fVar1 = (fVar1 + fVar2 + fVar7) * DAT_005ccac8;
  fVar2 = (fVar8 + fVar3 + fVar4) * DAT_005ccac8;
  fVar3 = (fVar9 + fVar5 + fVar6) * DAT_005ccac8;
  *param_1 = fVar18;
  param_1[2] = -(fVar16 + fVar18);
  param_1[3] = fVar11;
  param_1[4] = fVar19;
  param_1[6] = -(fVar17 + fVar19);
  param_1[7] = fVar12;
  param_1[8] = fVar15;
  param_1[10] = -(fVar14 + fVar15);
  param_1[0xb] = fVar13;
  param_1[0xc] = -(param_5[2] * fVar15 + fVar18 * *param_5 + param_5[1] * fVar19);
  fVar4 = -(param_3[2] * fVar14 + fVar16 * *param_3 + param_3[1] * fVar17);
  param_1[0xd] = fVar4;
  param_1[0xe] = DAT_005cc320 - (fVar4 + param_1[0xc]);
  param_1[0xf] = -(fVar3 * fVar13 + fVar1 * fVar11 + fVar2 * fVar12);
  return !bVar10;
}
