// FLAGS: /O1
/* h3 hand-match: best 8.5% (regs 9.9%), class c. Original is hand-written SSE (movups/movaps alignment branch, shufps, mulps, addps, movhlps, movlps) with push ebp frame. cl 13.00.9176 has no __m128/_mm_*ps (tested: C2061) and __asm is banned; the C is only the scalar semantics (x87 build, no SSE). */
/* MATCH-RWL01 0x004dc300: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
typedef unsigned int uint;
#define CONCAT44(a,b) ((ulonglong)(((ulonglong)(uint)(a)<<32)|(uint)(b)))
// MATCH: 0x004dc300 FUN_004dc300
undefined8 * FUN_004dc300(undefined8 *param_1,float *param_2,float *param_3)

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
  
  if (((uint)param_3 & 0xf) == 0) {
    fVar4 = *param_3;
    fVar5 = param_3[1];
    fVar6 = param_3[2];
    fVar7 = param_3[4];
    fVar8 = param_3[5];
    fVar9 = param_3[6];
    fVar10 = param_3[8];
    fVar11 = param_3[9];
    fVar12 = param_3[10];
  }
  else {
    fVar4 = *param_3;
    fVar5 = param_3[1];
    fVar6 = param_3[2];
    fVar7 = param_3[4];
    fVar8 = param_3[5];
    fVar9 = param_3[6];
    fVar10 = param_3[8];
    fVar11 = param_3[9];
    fVar12 = param_3[10];
  }
  fVar1 = *param_2;
  fVar2 = param_2[1];
  fVar3 = param_2[2];
  *param_1 = CONCAT44(fVar3 * fVar11 + fVar2 * fVar8 + fVar1 * fVar5,
                      fVar3 * fVar10 + fVar2 * fVar7 + fVar1 * fVar4);
  *(float *)(param_1 + 1) = fVar3 * fVar12 + fVar2 * fVar9 + fVar1 * fVar6;
  return param_1;
}
