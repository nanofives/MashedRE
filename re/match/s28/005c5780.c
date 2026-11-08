// FLAGS: /O2 /Ob1 /G5
// best 13.6 (regs 20.1, ops 48.4), compile-only form (4178 B, 1339 vs 1223 ins). Raw Ghidra text with NAN -> -1.0f placeholder and the Ghidra float/ushort* confusion on local_98/local_94 turned into int locals plus casts for both the DDK and C++ builds. Those int/float edits change semantics (Ghidra holds integers in float-typed locals) so the body is NOT port-ready: needs a rewrite from the 1224-instruction listing. Class (b).
/* MATCH-S28 0x005c5780: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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
typedef long double float10;
#ifndef __cplusplus
#define true 1
#endif
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
#define SUB42(a,n) ((ushort)((uint)(a)>>((n)*8)))
MGLOBAL(undefined1, DAT_005e5050, 0x005e5050);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005cc33c, 0x005cc33c);
float10 FUN_00566b40(float * p0, float * p1, float * p2, float p3);
uint FUN_00566ca0(float * p0, float * p1, float * p2, float * p3, float p4);
uint FUN_00566d50(float * p0, float * p1, float * p2, float * p3, float p4);
uint FUN_00566ea0(float * p0, float * p1, float * p2, float * p3, float * p4, float p5, int p6, int p7);
MGLOBAL(int, _DAT_005e5050, 0x005e5050);

MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005cc33c, 0x005cc33c);
// MATCH: 0x005c5780 FUN_005c5780
/* [C1 2026-05-18] Stage 1 (005c57e2..005c5828): walk all faces (count at +0x1e), find face whose
   plane-distance is� */

undefined4 FUN_005c5780(int param_1,float param_2,float *param_3,float *param_4,int param_5)

{
  float *pfVar1;
  float *pfVar2;
  ushort uVar3;
  int iVar4;
  bool bVar5;
  ushort uVar6;
  uint uVar7;
  float fVar8;
  float fVar9;
  float *pfVar10;
  float *pfVar11;
  float fVar12;
  int iVar13;
  float fVar14;
  float fVar15;
  int local_98;
  int local_94;
  float local_90;
  float local_8c;
  float local_88;
  float local_84;
  float local_80;
  float local_7c;
  float local_78;
  float local_74;
  float local_70;
  float local_6c;
  float local_68;
  float local_64;
  float local_60;
  float local_5c;
  float local_58;
  float local_54;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_20;
  float local_1c;
  float local_18;
  int local_14;
  float local_10;
  float local_c;
  float local_8;
  float *local_4;
  
  local_90 = *param_3;
  local_8c = param_3[1];
  local_10 = *param_4;
  local_88 = param_3[2];
  local_60 = local_10 - local_90;
  local_c = param_4[1];
  local_5c = local_c - local_8c;
  local_8 = param_4[2];
  fVar12 = 0.0;
  local_58 = local_8 - local_88;
  local_40 = 0.0;
  local_14 = CONCAT22((short)((uint)local_c >> 0x10),
                      (*(ushort *)(param_1 + 0x20) >> 1) + *(short *)(param_1 + 0x1c)) +
             CONCAT22((short)((uint)local_8 >> 0x10),*(short *)(param_1 + 0x1e));
  local_98 = 0;
  if (*(short *)(param_1 + 0x1e) != 0) {
    local_94 = *(int *)(param_1 + 0x10);
    iVar13 = *(int *)(param_1 + 4);
    pfVar10 = *(float **)(param_1 + 0xc);
    fVar14 = _DAT_005e5050;
    do {
      pfVar11 = (float *)(iVar13 + (uint)*(ushort *)
                                          (*(int *)(param_1 + 0x14) + 4 + (uint)*(ushort *)local_94 * 0xc) *
                                   0xc);
      local_54 = (pfVar10[2] * local_88 + *pfVar10 * local_90 + pfVar10[1] * local_8c) -
                 (pfVar11[2] * pfVar10[2] + *pfVar10 * *pfVar11 + pfVar11[1] * pfVar10[1]);
      if (fVar14 < local_54) {
        local_40 = local_98;
        fVar14 = local_54;
      }
      local_98 = local_98 + 1;
      local_94 = local_94 + 2;
      pfVar10 = pfVar10 + 3;
    } while (SUB42(local_98,0) < *(ushort *)(param_1 + 0x1e));
    if (fVar14 < DAT_005d757c == (fVar14 == DAT_005d757c)) {
      uVar7 = (uint)local_40 & 0xffff;
      local_98 = -1;
      uVar6 = *(ushort *)(*(int *)(param_1 + 0x10) + uVar7 * 2);
      local_94 = (int)(uint)uVar6;
      uVar3 = *(ushort *)(*(int *)(param_1 + 0x10) + 2 + uVar7 * 2);
      fVar12 = local_40;
      if (uVar6 < uVar3) {
        pfVar10 = (float *)(*(int *)(param_1 + 0xc) + uVar7 * 0xc);
        uVar7 = (uint)uVar3 - (int)local_94 & 0xffff;
        pfVar11 = (float *)(*(int *)(param_1 + 0x14) + (int)local_94 * 0xc);
        fVar14 = _DAT_005e5050;
        do {
          pfVar1 = (float *)(iVar13 + (uint)*(ushort *)((int)pfVar11 + 6) * 0xc);
          fVar15 = *pfVar1 - *(float *)(iVar13 + (uint)*(ushort *)(pfVar11 + 1) * 0xc);
          pfVar2 = (float *)(iVar13 + (uint)*(ushort *)(pfVar11 + 1) * 0xc);
          local_20 = pfVar10[2] * (pfVar1[1] - pfVar2[1]) - pfVar10[1] * (pfVar1[2] - pfVar2[2]);
          local_1c = *pfVar10 * (pfVar1[2] - pfVar2[2]) - pfVar10[2] * fVar15;
          fVar15 = (pfVar10[1] * fVar15 - *pfVar10 * (pfVar1[1] - pfVar2[1])) * *pfVar11;
          local_6c = (fVar15 * local_88 +
                     local_20 * *pfVar11 * local_90 + local_1c * *pfVar11 * local_8c) -
                     (pfVar2[2] * fVar15 +
                     *pfVar2 * local_20 * *pfVar11 + pfVar2[1] * local_1c * *pfVar11);
          if (fVar14 < local_6c) {
            local_98 = local_94;
            fVar14 = local_6c;
          }
          pfVar11 = pfVar11 + 3;
          local_94 = local_94 + 1;
          uVar7 = uVar7 - 1;
        } while (uVar7 != 0);
        if (fVar14 < DAT_005d757c == (fVar14 == DAT_005d757c)) {
          fVar12 = (float)(uint)*(ushort *)(*(int *)(param_1 + 0x14) + 6 + (int)local_98 * 0xc);
          pfVar10 = (float *)(*(int *)(param_1 + 0x14) + (int)local_98 * 0xc);
          fVar14 = (float)(uint)*(ushort *)(pfVar10 + 1);
          pfVar11 = (float *)(iVar13 + (int)fVar12 * 0xc);
          pfVar1 = (float *)(iVar13 + (int)fVar14 * 0xc);
          local_6c = (local_88 - pfVar1[2]) * (pfVar11[2] - pfVar1[2]) +
                     (local_90 - *pfVar1) * (*pfVar11 - *(float *)(iVar13 + (int)fVar14 * 0xc)) +
                     (local_8c - pfVar1[1]) * (pfVar11[1] - pfVar1[1]);
          if (DAT_005d757c <= local_6c) {
            fVar14 = *pfVar10;
            if (fVar14 * fVar14 * local_6c <= _DAT_005cc320) {
              uVar6 = 1;
              fVar12 = local_98;
            }
            else {
              uVar6 = 0;
            }
          }
          else {
            uVar6 = 0;
            fVar12 = fVar14;
          }
        }
        else {
          uVar6 = 2;
        }
      }
      else {
        uVar6 = 2;
      }
      goto LAB_005c5ae9;
    }
  }
  uVar6 = 3;
LAB_005c5ae9:
  *(undefined4 *)(param_5 + 0x1c) = 0;
  while ((uVar6 & 2) == 0) {
    if (uVar6 == 0) {
      fVar14 = (float)((uint)fVar12 & 0xffff);
      local_6c = fVar14;
      local_94 = (int)
                 FUN_00566d50(&local_68,&local_90,&local_60,
                              (float *)(*(int *)(param_1 + 4) + (int)fVar14 * 0xc),param_2);
      if ((int)local_94 < 0) {
        return 0;
      }
      if (param_2 < DAT_005d757c == (param_2 == DAT_005d757c)) {
        if ((0 < (int)local_94) && (local_68 == DAT_005d757c)) {
          iVar13 = *(int *)(param_1 + 4) + ((uint)fVar12 & 0xffff) * 0xc;
          local_78 = local_90 - *(float *)(*(int *)(param_1 + 4) + ((uint)fVar12 & 0xffff) * 0xc);
          local_74 = local_8c - *(float *)(iVar13 + 4);
          local_70 = local_88 - *(float *)(iVar13 + 8);
          FUN_00566b40(&local_78,&local_78,(float *)iVar13,0x3f800000);
          goto LAB_005c6793;
        }
      }
      else {
        local_94 = 0;
      }
      local_54 = (-1.0f);
      uVar6 = *(ushort *)(*(int *)(param_1 + 8) + (int)fVar14 * 2);
      if (uVar6 < *(ushort *)(*(int *)(param_1 + 8) + 2 + (int)fVar14 * 2)) {
        iVar13 = *(int *)(param_1 + 4);
        do {
          fVar8 = (float)(uint)*(ushort *)(*(int *)(param_1 + 0x18) + (uint)uVar6 * 2);
          pfVar10 = (float *)(*(int *)(param_1 + 0x14) + (int)fVar8 * 0xc);
          pfVar11 = (float *)(iVar13 + (uint)*(ushort *)((int)pfVar10 + 6) * 0xc);
          pfVar1 = (float *)(iVar13 + (uint)*(ushort *)(pfVar10 + 1) * 0xc);
          local_18 = pfVar11[2] - pfVar1[2];
          local_20 = (*pfVar11 - *(float *)(iVar13 + (uint)*(ushort *)(pfVar10 + 1) * 0xc)) *
                     *pfVar10;
          fVar14 = (pfVar11[1] - pfVar1[1]) * *pfVar10;
          fVar15 = local_18 * *pfVar10;
          local_30 = fVar15 * local_58 + local_20 * local_60 + fVar14 * local_5c;
          if (((DAT_005d757c < local_30) &&
              (local_34 = (pfVar1[2] * fVar15 + *pfVar1 * local_20 + pfVar1[1] * fVar14) -
                          (fVar15 * local_88 + local_20 * local_90 + fVar14 * local_8c),
              local_34 < local_30 != (local_34 == local_30))) &&
             (((int)local_94 < 1 || (local_64 * local_34 <= local_30 * local_68)))) {
            local_94 = 1;
            local_68 = local_34;
            local_64 = local_30;
            local_54 = fVar8;
          }
          uVar6 = uVar6 + 1;
        } while (uVar6 < *(ushort *)(*(int *)(param_1 + 8) + 2 + (int)local_6c * 2));
      }
      if ((int)local_94 < 1) {
        return 0;
      }
      local_94 = *(int *)&local_54;
      if ((int)local_54 < 0) {
        local_68 = local_68 / local_64;
        *(float *)(param_5 + 0x1c) = local_68 + *(float *)(param_5 + 0x1c);
        local_90 = local_60 * local_68 + local_90;
        pfVar10 = (float *)(*(int *)(param_1 + 4) + ((uint)fVar12 & 0xffff) * 0xc);
        local_8c = local_5c * local_68 + local_8c;
        local_88 = local_68 * local_58 + local_88;
        local_70 = _DAT_005cc320 / param_2;
        local_78 = (local_90 - *pfVar10) * local_70;
        local_74 = (local_8c - pfVar10[1]) * local_70;
        local_70 = (local_88 - pfVar10[2]) * local_70;
        goto LAB_005c6793;
      }
LAB_005c6431:
      uVar6 = 1;
    }
    else {
      pfVar10 = (float *)(*(int *)(param_1 + 0x14) + ((uint)fVar12 & 0xffff) * 0xc);
      iVar13 = *(int *)(param_1 + 4);
      local_84 = (*(float *)(iVar13 + (uint)*(ushort *)((int)pfVar10 + 6) * 0xc) -
                 *(float *)(iVar13 + (uint)*(ushort *)(pfVar10 + 1) * 0xc)) * *pfVar10;
      local_80 = (*(float *)(iVar13 + 4 + (uint)*(ushort *)((int)pfVar10 + 6) * 0xc) -
                 *(float *)(iVar13 + 4 + (uint)*(ushort *)(pfVar10 + 1) * 0xc)) * *pfVar10;
      local_7c = (*(float *)(iVar13 + 8 + (uint)*(ushort *)((int)pfVar10 + 6) * 0xc) -
                 *(float *)(iVar13 + 8 + (uint)*(ushort *)(pfVar10 + 1) * 0xc)) * *pfVar10;
      local_54 = (float)FUN_00566ea0(&local_68,&local_90,&local_60,
                                     (float *)(iVar13 + (uint)*(ushort *)(pfVar10 + 1) * 0xc),&local_84,param_2
                                     ,0,0);
      if ((int)local_54 < 0) {
        return 0;
      }
      if (param_2 < DAT_005d757c == (param_2 == DAT_005d757c)) {
        if ((0 < (int)local_54) && (local_68 == DAT_005d757c)) {
          iVar13 = *(int *)(param_1 + 4);
          local_78 = local_90 - *(float *)(iVar13 + (uint)*(ushort *)(pfVar10 + 1) * 0xc);
          local_74 = local_8c - *(float *)(iVar13 + 4 + (uint)*(ushort *)(pfVar10 + 1) * 0xc);
          local_70 = local_88 - *(float *)(iVar13 + 8 + (uint)*(ushort *)(pfVar10 + 1) * 0xc);
          fVar14 = local_80 * local_74;
          fVar12 = local_7c * local_70;
          local_78 = -(local_84 * local_78 + fVar14 + fVar12) * local_84 + local_78;
          local_74 = -(local_84 * local_78 + fVar14 + fVar12) * local_80 + local_74;
          local_70 = -(local_80 * local_74 + local_84 * local_78 + fVar12) * local_7c + local_70;
          FUN_00566b40(&local_78,&local_78,&local_84,0);
          goto LAB_005c6793;
        }
      }
      else {
        local_54 = 0.0;
      }
      iVar13 = *(int *)(param_1 + 4);
      uVar6 = 0;
      local_4c = local_7c * local_58 + local_84 * local_60 + local_80 * local_5c;
      if (DAT_005d757c <= local_4c) {
        local_94 = (uint)*(ushort *)((int)pfVar10 + 6);
        pfVar11 = (float *)(iVar13 + (int)local_94 * 0xc);
        local_50 = pfVar11[2] * local_7c +
                   *pfVar11 * local_84 + *(float *)(iVar13 + 4 + (int)local_94 * 0xc) * local_80;
        fVar14 = local_84 * local_90 + local_80 * local_8c;
        fVar12 = local_88;
      }
      else {
        local_4c = local_4c * _DAT_005cc33c;
        local_94 = (uint)*(ushort *)(pfVar10 + 1);
        pfVar11 = (float *)(iVar13 + (int)local_94 * 0xc);
        local_50 = local_7c * local_88 + local_84 * local_90 + local_80 * local_8c;
        fVar14 = *pfVar11 * local_84 + pfVar11[1] * local_80;
        fVar12 = pfVar11[2];
      }
      local_50 = local_50 - (fVar12 * local_7c + fVar14);
      bVar5 = local_50 < local_4c != (local_50 == local_4c);
      iVar4 = *(int *)(param_1 + 0xc);
      fVar9 = (float)(uint)*(ushort *)((int)pfVar10 + 10);
      pfVar11 = (float *)(iVar4 + (int)fVar9 * 0xc);
      fVar12 = *(float *)(iVar4 + 8 + (int)fVar9 * 0xc) * local_80 - pfVar11[1] * local_7c;
      fVar14 = *pfVar11 * local_7c - pfVar11[2] * local_84;
      fVar8 = pfVar11[1] * local_84 - *pfVar11 * local_80;
      fVar15 = -(fVar8 * local_58 + fVar12 * local_60 + fVar14 * local_5c);
      if (DAT_005d757c < fVar15) {
        pfVar11 = (float *)(iVar13 + (uint)*(ushort *)(pfVar10 + 1) * 0xc);
        local_48 = (fVar8 * local_88 + fVar12 * local_90 + fVar14 * local_8c) -
                   (pfVar11[2] * fVar8 + *pfVar11 * fVar12 + pfVar11[1] * fVar14);
        if ((local_48 < fVar15 != (local_48 == fVar15)) &&
           ((!bVar5 || (local_48 * local_4c <= fVar15 * local_50)))) {
          uVar6 = 2;
          bVar5 = true;
          local_94 = *(int *)&fVar9;
          local_50 = local_48;
          local_4c = fVar15;
        }
      }
      fVar8 = (float)(uint)*(ushort *)(pfVar10 + 2);
      pfVar11 = (float *)(iVar4 + (int)fVar8 * 0xc);
      fVar12 = *(float *)(iVar4 + 4 + (int)fVar8 * 0xc) * local_7c - pfVar11[2] * local_80;
      fVar14 = pfVar11[2] * local_84 - *pfVar11 * local_7c;
      fVar15 = *pfVar11 * local_80 - pfVar11[1] * local_84;
      local_44 = -(fVar15 * local_58 + fVar12 * local_60 + fVar14 * local_5c);
      if (DAT_005d757c < local_44) {
        pfVar11 = (float *)(iVar13 + (uint)*(ushort *)(pfVar10 + 1) * 0xc);
        local_48 = (fVar15 * local_88 + fVar12 * local_90 + fVar14 * local_8c) -
                   (pfVar11[2] * fVar15 + *pfVar11 * fVar12 + pfVar11[1] * fVar14);
        if ((local_48 < local_44 != (local_48 == local_44)) &&
           ((!bVar5 || (local_48 * local_4c <= local_44 * local_50)))) {
          uVar6 = 2;
          bVar5 = true;
          local_94 = *(int *)&fVar8;
          local_50 = local_48;
          local_4c = local_44;
        }
      }
      if ((0 < (int)local_54) && ((!bVar5 || (local_4c * local_68 <= local_50 * local_64)))) {
        local_68 = local_68 / local_64;
        *(float *)(param_5 + 0x1c) = local_68 + *(float *)(param_5 + 0x1c);
        iVar13 = *(int *)(param_1 + 4);
        local_90 = local_60 * local_68 + local_90;
        local_8c = local_5c * local_68 + local_8c;
        local_88 = local_68 * local_58 + local_88;
        local_78 = local_90 - *(float *)(iVar13 + (uint)*(ushort *)(pfVar10 + 1) * 0xc);
        fVar12 = local_8c - *(float *)(iVar13 + 4 + (uint)*(ushort *)(pfVar10 + 1) * 0xc);
        fVar14 = local_88 - *(float *)(iVar13 + 8 + (uint)*(ushort *)(pfVar10 + 1) * 0xc);
        fVar8 = local_80 * fVar12;
        fVar15 = local_7c * fVar14;
        local_78 = -(local_84 * local_78 + fVar8 + fVar15) * local_84 + local_78;
        local_84 = local_84 * local_78;
        fVar12 = -(local_84 + fVar8 + fVar15) * local_80 + fVar12;
        local_70 = _DAT_005cc320 / param_2;
        local_78 = local_78 * local_70;
        local_74 = fVar12 * local_70;
        local_70 = (-(local_80 * fVar12 + local_84 + fVar15) * local_7c + fVar14) * local_70;
        goto LAB_005c6793;
      }
      if (!bVar5) {
        return 0;
      }
      local_68 = local_50;
      local_64 = local_4c;
    }
    fVar12 = local_68 / local_64;
    local_14 = local_14 + 0xffff;
    *(float *)(param_5 + 0x1c) = fVar12 + *(float *)(param_5 + 0x1c);
    local_90 = local_60 * fVar12 + local_90;
    local_8c = local_5c * fVar12 + local_8c;
    local_88 = fVar12 * local_58 + local_88;
    local_60 = local_10 - local_90;
    local_5c = local_c - local_8c;
    local_58 = local_8 - local_88;
    fVar12 = *(float *)&local_94;
    if ((short)local_14 == 0) {
      return 0;
    }
  }
  fVar14 = local_40;
  if (uVar6 == 2) {
    fVar15 = (float)((uint)fVar12 & 0xffff);
    local_94 = -1;
    iVar13 = (int)fVar15 * 0xc;
    pfVar10 = (float *)(*(int *)(param_1 + 4) +
                       (uint)*(ushort *)
                              (*(int *)(param_1 + 0x14) + 4 +
                              (uint)*(ushort *)(*(int *)(param_1 + 0x10) + (int)fVar15 * 2) * 0xc) *
                       0xc);
    pfVar11 = (float *)(*(int *)(param_1 + 0xc) + iVar13);
    local_6c = fVar15;
    local_54 = (float)FUN_00566ca0(&local_68,&local_90,&local_60,pfVar11,
                                   pfVar11[2] * pfVar10[2] +
                                   *pfVar11 * *pfVar10 +
                                   *(float *)(*(int *)(param_1 + 0xc) + 4 + iVar13) * pfVar10[1] +
                                   param_2);
    if ((int)local_54 < 0) {
      return 0;
    }
    if (((int)local_54 < 1) || (fVar14 = fVar12, local_68 != DAT_005d757c)) {
      uVar6 = *(ushort *)(*(int *)(param_1 + 0x10) + (int)fVar15 * 2);
      if (uVar6 < *(ushort *)(*(int *)(param_1 + 0x10) + 2 + (int)fVar15 * 2)) {
        pfVar10 = (float *)(*(int *)(param_1 + 0xc) + iVar13);
        do {
          pfVar11 = (float *)(*(int *)(param_1 + 0x14) + (uint)uVar6 * 0xc);
          local_4 = (float *)(*(int *)(param_1 + 4) + (uint)*(ushort *)((int)pfVar11 + 6) * 0xc);
          pfVar1 = (float *)(*(int *)(param_1 + 4) + (uint)*(ushort *)(pfVar11 + 1) * 0xc);
          local_2c = (pfVar10[2] * (local_4[1] - pfVar1[1]) - pfVar10[1] * (local_4[2] - pfVar1[2]))
                     * *pfVar11;
          local_28 = (*pfVar10 * (local_4[2] - pfVar1[2]) - pfVar10[2] * (*local_4 - *pfVar1)) *
                     *pfVar11;
          fVar14 = (pfVar10[1] * (*local_4 - *pfVar1) - *pfVar10 * (local_4[1] - pfVar1[1])) *
                   *pfVar11;
          local_38 = fVar14 * local_58 + local_2c * local_60 + local_28 * local_5c;
          if (((DAT_005d757c < local_38) &&
              (local_3c = (pfVar1[2] * fVar14 + pfVar1[1] * local_28 + *pfVar1 * local_2c) -
                          (fVar14 * local_88 + local_2c * local_90 + local_28 * local_8c),
              local_3c < local_38 != (local_3c == local_38))) &&
             (local_3c * local_64 < local_38 * local_68)) {
            local_54 = 1.4013e-45;
            local_94 = (int)(uint)uVar6;
            local_68 = local_3c;
            local_64 = local_38;
          }
          uVar6 = uVar6 + 1;
        } while (uVar6 < *(ushort *)(*(int *)(param_1 + 0x10) + 2 + (int)local_6c * 2));
      }
      if ((int)local_54 < 1) {
        return 0;
      }
      if ((int)local_94 < 0) {
        local_68 = local_68 / local_64;
        iVar13 = *(int *)(param_1 + 0xc) + ((uint)fVar12 & 0xffff) * 0xc;
        local_78 = *(float *)(*(int *)(param_1 + 0xc) + ((uint)fVar12 & 0xffff) * 0xc);
        local_74 = *(float *)(iVar13 + 4);
        local_70 = *(float *)(iVar13 + 8);
        *(float *)(param_5 + 0x1c) = local_68 + *(float *)(param_5 + 0x1c);
        local_90 = local_60 * local_68 + local_90;
        local_8c = local_5c * local_68 + local_8c;
        local_88 = local_68 * local_58 + local_88;
        goto LAB_005c6793;
      }
      goto LAB_005c6431;
    }
  }
  iVar13 = *(int *)(param_1 + 0xc) + ((uint)fVar14 & 0xffff) * 0xc;
  local_78 = *(float *)(*(int *)(param_1 + 0xc) + ((uint)fVar14 & 0xffff) * 0xc);
  local_74 = *(float *)(iVar13 + 4);
  local_70 = *(float *)(iVar13 + 8);
LAB_005c6793:
  *(float *)(param_5 + 4) = local_90;
  *(float *)(param_5 + 8) = local_8c;
  *(float *)(param_5 + 0x10) = local_78;
  *(float *)(param_5 + 0xc) = local_88;
  *(float *)(param_5 + 0x14) = local_74;
  *(float *)(param_5 + 0x18) = local_70;
  return 1;
}
