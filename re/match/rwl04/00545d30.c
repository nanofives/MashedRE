// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00545d30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(undefined1, DAT_005cc33c, 0x005cc33c);
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
MGLOBAL(int, DAT_007dc6d4, 0x007dc6d4);
MGLOBAL(int, DAT_007dc6d0, 0x007dc6d0);
MGLOBAL(int, DAT_007dc6cc, 0x007dc6cc);
MGLOBAL(int, DAT_007dc6c8, 0x007dc6c8);
MGLOBAL(undefined1, DAT_005cc34c, 0x005cc34c);
MGLOBAL(undefined1, DAT_005cc35c, 0x005cc35c);
float10 FUN_004c3910(float * p0, float * p1);
float10 FUN_004c39b0(float * p0, float * p1);
int FUN_004c4530(float * p0, int p1);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
undefined4 * FUN_004d8480(undefined4 * p0);
MGLOBAL(float, _DAT_005cc320, 0x005cc320);
MGLOBAL(float, _DAT_005cc33c, 0x005cc33c);
MGLOBAL(float, _DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, _DAT_005cc35c, 0x005cc35c);
MGLOBAL(float, _DAT_005cc34c, 0x005cc34c);
MGLOBAL(float, _DAT_005cea20, 0x005cea20);
double ROUND(double);
// MATCH: 0x00545d30 FUN_00545d30
/* [C1 2026-05-20] Five-arg parametric-curve evaluator that also computes a tangent/normal/binormal
   frame — extension of 0x005459c0. */

float10 FUN_00545d30(int param_1,int param_2,float param_3,float *param_4,float *param_5)

{
  float *pfVar1;
  float *pfVar2;
  float *pfVar3;
  float *pfVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  bool bVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  int iVar18;
  int iVar19;
  float10 fVar20;
  float10 fVar21;
  float *local_24;
  float f24;
  float fStack_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  float local_8;
  float local_4;
  
  if (param_3 < DAT_005d757c) {
    param_3 = -param_3;
  }
  if ((*(int *)(param_1 + 0xc) == 2) && (_DAT_005cc320 < param_3)) {
    fStack_20 = 0.0;
    param_3 = param_3 - (float)(uint)(int)ROUND(param_3);
  }
  if (param_2 != 10) {
    if (param_2 != 0xb) {
      local_24 = (float *)0x102;
      fStack_20 = (float)FUN_004d7ff0(1);
      FUN_004d8480((undefined4 *)&local_24);
      return (float10)_DAT_005cc33c;
    }
    if (param_3 < _DAT_005cc32c == (param_3 == _DAT_005cc32c)) {
      param_3 = (param_3 * _DAT_005cc35c + param_3 * param_3 * _DAT_005cc34c) - _DAT_005cc320;
    }
    else {
      param_3 = (param_3 + param_3) * param_3;
    }
  }
  if (param_3 == _DAT_005cc320) {
    iVar18 = *(int *)(param_1 + 8) + -4;
    iVar19 = 0xff;
    param_3 = 1.0;
  }
  else {
    param_3 = (float)(*(int *)(param_1 + 8) + -3) * param_3;
    iVar18 = (int)ROUND(param_3);
    param_3 = (param_3 - (float)iVar18) * _DAT_005cea20;
    iVar19 = (int)ROUND(param_3);
    param_3 = param_3 - (float)iVar19;
  }
  pfVar2 = (float *)(param_1 + 0x38 + iVar18 * 0xc);
  pfVar3 = (float *)(param_1 + 0x38 + (iVar18 * 3 + 3) * 4);
  pfVar4 = (float *)(param_1 + 0x38 + (iVar18 * 3 + 6) * 4);
  local_24 = (float *)(param_1 + 0x38 + (iVar18 * 3 + 9) * 4);
  FUN_004c4530(param_5,0);
  fVar5 = (*(float *)(DAT_007dc6b4 + 4 + iVar19 * 4) - *(float *)(DAT_007dc6b4 + iVar19 * 4)) *
          param_3 + *(float *)(DAT_007dc6b4 + iVar19 * 4);
  fVar6 = (*(float *)(DAT_007dc6b0 + 4 + iVar19 * 4) - *(float *)(DAT_007dc6b0 + iVar19 * 4)) *
          param_3 + *(float *)(DAT_007dc6b0 + iVar19 * 4);
  fVar7 = (*(float *)(DAT_007dc6ac + 4 + iVar19 * 4) - *(float *)(DAT_007dc6ac + iVar19 * 4)) *
          param_3 + *(float *)(DAT_007dc6ac + iVar19 * 4);
  fVar8 = (*(float *)(DAT_007dc6a8 + 4 + iVar19 * 4) - *(float *)(DAT_007dc6a8 + iVar19 * 4)) *
          param_3 + *(float *)(DAT_007dc6a8 + iVar19 * 4);
  param_5[0xc] = fVar5 * *pfVar2;
  param_5[0xd] = pfVar2[1] * fVar5;
  param_5[0xe] = pfVar2[2] * fVar5;
  param_5[0xc] = fVar6 * *pfVar3 + param_5[0xc];
  param_5[0xd] = fVar6 * pfVar3[1] + param_5[0xd];
  param_5[0xe] = fVar6 * pfVar3[2] + param_5[0xe];
  param_5[0xc] = fVar7 * *pfVar4 + param_5[0xc];
  param_5[0xd] = fVar7 * pfVar4[1] + param_5[0xd];
  param_5[0xe] = fVar7 * pfVar4[2] + param_5[0xe];
  param_5[0xc] = fVar8 * *local_24 + param_5[0xc];
  param_5[0xd] = fVar8 * local_24[1] + param_5[0xd];
  param_5[0xe] = fVar8 * local_24[2] + param_5[0xe];
  fVar5 = (*(float *)(DAT_007dc6c4 + 4 + iVar19 * 4) - *(float *)(DAT_007dc6c4 + iVar19 * 4)) *
          param_3 + *(float *)(DAT_007dc6c4 + iVar19 * 4);
  fVar6 = (*(float *)(DAT_007dc6c0 + 4 + iVar19 * 4) - *(float *)(DAT_007dc6c0 + iVar19 * 4)) *
          param_3 + *(float *)(DAT_007dc6c0 + iVar19 * 4);
  fVar7 = (*(float *)(DAT_007dc6bc + 4 + iVar19 * 4) - *(float *)(DAT_007dc6bc + iVar19 * 4)) *
          param_3 + *(float *)(DAT_007dc6bc + iVar19 * 4);
  fVar8 = (*(float *)(DAT_007dc6b8 + 4 + iVar19 * 4) - *(float *)(DAT_007dc6b8 + iVar19 * 4)) *
          param_3 + *(float *)(DAT_007dc6b8 + iVar19 * 4);
  local_18 = fVar8 * *local_24 + fVar7 * *pfVar4 + fVar6 * *pfVar3 + fVar5 * *pfVar2;
  local_14 = fVar8 * local_24[1] + fVar7 * pfVar4[1] + fVar6 * pfVar3[1] + pfVar2[1] * fVar5;
  local_10 = fVar8 * local_24[2] + fVar7 * pfVar4[2] + fVar6 * pfVar3[2] + pfVar2[2] * fVar5;
  fVar14 = (*(float *)(DAT_007dc6d4 + 4 + iVar19 * 4) - *(float *)(DAT_007dc6d4 + iVar19 * 4)) *
           param_3 + *(float *)(DAT_007dc6d4 + iVar19 * 4);
  fVar15 = (*(float *)(DAT_007dc6d0 + 4 + iVar19 * 4) - *(float *)(DAT_007dc6d0 + iVar19 * 4)) *
           param_3 + *(float *)(DAT_007dc6d0 + iVar19 * 4);
  fVar16 = (*(float *)(DAT_007dc6cc + 4 + iVar19 * 4) - *(float *)(DAT_007dc6cc + iVar19 * 4)) *
           param_3 + *(float *)(DAT_007dc6cc + iVar19 * 4);
  fVar17 = (*(float *)(DAT_007dc6c8 + 4 + iVar19 * 4) - *(float *)(DAT_007dc6c8 + iVar19 * 4)) *
           param_3 + *(float *)(DAT_007dc6c8 + iVar19 * 4);
  fVar5 = *pfVar2;
  fVar6 = pfVar2[2];
  pfVar1 = param_5 + 8;
  fVar7 = *pfVar3;
  fVar8 = pfVar3[2];
  fVar9 = *pfVar4;
  fVar10 = pfVar4[2];
  fVar11 = *local_24;
  fStack_20 = fVar17 * local_24[1] + fVar16 * pfVar4[1] + fVar15 * pfVar3[1] + pfVar2[1] * fVar14;
  fVar12 = local_24[2];
  *pfVar1 = local_18;
  local_1c = fVar17 * fVar12 + fVar16 * fVar10 + fVar15 * fVar8 + fVar6 * fVar14;
  param_5[9] = local_14;
  param_5[10] = local_10;
  f24 = fVar17 * fVar11 + fVar16 * fVar9 + fVar15 * fVar7 + fVar14 * fVar5;
  fVar20 = (float10)FUN_004c3910(pfVar1,pfVar1);
  if (fVar20 == (float10)DAT_005d757c) {
    *pfVar1 = 0.0;
    param_5[9] = 0.0;
    param_5[10] = 1.0;
  }
  local_c = param_5[10] * fStack_20 - param_5[9] * local_1c;
  local_8 = local_1c * *pfVar1 - param_5[10] * f24;
  local_4 = param_5[9] * f24 - fStack_20 * *pfVar1;
  fVar20 = (float10)FUN_004c39b0(&local_c,&local_c);
  if (param_4 == (float *)0x0) {
    bVar13 = (float)fVar20 == DAT_005d757c;
    param_5[4] = local_c;
    param_5[5] = local_8;
    param_5[6] = local_4;
    if (bVar13) {
      param_5[5] = 1.0;
    }
  }
  else {
    pfVar2 = param_5 + 4;
    *pfVar2 = *param_4;
    param_5[5] = param_4[1];
    param_5[6] = param_4[2];
    fVar21 = (float10)FUN_004c3910(pfVar2,pfVar2);
    if (fVar21 == (float10)DAT_005d757c) {
      param_5[5] = 1.0;
    }
  }
  pfVar2 = param_5 + 4;
  *param_5 = param_5[5] * param_5[10] - param_5[9] * param_5[6];
  param_5[1] = param_5[6] * *pfVar1 - param_5[10] * *pfVar2;
  param_5[2] = param_5[9] * *pfVar2 - param_5[5] * *pfVar1;
  fVar21 = (float10)FUN_004c3910(param_5,param_5);
  if (fVar21 == (float10)DAT_005d757c) {
    *param_5 = 1.0;
  }
  if (param_4 != (float *)0x0) {
    *pfVar2 = param_5[9] * param_5[2] - param_5[10] * param_5[1];
    param_5[5] = param_5[10] * *param_5 - param_5[2] * *pfVar1;
    param_5[6] = param_5[1] * *pfVar1 - param_5[9] * *param_5;
    fVar21 = (float10)FUN_004c3910(pfVar2,pfVar2);
    if (fVar21 == (float10)DAT_005d757c) {
      param_5[5] = 1.0;
    }
  }
  return (float10)(float)fVar20;
}
