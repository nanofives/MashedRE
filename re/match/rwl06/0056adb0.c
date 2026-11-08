// FLAGS: /O2 /GS /Ob1
/* MATCH-RWL06 0x0056adb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef long double float10;
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005cc568, 0x005cc568);
void FUN_0056aae0(int * p0, int * p1, int p2, undefined4 p3, undefined4 p4, int p5, undefined4 p6, undefined4 p7, int p8, undefined4 p9, undefined4 p10, int p11);
float10 FUN_0056ac40();
MGLOBAL(int, stack0x0000000c, 0x0000000c);
MGLOBAL(int, _DAT_005cc32c, 0x005cc32c);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005cc568, 0x005cc568);
// MATCH: 0x0056adb0 FUN_0056adb0
/* [C1 2026-05-19] Signature: `void FUN_0056adb0(int *param_1, int *param_2)`. */

void FUN_0056adb0(int *param_1,int *param_2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  int iVar11;
  undefined4 *puVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  undefined4 *puVar16;
  float *pfVar17;
  int *piVar18;
  float10 fVar19;
  int in_stack_00000094;
  int in_stack_000000a0;
  int in_stack_000000ac;
  undefined4 *in_stack_000000b8;
  int in_stack_000000bc;
  int in_stack_000000c4;
  undefined4 in_stack_00000100;
  undefined4 in_stack_00000104;
  undefined4 in_stack_00000108;
  uint in_stack_00000110;
  int in_stack_00000114;
  undefined4 auStack_148 [26];
  undefined4 uStack_e0;
  int *piStack_dc;
  int *piStack_d8;
  int local_d4;
  uint local_d0;
  int local_cc;
  int local_c8;
  int local_c4;
  int local_c0;
  int local_bc;
  int local_b8;
  int local_b4;
  int local_b0;
  int local_ac;
  int local_a8;
  undefined4 uStack_a4;
  float local_90;
  int local_88;
  int local_84;
  float local_80;
  float local_7c;
  uint local_78;
  uint local_74;
  undefined4 local_70;
  int local_6c;
  int local_68;
  int local_64;
  int local_60;
  float local_5c;
  uint local_58;
  float local_54;
  uint local_50;
  undefined4 uStack_4c;
  uint local_48;
  undefined4 uStack_44;
  int local_40;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  uint local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  
  iVar14 = *param_1;
  iVar15 = param_1[3];
  local_d0 = param_1[4];
  local_cc = param_1[5];
  local_bc = *param_2;
  local_b8 = param_2[1];
  local_b4 = param_2[2];
  local_b0 = param_2[3];
  local_ac = param_2[4];
  local_a8 = param_2[5];
  local_c4 = param_1[1];
  local_c0 = param_1[2];
  if (in_stack_000000bc != 0) {
    local_58 = iVar15 - (int)in_stack_000000b8;
    local_5c = (float)(iVar14 - (int)in_stack_000000b8);
    local_60 = in_stack_00000094 - (int)in_stack_000000b8;
    local_64 = local_bc - (int)in_stack_000000b8;
    local_68 = in_stack_000000a0 - (int)in_stack_000000b8;
    local_6c = local_b0 - (int)in_stack_000000b8;
    local_74 = in_stack_000000bc;
    puVar12 = in_stack_000000b8;
    do {
      pfVar17 = (float *)((in_stack_000000c4 - (int)in_stack_000000b8) + (int)puVar12);
      fVar1 = *pfVar17;
      fVar2 = pfVar17[1];
      fVar3 = pfVar17[2];
      fVar4 = pfVar17[3];
      uVar8 = puVar12[1];
      uVar9 = puVar12[2];
      uVar10 = puVar12[3];
      puVar16 = (undefined4 *)(local_58 + (int)puVar12);
      *puVar16 = *puVar12;
      puVar16[1] = uVar8;
      puVar16[2] = uVar9;
      puVar16[3] = uVar10;
      pfVar17 = (float *)((int)local_5c + (int)puVar12);
      *pfVar17 = fVar1;
      pfVar17[1] = fVar2;
      pfVar17[2] = fVar3;
      pfVar17[3] = fVar4;
      pfVar17 = (float *)(local_60 + (int)puVar12);
      fVar5 = pfVar17[1];
      fVar6 = pfVar17[2];
      fVar7 = pfVar17[3];
      piVar18 = (int *)(local_64 + (int)puVar12);
      *piVar18 = -(uint)(fVar1 != *pfVar17);
      piVar18[1] = -(uint)(fVar2 != fVar5);
      piVar18[2] = -(uint)(fVar3 != fVar6);
      piVar18[3] = -(uint)(fVar4 != fVar7);
      pfVar17 = (float *)(local_68 + (int)puVar12);
      fVar5 = pfVar17[1];
      fVar6 = pfVar17[2];
      fVar7 = pfVar17[3];
      piVar18 = (int *)(local_6c + (int)puVar12);
      puVar12 = puVar12 + 4;
      *piVar18 = -(uint)(fVar1 != *pfVar17);
      piVar18[1] = -(uint)(fVar2 != fVar5);
      piVar18[2] = -(uint)(fVar3 != fVar6);
      piVar18[3] = -(uint)(fVar4 != fVar7);
      local_74 = local_74 + -1;
    } while (local_74 != 0);
  }
  uStack_a4 = in_stack_00000108;
  piStack_dc = &local_88;
  piStack_d8 = &local_84;
  uStack_e0 = 0x56af69;
  local_d4 = iVar15;
  local_c8 = iVar14;
  local_40 = local_b0;
  local_3c = local_ac;
  local_38 = local_a8;
  local_34 = local_bc;
  local_30 = local_b8;
  local_2c = local_b4;
  local_28 = iVar15;
  local_24 = local_d0;
  local_20 = local_cc;
  local_1c = iVar14;
  local_18 = local_c4;
  local_14 = local_c0;
  fVar19 = (float10)FUN_0056ac40();
  local_5c = (float)fVar19;
  iVar13 = (local_84 + local_88 * 4) * 4;
  local_70 = *(undefined4 *)(iVar13 + iVar15);
  local_6c = *(undefined4 *)(iVar13 + iVar14);
  local_78 = 0;
  local_90 = 0.0;
  local_68 = *(undefined4 *)(iVar13 + in_stack_00000094);
  local_80 = 0.0;
  local_64 = *(undefined4 *)(iVar13 + in_stack_000000a0);
  local_60 = *(undefined4 *)(iVar13 + in_stack_000000ac);
  local_50 = local_24;
  uStack_4c = 0;
  local_7c = (float)fVar19;
  local_54 = (float)local_24;
  local_58 = local_24 + 1;
  local_74 = 0;
  if (in_stack_00000110 == 0) {
    param_1[6] = 0;
    return;
  }
  do {
    iVar14 = local_18;
    if (local_88 == -1) {
      param_1[6] = local_78;
      return;
    }
    uStack_a4 = in_stack_00000108;
    local_a8 = in_stack_00000104;
    local_ac = in_stack_00000100;
    local_b0 = local_60;
    local_b4 = local_6c;
    local_b8 = local_64;
    local_bc = local_68;
    local_c0 = local_70;
    puVar12 = (undefined4 *)&stack0x0000000c;
    puVar16 = auStack_148;
    for (iVar15 = 0x22; iVar11 = local_1c, iVar13 = local_28, iVar15 != 0; iVar15 = iVar15 + -1) {
      *puVar16 = *puVar12;
      puVar12 = puVar12 + 1;
      puVar16 = puVar16 + 1;
    }
    FUN_0056aae0(&local_88,&local_84,local_28,local_24,local_20,local_34,local_30,local_2c,local_40,
                 local_3c,local_38,local_1c,iVar14,local_14);
    uStack_a4 = in_stack_00000108;
    local_b0 = local_40;
    local_ac = local_3c;
    local_a8 = local_38;
    local_bc = local_34;
    local_b8 = local_30;
    local_b4 = local_2c;
    local_c8 = iVar11;
    local_c4 = local_18;
    local_c0 = local_14;
    local_d4 = iVar13;
    local_d0 = local_24;
    piStack_dc = &local_88;
    local_cc = local_20;
    piStack_d8 = &local_84;
    uStack_e0 = 0x56b16d;
    fVar19 = (float10)FUN_0056ac40();
    fVar1 = (float)fVar19;
    iVar14 = (local_84 + local_88 * 4) * 4;
    local_70 = *(undefined4 *)(iVar14 + iVar13);
    local_6c = *(undefined4 *)(iVar14 + iVar11);
    local_68 = *(undefined4 *)(iVar14 + in_stack_00000094);
    local_64 = *(undefined4 *)(iVar14 + in_stack_000000a0);
    local_60 = *(undefined4 *)(iVar14 + in_stack_000000ac);
    if (in_stack_00000114 != 0) {
      local_74 = local_74 + 1;
      if (local_58 < local_74) {
        local_74 = 0;
LAB_0056b1e6:
        if (local_90 < local_5c) {
          local_48 = local_58;
          uStack_44 = 0;
          local_5c = local_90;
          local_90 = (local_90 + local_90) * _DAT_005cc32c;
          fVar2 = local_80 - local_7c;
          local_80 = fVar1;
          local_7c = fVar1;
          if ((_DAT_005cc320 / local_90) * fVar2 * _DAT_005cc568 + _DAT_005cc320 < (float)local_58)
          {
            local_50 = (uint)ROUND((_DAT_005cc320 / local_90) * fVar2 * local_54);
            local_58 = local_50 + 1;
          }
        }
        else {
          local_88 = -1;
        }
      }
      else if (local_74 == 0) goto LAB_0056b1e6;
      local_90 = (fVar1 - local_90) * _DAT_005cc32c + local_90;
      if (local_80 < fVar1) {
        local_80 = fVar1;
      }
      if (fVar1 < local_7c) {
        local_7c = fVar1;
      }
    }
    local_78 = local_78 + 1;
    if (in_stack_00000110 <= local_78) {
      param_1[6] = local_78;
      return;
    }
  } while( true );
}
