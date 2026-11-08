// FLAGS: /O1
/* MATCH-RWL03 0x005320b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned int uint;
typedef unsigned char byte;
// MATCH: 0x005320b0 FUN_005320b0
void FUN_005320b0(int param_1,byte *param_2,int param_3,float *param_4,float *param_5,float *param_6
                 ,float *param_7,int param_8)

{
  float *pfVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  byte bVar5;
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
  
  do {
    fVar2 = *param_5;
    fVar3 = param_5[1];
    fVar4 = param_5[2];
    bVar5 = *param_2;
    param_2 = param_2 + 4;
    pfVar1 = (float *)(param_3 + (uint)bVar5 * 0x40);
    fVar6 = *pfVar1;
    fVar7 = pfVar1[1];
    fVar8 = pfVar1[2];
    fVar9 = pfVar1[4];
    fVar10 = pfVar1[5];
    fVar11 = pfVar1[6];
    fVar12 = pfVar1[8];
    fVar13 = pfVar1[9];
    fVar14 = pfVar1[10];
    fVar15 = pfVar1[0xd];
    fVar16 = pfVar1[0xe];
    param_5 = param_5 + 3;
    *param_4 = fVar4 * fVar12 + pfVar1[0xc] + fVar3 * fVar9 + fVar2 * fVar6;
    param_4[1] = fVar4 * fVar13 + fVar15 + fVar3 * fVar10 + fVar2 * fVar7;
    param_4[2] = fVar4 * fVar14 + fVar16 + fVar3 * fVar11 + fVar2 * fVar8;
    param_4 = (float *)((int)param_4 + param_8);
    if (param_7 != (float *)0x0) {
      fVar2 = *param_7;
      fVar3 = param_7[1];
      fVar4 = param_7[2];
      param_7 = param_7 + 3;
      *param_6 = fVar4 * fVar12 + fVar3 * fVar9 + fVar2 * fVar6;
      param_6[1] = fVar4 * fVar13 + fVar3 * fVar10 + fVar2 * fVar7;
      param_6[2] = fVar4 * fVar14 + fVar3 * fVar11 + fVar2 * fVar8;
      param_6 = (float *)((int)param_6 + param_8);
    }
    param_1 = param_1 + -1;
  } while (param_1 != 0);
  return;
}
