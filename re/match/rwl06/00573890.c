// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00573890: hand-matched (h2). 67.6% regs 67.6% ops 100.0%. Ghidra float10 locals/returns are float (0x005739d0 returns float, DAT_005cc320 is a float). Every instruction matches; only stack offsets differ: the original frame is 0x28 with the spill temps living in the dead argument slots (fst [esp+0x3c..0x48] after the call), ours has its own. 9 locals as float L[10]. Class a (spill-slot allocation of the period compiler). */
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
MGLOBAL(float, DAT_005cc320, 0x005cc320);
float FUN_005739d0(undefined4 p0, float * p1, float * p2, float * p3);
// MATCH: 0x00573890 FUN_00573890
undefined4 FUN_00573890(undefined4 param_1,float *param_2,float *param_3,float *param_4)
{
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float L[10];

  fVar2 = FUN_005739d0(param_1,&L[3],&L[0],&L[6]);
  *param_4 = fVar2;
  fVar2 = DAT_005cc320 / fVar2;
  *param_2 = L[3] * fVar2;
  param_2[1] = L[4] * fVar2;
  param_2[2] = L[5] * fVar2;
  fVar3 = (L[2] + L[1]) * fVar2;
  *param_3 = fVar3;
  fVar4 = (L[0] + L[2]) * fVar2;
  param_3[4] = fVar4;
  fVar5 = (L[0] + L[1]) * fVar2;
  param_3[8] = fVar5;
  fVar6 = -(L[6] * fVar2);
  param_3[3] = fVar6;
  param_3[1] = fVar6;
  fVar7 = -(L[7] * fVar2);
  param_3[7] = fVar7;
  param_3[5] = fVar7;
  fVar2 = -(L[8] * fVar2);
  param_3[2] = fVar2;
  param_3[6] = fVar2;
  *param_3 = fVar3 - (param_2[2] * param_2[2] + param_2[1] * param_2[1]);
  param_3[4] = fVar4 - (param_2[2] * param_2[2] + *param_2 * *param_2);
  param_3[8] = fVar5 - (param_2[1] * param_2[1] + *param_2 * *param_2);
  param_3[3] = param_2[1] * *param_2 + fVar6;
  param_3[1] = param_3[3];
  param_3[7] = param_2[2] * param_2[1] + fVar7;
  param_3[5] = param_3[7];
  param_3[2] = param_2[2] * *param_2 + fVar2;
  param_3[6] = param_3[2];
  return 1;
}
