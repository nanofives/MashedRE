// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h1 0x005742c0: 11.8% (regs 19.7%), class b. 1490 B x87 function, structure only (compiles after PTR_DAT_005ceabc typed float). Tried: raw converter form with global type fix, no search. cxx:ok. */
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
double sqrt(double);
#pragma intrinsic(sqrt)
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef long double float10;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, PTR_DAT_005ceabc, 0x005ceabc);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
float10 FUN_0055bbf0(int p0, int p1, float * p2);
void FUN_005655a0(float * p0, int p1);
undefined4 FUN_00565ab0(undefined4 * p0, undefined4 * p1);
void FUN_005748a0(void * p0, float * p1, float p2);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005cc32c, 0x005cc32c);
// MATCH: 0x005742c0 FUN_005742c0
/* [C1 2026-05-20] Signature recovered: `float10 FUN_005742c0(int part_array, uint part_count,
   undefined4 *out_inertia,
 */

float10 FUN_005742c0(int param_1,uint param_2,undefined4 *param_3,float *param_4,int param_5)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float *pfVar7;
  int iVar8;
  uint uVar9;
  float *pfVar10;
  float10 fVar11;
  int iStack_d0;
  int iStack_cc;
  float local_c8;
  float fStack_b4;
  float fStack_b0;
  float fStack_ac;
  float fStack_a8;
  float fStack_a4;
  float fStack_a0;
  float fStack_9c;
  float fStack_98;
  float fStack_94;
  int iStack_90;
  float fStack_8c;
  float *pfStack_88;
  float fStack_80;
  float fStack_7c;
  float fStack_78;
  float fStack_74;
  float fStack_70;
  float fStack_6c;
  float fStack_68;
  float fStack_64;
  float fStack_60;
  float fStack_5c;
  float fStack_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  float fStack_28;
  float fStack_24;
  float fStack_20;
  float fStack_1c;
  float fStack_18;
  float fStack_14;
  float fStack_10;
  float fStack_c;
  float fStack_8;
  float fStack_4;
  
  uVar9 = 0;
  local_c8 = 0.0;
  if (param_5 == 0) {
    param_4[2] = 0.0;
    param_4[1] = 0.0;
    *param_4 = 0.0;
  }
  local_4c = 0;
  local_50 = 0;
  local_54 = 0;
  local_40 = 0;
  local_44 = 0;
  local_48 = 0;
  local_34 = 0;
  local_38 = 0;
  local_3c = 0;
  pfVar7 = (float *)(**(code **)(DAT_007d3ff8 + 0x108))(param_2 * 0xc,0x10900);
  pfStack_88 = pfVar7;
  iVar8 = (**(code **)(DAT_007d3ff8 + 0x108))(param_2 * 4,0x10900);
  iStack_cc = (**(code **)(DAT_007d3ff8 + 0x108))(param_2 * 0x1c,0x10900);
  iStack_90 = iStack_cc;
  if (param_2 != 0) {
    iStack_d0 = param_1;
    do {
      fVar11 = (float10)FUN_0055bbf0(iStack_d0,iStack_cc,pfVar7);
      *(float *)(iVar8 + uVar9 * 4) = (float)fVar11;
      local_c8 = (float)((float10)local_c8 + fVar11);
      if (param_5 == 0) {
        *param_4 = (float)(fVar11 * (float10)*pfVar7 + (float10)*param_4);
        param_4[1] = pfVar7[1] * *(float *)(iVar8 + uVar9 * 4) + param_4[1];
        param_4[2] = *(float *)(iVar8 + uVar9 * 4) * pfVar7[2] + param_4[2];
      }
      uVar9 = uVar9 + 1;
      iStack_cc = iStack_cc + 0x1c;
      iStack_d0 = iStack_d0 + 0x60;
      pfVar7 = pfVar7 + 3;
    } while (uVar9 < param_2);
    if (local_c8 < PTR_DAT_005ceabc == (local_c8 == PTR_DAT_005ceabc)) {
      if (param_5 == 0) {
        fVar1 = _DAT_005cc320 / local_c8;
        *param_4 = fVar1 * *param_4;
        param_4[1] = param_4[1] * fVar1;
        param_4[2] = param_4[2] * fVar1;
      }
      uVar9 = 0;
      iStack_cc = iStack_90;
      pfVar7 = pfStack_88 + 2;
      pfVar10 = (float *)(param_1 + 0x10);
      do {
        if (DAT_005d757c < *(float *)(iVar8 + uVar9 * 4)) {
          FUN_005655a0(&fStack_b4,iStack_cc);
          fVar5 = fStack_b4 * pfVar10[-4] + fStack_ac * pfVar10[4] + fStack_b0 * *pfVar10;
          fVar1 = fStack_b4 * pfVar10[-3] + fStack_b0 * pfVar10[1] + fStack_ac * pfVar10[5];
          fStack_28 = fStack_b4 * pfVar10[-2] + fStack_b0 * pfVar10[2] + fStack_ac * pfVar10[6];
          fVar6 = fStack_a4 * *pfVar10 + fStack_a8 * pfVar10[-4] + fStack_a0 * pfVar10[4];
          fStack_80 = fStack_a8 * pfVar10[-3] + fStack_a4 * pfVar10[1] + fStack_a0 * pfVar10[5];
          fStack_7c = fStack_a8 * pfVar10[-2] + fStack_a4 * pfVar10[2] + fStack_a0 * pfVar10[6];
          fVar2 = fStack_98 * *pfVar10 + fStack_9c * pfVar10[-4] + fStack_94 * pfVar10[4];
          fVar3 = fStack_9c * pfVar10[-3] + fStack_98 * pfVar10[1] + fStack_94 * pfVar10[5];
          fVar4 = fStack_9c * pfVar10[-2] + fStack_98 * pfVar10[2] + fStack_94 * pfVar10[6];
          fStack_78 = fVar5 * pfVar10[-4] + fVar2 * pfVar10[4] + fVar6 * *pfVar10;
          fStack_68 = fStack_80 * pfVar10[1] + fVar3 * pfVar10[5] + fVar1 * pfVar10[-3];
          fStack_58 = fStack_28 * pfVar10[-2] + fStack_7c * pfVar10[2] + fVar4 * pfVar10[6];
          fStack_74 = (fVar3 * pfVar10[4] + fVar1 * pfVar10[-4] + fStack_80 * *pfVar10 +
                      fVar5 * pfVar10[-3] + fVar6 * pfVar10[1] + fVar2 * pfVar10[5]) * _DAT_005cc32c
          ;
          fStack_70 = (fVar5 * pfVar10[-2] + fVar6 * pfVar10[2] + fVar2 * pfVar10[6] +
                      fStack_28 * pfVar10[-4] + fVar4 * pfVar10[4] + fStack_7c * *pfVar10) *
                      _DAT_005cc32c;
          fStack_64 = (fStack_28 * pfVar10[-3] + fStack_7c * pfVar10[1] + fVar4 * pfVar10[5] +
                      fStack_80 * pfVar10[2] + fVar3 * pfVar10[6] + fVar1 * pfVar10[-2]) *
                      _DAT_005cc32c;
          fStack_8c = _DAT_005cc320 / local_c8;
          fStack_6c = fStack_74;
          fStack_60 = fStack_70;
          fStack_5c = fStack_64;
          FUN_005748a0(&local_54,&fStack_78,fStack_8c * *(float *)(iVar8 + uVar9 * 4));
          fStack_1c = pfVar7[-2] - *param_4;
          fVar1 = pfVar7[-1] - param_4[1];
          fStack_10 = *pfVar7 - param_4[2];
          fStack_24 = -(fVar1 * fVar1) - fStack_10 * fStack_10;
          fStack_14 = -(fStack_1c * fStack_1c) - fStack_10 * fStack_10;
          fStack_4 = -(fVar1 * fVar1) - fStack_1c * fStack_1c;
          fStack_20 = fVar1 * fStack_1c;
          fStack_1c = fStack_10 * fStack_1c;
          fStack_10 = fStack_10 * fVar1;
          fStack_18 = fStack_20;
          fStack_c = fStack_1c;
          fStack_8 = fStack_10;
          FUN_005748a0(&local_54,&fStack_24,-(fStack_8c * *(float *)(iVar8 + uVar9 * 4)));
        }
        uVar9 = uVar9 + 1;
        iStack_cc = iStack_cc + 0x1c;
        pfVar10 = pfVar10 + 0x18;
        pfVar7 = pfVar7 + 3;
      } while (uVar9 < param_2);
      FUN_00565ab0(param_3,&local_54);
      goto LAB_0057484e;
    }
  }
  local_c8 = 0.0;
  *param_3 = 0x3f800000;
  param_3[1] = 0x3f800000;
  param_3[2] = 0x3f800000;
  param_3[3] = 0;
  param_3[4] = 0;
  param_3[5] = 0;
  param_3[6] = 0x3f800000;
LAB_0057484e:
  (**(code **)(DAT_007d3ff8 + 0x10c))(pfStack_88);
  (**(code **)(DAT_007d3ff8 + 0x10c))(iVar8);
  (**(code **)(DAT_007d3ff8 + 0x10c))(iStack_90);
  return (float10)local_c8;
}
