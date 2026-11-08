// FLAGS: /O1
/* MATCH-RWL06 0x005cb0ef: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned __int64 undefined8;
#define CONCAT44(a,b) ((ulonglong)(((ulonglong)(uint)(a)<<32)|(uint)(b)))
// MATCH: 0x005cb0ef RwV3dTransformPoint
undefined8 __fastcall
__fastcall RwV3dTransformPoint(undefined4 param_1,undefined4 param_2,float *param_3,float *param_4,
                   float *param_5)

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
  
  fVar1 = *param_4;
  fVar2 = param_5[1];
  fVar3 = param_5[2];
  fVar4 = param_4[1];
  fVar5 = param_5[5];
  fVar6 = param_5[6];
  fVar7 = param_4[2];
  fVar8 = param_5[9];
  fVar9 = param_5[0xd];
  fVar10 = param_5[10];
  fVar11 = param_5[0xe];
  *param_3 = param_5[0xc] + param_5[8] * fVar7 + param_5[4] * fVar4 + *param_5 * fVar1;
  param_3[1] = fVar9 + fVar8 * fVar7 + fVar5 * fVar4 + fVar2 * fVar1;
  param_3[2] = fVar11 + fVar10 * fVar7 + fVar6 * fVar4 + fVar3 * fVar1;
  return CONCAT44(param_2,param_4);
}
