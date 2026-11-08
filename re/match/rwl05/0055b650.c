// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055b650: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl05_20261107/results.tsv. */
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
// MATCH: 0x0055b650 FUN_0055b650

void FUN_0055b650(int *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float *pfVar8;
  int iVar9;
  
  pfVar8 = (float *)(*(int *)(*(int *)(*param_1 + 0x10) + 0x30) + param_1[1] * 0x20);
  *pfVar8 = *pfVar8 + *param_2;
  iVar9 = *(int *)(*(int *)(*param_1 + 0x10) + 0x30);
  *(float *)(iVar9 + 4 + param_1[1] * 0x20) = *(float *)(iVar9 + 4 + param_1[1] * 0x20) + param_2[1]
  ;
  iVar9 = *(int *)(*(int *)(*param_1 + 0x10) + 0x30);
  *(float *)(iVar9 + 8 + param_1[1] * 0x20) = *(float *)(iVar9 + 8 + param_1[1] * 0x20) + param_2[2]
  ;
  iVar9 = param_1[1] * 0x40 + **(int **)(*param_1 + 0x10);
  fVar5 = *param_3 - *(float *)(iVar9 + 0x30);
  fVar7 = param_3[1] - *(float *)(iVar9 + 0x34);
  fVar6 = param_3[2] - *(float *)(iVar9 + 0x38);
  fVar1 = *param_2;
  fVar2 = param_2[2];
  fVar3 = param_2[1];
  fVar4 = *param_2;
  pfVar8 = (float *)(param_1[1] * 0x20 + 0x10 + (*(int **)(*param_1 + 0x10))[0xc]);
  *pfVar8 = *pfVar8 + (param_2[2] * fVar7 - param_2[1] * fVar6);
  iVar9 = *(int *)(*(int *)(*param_1 + 0x10) + 0x30);
  *(float *)(iVar9 + 0x14 + param_1[1] * 0x20) =
       *(float *)(iVar9 + 0x14 + param_1[1] * 0x20) + (fVar6 * fVar1 - fVar2 * fVar5);
  iVar9 = *(int *)(*(int *)(*param_1 + 0x10) + 0x30);
  *(float *)(iVar9 + 0x18 + param_1[1] * 0x20) =
       *(float *)(iVar9 + 0x18 + param_1[1] * 0x20) + (fVar3 * fVar5 - fVar7 * fVar4);
  return;
}
