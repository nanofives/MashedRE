// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x005530f0 (h2): 67.8% (regs 67.8, ops 90.8). Raw ints/globals typed: float* segment view (p[-5..5]), float DAT, int *(int*)p=1, callee proto types; sum kept in param_3 slot. Residue: x87 spill slots/order (orig fstp [esp+1c], fst [esp+20] pairs vs ours qword spills), not moved by float10 vs float temps. class (b). */
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
typedef long double float10;
MGLOBAL(float, DAT_005cc320, 0x005cc320);
float10 FUN_004c3b30(float p0);
float10 FUN_004c3b90(float p0);
int FUN_0055deb0(int p0);
float * FUN_005c4bb0(int p0, int p1);
// MATCH: 0x005530f0 FUN_005530f0
int FUN_005530f0(int param_1,float param_2,float param_3)
{
  float fVar1;
  float fVar2;
  float fVar3;
  int iVar4;
  float *puVar5;
  float10 fVar6;
  float10 fVar7;

  iVar4 = FUN_0055deb0(**(int **)(param_1 + 0x18));
  puVar5 = FUN_005c4bb0(**(int **)(param_1 + 0x18),0x30190);
  fVar1 = param_3 - puVar5[-4];
  *(int *)puVar5 = 1;
  puVar5[1] = param_2;
  puVar5[2] = param_3;
  param_2 = puVar5[-5] - param_2;
  param_3 = fVar1 * fVar1 + param_2 * param_2;
  fVar6 = FUN_004c3b30(param_3);
  fVar7 = FUN_004c3b90(param_3);
  puVar5[3] = (float)((float10)fVar1 * fVar7);
  puVar5[4] = (float)(fVar7 * (float10)param_2);
  puVar5[5] = (float)fVar6 + puVar5[-1];
  if (1 < iVar4) {
    fVar1 = puVar5[-3] + (float)((float10)fVar1 * fVar7);
    fVar3 = puVar5[-2] + (float)(fVar7 * (float10)param_2);
    fVar2 = DAT_005cc320 / (puVar5[-2] * fVar3 + puVar5[-3] * fVar1);
    puVar5[-3] = fVar2 * fVar1;
    puVar5[-2] = fVar2 * fVar3;
    return param_1;
  }
  puVar5[-3] = puVar5[3];
  puVar5[-2] = puVar5[4];
  return param_1;
}
