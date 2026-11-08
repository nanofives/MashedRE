// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h1 0x005655a0: 14.8% (regs 14.8%), class b. quaternion-to-matrix: original stores every product in its own 4-byte slot (0x74 frame), x87 order. Tried: Ghidra form with fabs-free float locals only, structure only (468 B). cxx:ok. */
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
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x005655a0 FUN_005655a0
void FUN_005655a0(float *param_1,float *param_2)
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

  fVar1 = param_2[6];
  fVar7 = -param_2[3];
  fVar6 = -param_2[4];
  fVar2 = -param_2[5];
  fVar3 = fVar2 * fVar2 + fVar6 * fVar6;
  fVar10 = DAT_005cc320 - (fVar3 + fVar3);
  fVar8 = fVar6 * fVar7 + fVar2 * fVar1;
  fVar8 = fVar8 + fVar8;
  fVar4 = fVar2 * fVar7 - fVar6 * fVar1;
  fVar4 = fVar4 + fVar4;
  fVar9 = fVar6 * fVar7 - fVar2 * fVar1;
  fVar9 = fVar9 + fVar9;
  fVar3 = fVar2 * fVar2 + fVar7 * fVar7;
  fVar11 = DAT_005cc320 - (fVar3 + fVar3);
  fVar3 = fVar7 * fVar1 + fVar2 * fVar6;
  fVar3 = fVar3 + fVar3;
  fVar5 = fVar6 * fVar1 + fVar2 * fVar7;
  fVar5 = fVar5 + fVar5;
  fVar2 = fVar2 * fVar6 - fVar7 * fVar1;
  fVar2 = fVar2 + fVar2;
  fVar1 = fVar6 * fVar6 + fVar7 * fVar7;
  fVar1 = DAT_005cc320 - (fVar1 + fVar1);
  param_1[0] = fVar4 * param_2[2] * fVar4 + fVar8 * fVar8 * param_2[1] + fVar10 * param_2[0] * fVar10;
  param_1[1] = fVar3 * param_2[2] * fVar4 + fVar11 * fVar8 * param_2[1] + fVar9 * param_2[0] * fVar10;
  param_1[2] = fVar1 * param_2[2] * fVar4 + fVar2 * fVar8 * param_2[1] + fVar5 * param_2[0] * fVar10;
  param_1[4] = fVar3 * param_2[2] * fVar3 + fVar11 * fVar11 * param_2[1] + fVar9 * param_2[0] * fVar9;
  param_1[5] = fVar1 * param_2[2] * fVar3 + fVar2 * fVar11 * param_2[1] + fVar5 * param_2[0] * fVar9;
  param_1[8] = fVar1 * param_2[2] * fVar1 + fVar2 * fVar2 * param_2[1] + fVar5 * param_2[0] * fVar5;
  param_1[3] = param_1[1];
  param_1[6] = param_1[2];
  param_1[7] = param_1[5];
}
