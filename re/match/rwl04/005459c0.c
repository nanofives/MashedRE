// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x005459c0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef long double float10;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
MGLOBAL(undefined1, DAT_005cea20, 0x005cea20);
MGLOBAL(int, DAT_007dc6b4, 0x007dc6b4);
MGLOBAL(int, DAT_007dc6b0, 0x007dc6b0);
MGLOBAL(int, DAT_007dc6ac, 0x007dc6ac);
MGLOBAL(int, DAT_007dc6a8, 0x007dc6a8);
MGLOBAL(int, DAT_007dc6c4, 0x007dc6c4);
MGLOBAL(int, DAT_007dc6c0, 0x007dc6c0);
MGLOBAL(int, DAT_007dc6bc, 0x007dc6bc);
MGLOBAL(int, DAT_007dc6b8, 0x007dc6b8);
MGLOBAL(undefined1, DAT_005cc34c, 0x005cc34c);
MGLOBAL(undefined1, DAT_005cc35c, 0x005cc35c);
float10 FUN_004c3910(float * p0, float * p1);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005cc32c, 0x005cc32c);
MGLOBAL(int, _DAT_005cc35c, 0x005cc35c);
MGLOBAL(int, _DAT_005cc34c, 0x005cc34c);
MGLOBAL(int, _DAT_005cea20, 0x005cea20);
double ROUND(double);
// MATCH: 0x005459c0 FUN_005459c0
/* [C1 2026-05-20] Five-arg parametric-curve evaluator (int param_1, int param_2 (mode), float t,
   float *outPos, float *outDir). */

float * FUN_005459c0(int param_1,int param_2,float param_3,float *param_4,float *param_5)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  int iVar9;
  int iVar10;
  float10 fVar11;
  float *local_8;
  undefined4 uStack_4;
  
  if (param_3 < DAT_005d757c) {
    param_3 = -param_3;
  }
  if ((*(int *)(param_1 + 0xc) == 2) && (_DAT_005cc320 < param_3)) {
    uStack_4 = 0;
    param_3 = param_3 - (float)(uint)(int)ROUND(param_3);
  }
  if (param_2 != 10) {
    if (param_2 != 0xb) {
      local_8 = (float *)0x102;
      uStack_4 = FUN_004d7ff0(1);
      FUN_004d8480((undefined4 *)&local_8);
      return (float *)0x0;
    }
    if (param_3 < _DAT_005cc32c == (param_3 == _DAT_005cc32c)) {
      param_3 = (param_3 * _DAT_005cc35c + param_3 * param_3 * _DAT_005cc34c) - _DAT_005cc320;
    }
    else {
      param_3 = (param_3 + param_3) * param_3;
    }
  }
  if (param_3 == _DAT_005cc320) {
    iVar10 = *(int *)(param_1 + 8) + -4;
    iVar9 = 0xff;
    fVar4 = _DAT_005cc320;
  }
  else {
    param_3 = (float)(*(int *)(param_1 + 8) + -3) * param_3;
    iVar10 = (int)ROUND(param_3);
    fVar4 = (param_3 - (float)iVar10) * _DAT_005cea20;
    iVar9 = (int)ROUND(fVar4);
    fVar4 = fVar4 - (float)iVar9;
  }
  pfVar1 = (float *)(param_1 + 0x38 + (iVar10 * 3 + 3) * 4);
  pfVar2 = (float *)(param_1 + 0x38 + iVar10 * 0xc);
  pfVar3 = (float *)(param_1 + 0x38 + (iVar10 * 3 + 6) * 4);
  local_8 = (float *)(param_1 + 0x38 + (iVar10 * 3 + 9) * 4);
  fVar8 = (*(float *)(DAT_007dc6b4 + 4 + iVar9 * 4) - *(float *)(DAT_007dc6b4 + iVar9 * 4)) * fVar4
          + *(float *)(DAT_007dc6b4 + iVar9 * 4);
  fVar7 = (*(float *)(DAT_007dc6b0 + 4 + iVar9 * 4) - *(float *)(DAT_007dc6b0 + iVar9 * 4)) * fVar4
          + *(float *)(DAT_007dc6b0 + iVar9 * 4);
  fVar6 = (*(float *)(DAT_007dc6ac + 4 + iVar9 * 4) - *(float *)(DAT_007dc6ac + iVar9 * 4)) * fVar4
          + *(float *)(DAT_007dc6ac + iVar9 * 4);
  fVar5 = (*(float *)(DAT_007dc6a8 + 4 + iVar9 * 4) - *(float *)(DAT_007dc6a8 + iVar9 * 4)) * fVar4
          + *(float *)(DAT_007dc6a8 + iVar9 * 4);
  *param_4 = fVar8 * *pfVar2;
  param_4[1] = pfVar2[1] * fVar8;
  param_4[2] = pfVar2[2] * fVar8;
  *param_4 = fVar7 * *pfVar1 + *param_4;
  param_4[1] = fVar7 * pfVar1[1] + param_4[1];
  param_4[2] = fVar7 * pfVar1[2] + param_4[2];
  *param_4 = fVar6 * *pfVar3 + *param_4;
  param_4[1] = fVar6 * pfVar3[1] + param_4[1];
  param_4[2] = fVar6 * pfVar3[2] + param_4[2];
  *param_4 = fVar5 * *local_8 + *param_4;
  param_4[1] = fVar5 * local_8[1] + param_4[1];
  param_4[2] = fVar5 * local_8[2] + param_4[2];
  if (param_5 != (float *)0x0) {
    fVar7 = (*(float *)(DAT_007dc6c4 + 4 + iVar9 * 4) - *(float *)(DAT_007dc6c4 + iVar9 * 4)) *
            fVar4 + *(float *)(DAT_007dc6c4 + iVar9 * 4);
    fVar6 = (*(float *)(DAT_007dc6c0 + 4 + iVar9 * 4) - *(float *)(DAT_007dc6c0 + iVar9 * 4)) *
            fVar4 + *(float *)(DAT_007dc6c0 + iVar9 * 4);
    fVar5 = (*(float *)(DAT_007dc6bc + 4 + iVar9 * 4) - *(float *)(DAT_007dc6bc + iVar9 * 4)) *
            fVar4 + *(float *)(DAT_007dc6bc + iVar9 * 4);
    fVar4 = (*(float *)(DAT_007dc6b8 + 4 + iVar9 * 4) - *(float *)(DAT_007dc6b8 + iVar9 * 4)) *
            fVar4 + *(float *)(DAT_007dc6b8 + iVar9 * 4);
    *param_5 = fVar7 * *pfVar2;
    param_5[1] = pfVar2[1] * fVar7;
    param_5[2] = pfVar2[2] * fVar7;
    *param_5 = fVar6 * *pfVar1 + *param_5;
    param_5[1] = fVar6 * pfVar1[1] + param_5[1];
    param_5[2] = fVar6 * pfVar1[2] + param_5[2];
    *param_5 = fVar5 * *pfVar3 + *param_5;
    param_5[1] = fVar5 * pfVar3[1] + param_5[1];
    param_5[2] = fVar5 * pfVar3[2] + param_5[2];
    *param_5 = fVar4 * *local_8 + *param_5;
    param_5[1] = fVar4 * local_8[1] + param_5[1];
    param_5[2] = fVar4 * local_8[2] + param_5[2];
    fVar11 = (float10)FUN_004c3910(param_5,param_5);
    if (fVar11 == (float10)DAT_005d757c) {
      *param_5 = 0.0;
      param_5[1] = 1.0;
      param_5[2] = 0.0;
      return param_4;
    }
  }
  return param_4;
}
