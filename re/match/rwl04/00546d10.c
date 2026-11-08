// FLAGS: /O2 /Ob1 /G5
/* h2 note: 22.1% (regs 38.5, ops 73.8): raw form kept. Hand try with t[3] cross temp and original term order scored 14.8. cxx check: ok. class (b), x87 stack residency of w*w-(...) and term order. */
/* MATCH-RWL04 0x00546d10: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
// MATCH: 0x00546d10 FUN_00546d10
float * FUN_00546d10(float *param_1,int param_2,int param_3,float *param_4)

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
  float *pfVar13;
  float *pfVar14;
  float *pfVar15;
  int iVar16;
  
  fVar5 = param_4[3] * param_4[3] -
          (param_4[2] * param_4[2] + param_4[1] * param_4[1] + *param_4 * *param_4);
  fVar6 = *param_4 + *param_4;
  fVar7 = param_4[1] + param_4[1];
  fVar8 = param_4[2] + param_4[2];
  fVar9 = param_4[3] * fVar6;
  fVar10 = param_4[3] * fVar7;
  fVar11 = param_4[3] * fVar8;
  if (0 < param_3) {
    iVar16 = (int)param_1 - param_2;
    pfVar13 = (float *)(param_2 + 8);
    pfVar15 = param_1;
    do {
      fVar12 = *pfVar13 * fVar8 + fVar6 * pfVar13[-2] + pfVar13[-1] * fVar7;
      fVar1 = pfVar13[-2];
      fVar2 = *pfVar13;
      fVar3 = pfVar13[-1];
      fVar4 = pfVar13[-2];
      *pfVar15 = *pfVar13 * fVar10 - pfVar13[-1] * fVar11;
      pfVar15[1] = fVar11 * fVar1 - fVar2 * fVar9;
      pfVar15[2] = fVar3 * fVar9 - fVar10 * fVar4;
      *pfVar15 = fVar12 * *param_4 + *pfVar15;
      pfVar15[1] = param_4[1] * fVar12 + pfVar15[1];
      *(float *)(iVar16 + (int)pfVar13) = param_4[2] * fVar12 + *(float *)(iVar16 + (int)pfVar13);
      pfVar14 = pfVar13 + 3;
      param_3 = param_3 + -1;
      *pfVar15 = fVar5 * pfVar13[-2] + *pfVar15;
      pfVar15[1] = pfVar13[-1] * fVar5 + pfVar15[1];
      *(float *)(iVar16 + -0xc + (int)pfVar14) =
           *pfVar13 * fVar5 + *(float *)(iVar16 + -0xc + (int)pfVar14);
      pfVar13 = pfVar14;
      pfVar15 = pfVar15 + 3;
    } while (param_3 != 0);
  }
  return param_1;
}
