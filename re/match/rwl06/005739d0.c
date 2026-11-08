// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x005739d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef signed char sbyte;
typedef long double float10;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc31c, 0x005cc31c);
MGLOBAL(float, DAT_005cc35c, 0x005cc35c);
MGLOBAL(float, DAT_005ce1d8, 0x005ce1d8);
MGLOBAL(float, DAT_005e58b0, 0x005e58b0);
MGLOBAL(float, DAT_005e58a8, 0x005e58a8);
MGLOBAL(float, DAT_005e3380, 0x005e3380);
MGLOBAL(float, DAT_005e58a0, 0x005e58a0);
MGLOBAL(float, DAT_005e5898, 0x005e5898);
MGLOBAL(float, DAT_005e5890, 0x005e5890);
MGLOBAL(float, DAT_005e5888, 0x005e5888);
MGLOBAL(float, DAT_005e5880, 0x005e5880);
MGLOBAL(float, DAT_005e5878, 0x005e5878);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005ccac8, 0x005ccac8);
undefined4 FUN_00565bc0(float * p0);
// MATCH: 0x005739d0 FUN_005739d0
/* [C1 2026-05-20] Signature recovered: `float FUN_005739d0(int mesh, float *p_first_moments,
   float *p_diag_second,
 */

float FUN_005739d0(int param_1,float *param_2,float *param_3,float *param_4)

{
  float *pfVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  int iVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float fVar20;
  float fVar21;
  float fVar22;
  float fVar23;
  float fVar24;
  float fVar25;
  float fVar26;
  int iVar27;
  uint uVar28;
  ushort *puVar29;
  float *pfVar30;
  uint uVar31;
  float local_60;
  float local_5c;
  float local_54;
  float local_50;
  uint local_44;
  float local_34;
  int local_30;
  float local_2c;
  int local_4;
  
  param_4[2] = 0.0;
  param_4[1] = 0.0;
  *param_4 = 0.0;
  param_3[2] = 0.0;
  param_3[1] = 0.0;
  *param_3 = 0.0;
  param_2[2] = 0.0;
  param_2[1] = 0.0;
  *param_2 = 0.0;
  local_2c = 0.0;
  local_44 = 0;
  if (*(short *)(param_1 + 0x1e) != 0) {
    local_30 = 0;
    do {
      uVar28 = (uint)*(ushort *)(*(int *)(param_1 + 0x10) + local_44 * 2);
      local_4 = *(ushort *)(*(int *)(param_1 + 0x10) + 2 + local_44 * 2) - uVar28;
      pfVar30 = (float *)(*(int *)(param_1 + 0xc) + local_30);
      uVar28 = (uint)*(ushort *)(*(int *)(param_1 + 0x14) + 4 + uVar28 * 0xc);
      pfVar1 = (float *)(*(int *)(param_1 + 4) + uVar28 * 0xc);
      fVar8 = -(pfVar1[2] * pfVar30[2] +
               *pfVar1 * *pfVar30 +
               *(float *)(*(int *)(param_1 + 4) + 4 + uVar28 * 0xc) * pfVar30[1]);
      iVar27 = FUN_00565bc0(pfVar30);
      uVar28 = 1 << ((byte)iVar27 & 0x1f) & 3;
      local_34 = 0.0;
      local_5c = 0.0;
      local_60 = 0.0;
      local_54 = 0.0;
      local_50 = 0.0;
      uVar31 = 1 << (sbyte)uVar28 & 3;
      fVar18 = DAT_005d757c;
      fVar19 = DAT_005d757c;
      fVar21 = DAT_005d757c;
      fVar22 = DAT_005d757c;
      fVar6 = DAT_005d757c;
      if (local_4 != 0) {
        iVar7 = *(int *)(param_1 + 4);
        puVar29 = (ushort *)
                  (*(int *)(param_1 + 0x14) + 6 +
                  (uint)*(ushort *)(*(int *)(param_1 + 0x10) + local_44 * 2) * 0xc);
        do {
          fVar2 = *(float *)(iVar7 + ((uint)puVar29[-1] * 3 + uVar28) * 4);
          fVar3 = *(float *)(iVar7 + ((uint)puVar29[-1] * 3 + uVar31) * 4);
          fVar4 = *(float *)(iVar7 + ((uint)*puVar29 * 3 + uVar28) * 4);
          fVar5 = *(float *)(iVar7 + ((uint)*puVar29 * 3 + uVar31) * 4);
          fVar9 = fVar4 - fVar2;
          fVar10 = fVar5 - fVar3;
          fVar20 = fVar2 * fVar2;
          fVar23 = fVar2 * fVar20;
          fVar11 = fVar3 * fVar3;
          fVar12 = fVar11 * fVar3;
          fVar13 = fVar4 * fVar4;
          fVar14 = fVar5 * fVar5 * fVar5;
          fVar24 = (fVar4 + fVar2) * fVar4 + fVar20;
          fVar26 = fVar24 * fVar4 + fVar23;
          fVar15 = (fVar5 + fVar3) * fVar5 + fVar11;
          fVar16 = fVar15 * fVar5 + fVar12;
          fVar25 = fVar4 * fVar2 + fVar4 * fVar2;
          fVar17 = fVar13 * DAT_005cc31c + fVar20 + fVar25;
          fVar25 = fVar20 * DAT_005cc31c + fVar13 + fVar25;
          fVar6 = (fVar4 + fVar2) * fVar10 + fVar6;
          fVar22 = fVar24 * fVar10 + fVar22;
          fVar21 = fVar26 * fVar10 + fVar21;
          fVar18 = (fVar26 * fVar4 + fVar23 * fVar2) * fVar10 + fVar18;
          local_50 = fVar15 * fVar9 + local_50;
          fVar19 = fVar16 * fVar9 + fVar19;
          local_34 = (fVar16 * fVar5 + fVar12 * fVar3) * fVar9 + local_34;
          local_54 = (fVar25 * fVar3 + fVar17 * fVar5) * fVar10 + local_54;
          local_60 = ((fVar23 * DAT_005cc35c + fVar25 * fVar4) * fVar3 +
                     (fVar17 * fVar2 + fVar13 * fVar4 * DAT_005cc35c) * fVar5) * fVar10 + local_60;
          fVar3 = fVar5 * fVar5 * fVar3;
          fVar11 = fVar11 * fVar5;
          puVar29 = puVar29 + 6;
          local_4 = local_4 + -1;
          local_5c = ((fVar3 * DAT_005cc31c + fVar11 + fVar11 + fVar14 * DAT_005cc35c + fVar12) *
                      fVar4 + (fVar3 + fVar3 + fVar11 * DAT_005cc31c + fVar12 * DAT_005cc35c +
                              fVar14) * fVar2) * fVar9 + local_5c;
        } while (local_4 != 0);
      }
      fVar22 = fVar22 * (float)DAT_005e58b0;
      fVar21 = fVar21 * (float)DAT_005e58a8;
      fVar18 = fVar18 * (float)DAT_005e3380;
      fVar19 = fVar19 * (float)DAT_005e5898;
      local_34 = local_34 * (float)DAT_005e5890;
      local_60 = local_60 * (float)DAT_005e5880;
      local_5c = local_5c * (float)DAT_005e5878;
      fVar23 = DAT_005cc320 / pfVar30[iVar27];
      fVar24 = fVar23 * fVar23;
      fVar25 = fVar24 * fVar23;
      fVar5 = fVar22 * pfVar30[uVar28] + local_50 * (float)DAT_005e58a0 * pfVar30[uVar31];
      fVar9 = fVar6 * (float)DAT_005ce1d8 * fVar8;
      fVar10 = pfVar30[uVar31] * pfVar30[uVar31];
      fVar11 = pfVar30[uVar28] * pfVar30[uVar28];
      fVar16 = pfVar30[uVar31] * pfVar30[uVar28] * local_54 * (float)DAT_005e5888;
      fVar16 = fVar16 + fVar16;
      fVar14 = fVar5 * DAT_005cc31c;
      fVar15 = (fVar16 + fVar11 * fVar21 + fVar10 * fVar19) * DAT_005cc31c;
      fVar20 = (fVar11 * pfVar30[uVar31] * local_60 + fVar10 * local_5c * pfVar30[uVar28]) *
               DAT_005cc31c;
      fVar6 = pfVar30[uVar28];
      fVar2 = pfVar30[uVar31];
      fVar3 = pfVar30[uVar31];
      fVar4 = pfVar30[uVar28];
      fVar12 = fVar21 * pfVar30[uVar28] + local_54 * (float)DAT_005e5888 * pfVar30[uVar31];
      fVar13 = pfVar30[uVar31] * pfVar30[uVar28] * local_60;
      if (uVar28 == 0) {
        fVar17 = fVar23 * fVar22;
      }
      else if (uVar31 == 0) {
        fVar17 = fVar23 * local_50 * (float)DAT_005e58a0;
      }
      else {
        fVar17 = -((fVar5 + fVar9) * fVar24);
      }
      local_2c = fVar17 * *pfVar30 + local_2c;
      param_2[uVar28] = fVar23 * fVar21 * pfVar30[uVar28] + param_2[uVar28];
      param_2[uVar31] = fVar23 * fVar19 * pfVar30[uVar31] + param_2[uVar31];
      param_2[iVar27] =
           ((fVar5 + fVar5 + fVar9) * fVar8 + fVar16 + fVar11 * fVar21 + fVar10 * fVar19) * fVar25 *
           pfVar30[iVar27] + param_2[iVar27];
      param_3[uVar28] = fVar23 * fVar18 * pfVar30[uVar28] + param_3[uVar28];
      param_3[uVar31] = fVar23 * local_34 * pfVar30[uVar31] + param_3[uVar31];
      param_3[iVar27] =
           -(fVar25 * fVar23 *
            (fVar2 * fVar10 * local_34 +
            fVar6 * fVar11 * fVar18 + fVar20 + (fVar15 + (fVar14 + fVar9) * fVar8) * fVar8)) *
           pfVar30[iVar27] + param_3[iVar27];
      param_4[uVar28] = fVar23 * local_60 * pfVar30[uVar28] + param_4[uVar28];
      local_30 = local_30 + 0xc;
      param_4[uVar31] =
           -((local_5c * fVar4 + fVar19 * fVar8 + local_34 * fVar3) * fVar24) * pfVar30[uVar31] +
           param_4[uVar31];
      param_4[iVar27] =
           (fVar10 * local_5c +
           fVar11 * fVar18 + fVar13 + fVar13 + (fVar22 * fVar8 + fVar12 + fVar12) * fVar8) * fVar25
           * pfVar30[iVar27] + param_4[iVar27];
      local_44 = local_44 + 1;
    } while (local_44 < *(ushort *)(param_1 + 0x1e));
  }
  *param_2 = *param_2 * DAT_005cc32c;
  param_2[1] = param_2[1] * DAT_005cc32c;
  param_2[2] = param_2[2] * DAT_005cc32c;
  *param_3 = *param_3 * DAT_005ccac8;
  param_3[1] = param_3[1] * DAT_005ccac8;
  param_3[2] = param_3[2] * DAT_005ccac8;
  *param_4 = *param_4 * DAT_005cc32c;
  param_4[1] = param_4[1] * DAT_005cc32c;
  param_4[2] = param_4[2] * DAT_005cc32c;
  return local_2c;
}
