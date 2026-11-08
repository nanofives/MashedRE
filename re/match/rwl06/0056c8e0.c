// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0056c8e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
// MATCH: 0x0056c8e0 FUN_0056c8e0
void FUN_0056c8e0(int param_1,int param_2,float *param_3,int param_4)

{
  float *pfVar1;
  float *pfVar2;
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
  int iVar15;
  
  iVar15 = param_4 * 0x20;
  fVar3 = *(float *)(iVar15 + 4 + param_2);
  fVar4 = *param_3;
  fVar5 = *(float *)(iVar15 + 8 + param_2);
  fVar6 = *param_3;
  fVar7 = *(float *)(iVar15 + 0x44 + param_2);
  fVar8 = param_3[1];
  fVar9 = *(float *)(iVar15 + 0x48 + param_2);
  fVar10 = param_3[1];
  pfVar1 = (float *)(iVar15 + 0x14 + param_1);
  fVar11 = *(float *)(iVar15 + 0x84 + param_2);
  fVar12 = param_3[2];
  fVar13 = *(float *)(iVar15 + 0x88 + param_2);
  fVar14 = param_3[2];
  pfVar2 = (float *)(iVar15 + 0x10 + param_1);
  *(float *)(iVar15 + param_1) =
       *(float *)((param_4 + 6) * 0x20 + param_2) * param_3[3] +
       *(float *)((param_4 + 4) * 0x20 + param_2) * param_3[2] +
       *(float *)((param_4 + 2) * 0x20 + param_2) * param_3[1] +
       *(float *)(iVar15 + param_2) * *param_3 + *(float *)(iVar15 + param_1);
  *(float *)(iVar15 + 4 + param_1) =
       *(float *)(iVar15 + 0xc4 + param_2) * param_3[3] +
       fVar11 * fVar12 + fVar7 * fVar8 + fVar3 * fVar4 + *(float *)(iVar15 + 4 + param_1);
  *(float *)(iVar15 + 8 + param_1) =
       *(float *)(iVar15 + 200 + param_2) * param_3[3] +
       fVar13 * fVar14 + fVar9 * fVar10 + fVar5 * fVar6 + *(float *)(iVar15 + 8 + param_1);
  fVar3 = *(float *)(iVar15 + 0x14 + param_2);
  fVar4 = *param_3;
  fVar5 = *(float *)(iVar15 + 0x18 + param_2);
  fVar6 = *param_3;
  fVar7 = *(float *)(iVar15 + 0x54 + param_2);
  fVar8 = param_3[1];
  fVar9 = *(float *)(iVar15 + 0x58 + param_2);
  fVar10 = param_3[1];
  fVar11 = *(float *)(iVar15 + 0x94 + param_2);
  fVar12 = param_3[2];
  fVar13 = *(float *)(iVar15 + 0x98 + param_2);
  fVar14 = param_3[2];
  *pfVar2 = *(float *)(iVar15 + 0xd0 + param_2) * param_3[3] +
            *(float *)(iVar15 + 0x90 + param_2) * param_3[2] +
            *(float *)(iVar15 + 0x50 + param_2) * param_3[1] +
            *(float *)(iVar15 + 0x10 + param_2) * *param_3 + *pfVar2;
  *pfVar1 = *(float *)(iVar15 + 0xd4 + param_2) * param_3[3] +
            fVar11 * fVar12 + fVar7 * fVar8 + fVar3 * fVar4 + *pfVar1;
  *(float *)(iVar15 + 0x18 + param_1) =
       *(float *)(iVar15 + 0xd8 + param_2) * param_3[3] +
       fVar13 * fVar14 + fVar9 * fVar10 + fVar5 * fVar6 + *(float *)(iVar15 + 0x18 + param_1);
  return;
}
