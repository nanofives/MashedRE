// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0056fea0: hand pass (h2), 100.0% regs 100.0%. Raw converter output was already right except the constant: 0x005cc32c is a float (MGLOBAL float), not undefined1/int. */
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
// MATCH: 0x0056fea0 FUN_0056fea0
/* [C1 2026-05-19] Signature: `void FUN_0056fea0(float *param_1, float *param_2, float *param_3,
   float *param_4)`. */

void FUN_0056fea0(float *param_1,float *param_2,float *param_3,float *param_4)

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
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  
  fVar9 = param_2[3] * param_1[3];
  fVar11 = *param_2 * param_1[3];
  fVar12 = param_2[1] * param_1[3];
  fVar14 = param_2[2] * param_1[3];
  fVar1 = *param_1 * param_2[3];
  fVar2 = *param_1 * *param_2;
  fVar16 = param_2[1] * *param_1;
  fVar3 = param_2[2] * *param_1;
  fVar13 = param_2[3] * param_1[1];
  fVar15 = *param_2 * param_1[1];
  fVar4 = param_2[1] * param_1[1];
  fVar5 = param_2[2] * param_1[1];
  fVar6 = param_1[2] * param_2[3];
  fVar7 = param_1[2] * *param_2;
  fVar8 = param_1[2] * param_2[1];
  fVar10 = param_1[2] * param_2[2];
  param_3[3] = fVar10 + fVar4 + fVar2 + fVar9;
  *param_3 = ((fVar8 - fVar1) + fVar11) - fVar5;
  param_3[1] = (fVar3 - (fVar7 + fVar13)) + fVar12;
  param_3[2] = ((fVar15 - fVar6) - fVar16) + fVar14;
  *param_4 = (((fVar1 - fVar11) - fVar5) + fVar8) * DAT_005cc32c;
  param_4[1] = (((fVar13 - fVar12) + fVar3) - fVar7) * DAT_005cc32c;
  param_4[2] = ((fVar6 - (fVar16 + fVar14)) + fVar15) * DAT_005cc32c;
  param_4[3] = (((fVar2 + fVar9) - fVar10) - fVar4) * DAT_005cc32c;
  param_4[4] = (fVar6 + fVar15 + fVar16 + fVar14) * DAT_005cc32c;
  param_4[5] = (((fVar3 - fVar12) + fVar7) - fVar13) * DAT_005cc32c;
  param_4[6] = (((fVar15 - fVar6) - fVar14) + fVar16) * DAT_005cc32c;
  param_4[7] = (((fVar4 - fVar10) + fVar9) - fVar2) * DAT_005cc32c;
  param_4[8] = (fVar8 + fVar5 + fVar1 + fVar11) * DAT_005cc32c;
  param_4[9] = (fVar7 + fVar13 + fVar3 + fVar12) * DAT_005cc32c;
  param_4[10] = (((fVar8 + fVar5) - fVar1) - fVar11) * DAT_005cc32c;
  param_4[0xb] = (((fVar10 - fVar4) - fVar2) + fVar9) * DAT_005cc32c;
  return;
}
