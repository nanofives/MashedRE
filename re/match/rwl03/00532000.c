// FLAGS: /O1
/* MATCH-RWL03 0x00532000: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned __int64 undefined8;
#define CONCAT44(a,b) ((ulonglong)(((ulonglong)(uint)(a)<<32)|(uint)(b)))
// MATCH: 0x00532000 FUN_00532000
void FUN_00532000(int param_1,float *param_2,undefined8 *param_3,float *param_4,undefined8 *param_5,
                 float *param_6,int param_7)

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
  
  fVar4 = *param_2;
  fVar5 = param_2[1];
  fVar6 = param_2[2];
  fVar7 = param_2[4];
  fVar8 = param_2[5];
  fVar9 = param_2[6];
  fVar10 = param_2[8];
  fVar11 = param_2[9];
  fVar12 = param_2[10];
  fVar13 = param_2[0xc];
  fVar14 = param_2[0xd];
  fVar15 = param_2[0xe];
  do {
    fVar1 = *param_4;
    fVar2 = param_4[1];
    fVar3 = param_4[2];
    param_4 = param_4 + 3;
    *param_3 = CONCAT44(fVar3 * fVar11 + fVar14 + fVar2 * fVar8 + fVar1 * fVar5,
                        fVar3 * fVar10 + fVar13 + fVar2 * fVar7 + fVar1 * fVar4);
    *(float *)(param_3 + 1) = fVar3 * fVar12 + fVar15 + fVar2 * fVar9 + fVar1 * fVar6;
    param_3 = (undefined8 *)((int)param_3 + param_7);
    if (param_6 != (float *)0x0) {
      fVar1 = *param_6;
      fVar2 = param_6[1];
      fVar3 = param_6[2];
      param_6 = param_6 + 3;
      *param_5 = CONCAT44(fVar3 * fVar11 + fVar2 * fVar8 + fVar1 * fVar5,
                          fVar3 * fVar10 + fVar2 * fVar7 + fVar1 * fVar4);
      *(float *)(param_5 + 1) = fVar3 * fVar12 + fVar2 * fVar9 + fVar1 * fVar6;
      param_5 = (undefined8 *)((int)param_5 + param_7);
    }
    param_1 = param_1 + -1;
  } while (param_1 != 0);
  return;
}
