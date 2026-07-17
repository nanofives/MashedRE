
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] **Massive 3537-byte islanded-solver step.** Receives 13 parameters including
   arrays for: position+force buffer… */

void FUN_00560260(int *param_1,int *param_2,undefined4 *param_3,undefined4 *param_4,
                 undefined4 *param_5,undefined4 param_6,undefined4 param_7,int param_8,
                 undefined4 *param_9,undefined4 param_10,code *param_11,undefined4 param_12,
                 code *param_13)

{
  undefined4 *puVar1;
  int iVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  undefined4 uVar6;
  int iVar7;
  int *piVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  int *piVar12;
  float *pfVar13;
  undefined4 *puVar14;
  int iVar15;
  float *pfVar16;
  int iVar17;
  float *pfVar18;
  float *pfVar19;
  uint uVar20;
  undefined4 *puVar21;
  undefined4 *puVar22;
  undefined4 auStack_150 [11];
  undefined4 uStack_124;
  undefined4 uStack_120;
  undefined4 uStack_11c;
  undefined4 uStack_118;
  undefined4 auStack_114 [19];
  undefined4 uStack_c8;
  undefined4 uStack_c4;
  undefined4 uStack_c0;
  undefined4 uStack_bc;
  undefined4 uStack_b8;
  undefined4 uStack_b4;
  undefined4 uStack_b0;
  undefined4 uStack_ac;
  undefined4 uStack_a8;
  undefined4 uStack_a4;
  undefined4 uStack_a0;
  int iStack_9c;
  int iStack_98;
  undefined4 *puStack_94;
  undefined4 *puStack_90;
  undefined4 *puStack_8c;
  int aiStack_88 [2];
  undefined4 *puStack_80;
  undefined4 *puStack_7c;
  undefined4 *puStack_78;
  int iStack_74;
  int iStack_70;
  int iStack_6c;
  int iStack_68;
  int iStack_64;
  int iStack_60;
  undefined4 *puStack_5c;
  undefined4 *puStack_58;
  float *pfStack_54;
  float *pfStack_50;
  int local_38;
  uint local_34;
  int local_2c;
  int local_28;
  int local_24;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  float local_8;
  float local_4;

  puVar14 = param_3;
  piVar8 = param_1;
  iVar17 = 0;
  local_24 = 0;
  iVar15 = *(int *)(param_8 + 4);
  if (iVar15 != 0) {
    piVar12 = (int *)(*(int *)(param_8 + 0x10) + 0x14);
    do {
      if ((*(byte *)(piVar12 + 1) & 3) == 0) {
        iVar17 = iVar17 + *piVar12;
      }
      piVar12 = piVar12 + 10;
      iVar15 = iVar15 + -1;
      local_24 = iVar17;
    } while (iVar15 != 0);
  }
  pfStack_50 = (float *)param_4;
  pfStack_54 = (float *)param_3;
  puStack_58 = (undefined4 *)0x56029f;
  FUN_00567c00();
  uVar6 = *param_9;
  param_1[0x17] = 0;
  param_2[1] = 0;
  local_2c = 0;
  if (*(int *)(param_8 + 4) != 0) {
    local_28 = 0;
    do {
      piVar12 = (int *)(*(int *)(param_8 + 0x10) + local_28);
      if ((*(byte *)(piVar12 + 6) & 3) == 0) {
        param_1 = (int *)0x0;
        if (piVar12[3] != 0) {
          do {
            local_34 = 0;
            iVar15 = *(int *)(*piVar12 + (int)param_1 * 4);
            if (*(short *)(iVar15 + 0xc) != 0) {
              local_38 = 0;
              do {
                iVar17 = *(int *)(local_38 + 4 + *(int *)(iVar15 + 0x10));
                iVar7 = *(int *)(iVar17 + 4);
                pfVar13 = (float *)(piVar8[0xc] + iVar7 * 0x20);
                pfVar18 = (float *)(piVar8[2] + iVar7 * 0x20);
                pfVar16 = (float *)(iVar7 * 0x30 + piVar8[8]);
                *(short *)(iVar17 + 0x20) = (short)piVar8[0x17];
                *(int *)(piVar8[0x16] + piVar8[0x17] * 4) = iVar7;
                piVar8[0x17] = piVar8[0x17] + 1;
                *pfVar13 = pfVar16[3] * (float)param_9[0x13] + *pfVar13;
                pfVar13[1] = pfVar16[3] * (float)param_9[0x14] + pfVar13[1];
                pfVar13[2] = pfVar16[3] * (float)param_9[0x15] + pfVar13[2];
                *pfVar13 = -(pfVar16[3] * *(float *)(iVar17 + 0x14)) * *pfVar18 + *pfVar13;
                pfVar13[1] = -(pfVar16[3] * *(float *)(iVar17 + 0x14)) * pfVar18[1] + pfVar13[1];
                pfVar13[2] = -(pfVar16[3] * *(float *)(iVar17 + 0x14)) * pfVar18[2] + pfVar13[2];
                if ((*(byte *)(piVar8[6] + iVar7 * 4) & 2) == 0) {
                  pfVar13[4] = -(pfVar16[0xb] * *(float *)(iVar17 + 0x18)) * pfVar18[4] + pfVar13[4]
                  ;
                  pfVar13[5] = -(pfVar16[0xb] * *(float *)(iVar17 + 0x18)) * pfVar18[5] + pfVar13[5]
                  ;
                  pfVar13[6] = -(pfVar16[0xb] * *(float *)(iVar17 + 0x18)) * pfVar18[6] + pfVar13[6]
                  ;
                }
                else {
                  pfVar19 = (float *)(iVar7 * 0x40 + *piVar8);
                  fVar3 = pfVar19[2] * pfVar18[6] + *pfVar19 * pfVar18[4] + pfVar19[1] * pfVar18[5];
                  fVar4 = pfVar18[6] * pfVar19[6] +
                          pfVar18[4] * pfVar19[4] + pfVar18[5] * pfVar19[5];
                  local_10 = pfVar18[6] * pfVar19[10] +
                             pfVar19[8] * pfVar18[4] + pfVar18[5] * pfVar19[9];
                  local_4 = pfVar16[2] * fVar3;
                  local_c = pfVar16[4] * fVar4 + fVar3 * *pfVar16;
                  local_8 = pfVar16[5] * fVar4 + pfVar16[1] * fVar3;
                  fVar11 = pfVar16[8] * local_10 + local_c;
                  fVar10 = pfVar16[9] * local_10 + local_8;
                  fVar9 = pfVar16[10] * local_10 + pfVar16[6] * fVar4 + local_4;
                  local_18 = fVar11 * *pfVar19 + fVar10 * pfVar19[4] + fVar9 * pfVar19[8];
                  local_14 = fVar11 * pfVar19[1] + fVar9 * pfVar19[9] + fVar10 * pfVar19[5];
                  fVar3 = pfVar19[10];
                  fVar4 = pfVar19[2];
                  fVar5 = pfVar19[6];
                  pfVar13[4] = -*(float *)(iVar17 + 0x18) * local_18 + pfVar13[4];
                  pfVar13[5] = -*(float *)(iVar17 + 0x18) * local_14 + pfVar13[5];
                  pfVar13[6] = -*(float *)(iVar17 + 0x18) *
                               (fVar10 * fVar5 + fVar11 * fVar4 + fVar9 * fVar3) + pfVar13[6];
                }
                local_34 = local_34 + 1;
                local_38 = local_38 + 0xc;
              } while (local_34 < *(ushort *)(iVar15 + 0xc));
            }
            param_1 = (int *)((int)param_1 + 1);
          } while (param_1 < (uint)piVar12[3]);
        }
        uVar20 = 0;
        pfStack_50 = (float *)piVar12[5];
        puStack_58 = (undefined4 *)0x560556;
        pfStack_54 = (float *)param_3;
        FUN_0056efc0();
        if (piVar12[4] != 0) {
          do {
            *(int *)(param_4[0xf] + param_4[0x10] * 4) = *(int *)(piVar12[2] + uVar20 * 4) + 0xc;
            param_4[0x10] = param_4[0x10] + 1;
            puStack_58 = *(undefined4 **)(piVar12[2] + uVar20 * 4);
            pfStack_54 = (float *)0x0;
            iStack_60 = 0x560595;
            puStack_5c = param_3;
            pfStack_50 = (float *)uVar6;
            FUN_00570090();
            uVar20 = uVar20 + 1;
          } while (uVar20 < (uint)piVar12[4]);
        }
        param_1 = (int *)0x0;
        if (piVar12[3] != 0) {
          do {
            uVar20 = 0;
            iVar15 = *(int *)(*piVar12 + (int)param_1 * 4);
            if (*(short *)(iVar15 + 0x14) != 0) {
              do {
                *(int *)(param_4[0xf] + param_4[0x10] * 4) =
                     *(int *)(*(int *)(iVar15 + 0x18) + uVar20 * 4) + 0xc;
                param_4[0x10] = param_4[0x10] + 1;
                puStack_58 = *(undefined4 **)(*(int *)(iVar15 + 0x18) + uVar20 * 4);
                pfStack_54 = (float *)0x0;
                iStack_60 = 0x5605ef;
                puStack_5c = param_3;
                pfStack_50 = (float *)uVar6;
                FUN_00570090();
                uVar20 = uVar20 + 1;
              } while (uVar20 < *(ushort *)(iVar15 + 0x14));
            }
            param_1 = (int *)((int)param_1 + 1);
          } while (param_1 < (uint)piVar12[3]);
        }
        for (iVar15 = piVar12[1]; iVar15 != 0; iVar15 = *(int *)(iVar15 + 0xdc)) {
          if (*(int *)(iVar15 + 0xac) != 0) {
            *(undefined4 *)(*param_2 + param_2[1] * 4) = param_3[0x42];
            param_2[1] = param_2[1] + 1;
            *(undefined4 *)(param_4[0xf] + param_4[0x10] * 4) = *(undefined4 *)(iVar15 + 0xb0);
            param_4[0x10] = param_4[0x10] + 1;
            pfStack_50 = (float *)param_10;
            puStack_5c = (undefined4 *)0x560663;
            puStack_58 = param_3;
            pfStack_54 = (float *)iVar15;
            FUN_0056f350();
          }
        }
        pfStack_54 = (float *)0x560676;
        pfStack_50 = (float *)param_3;
        FUN_0056f020();
      }
      local_2c = local_2c + 1;
      local_28 = local_28 + 0x28;
    } while (local_2c != *(int *)(param_8 + 4));
  }
  param_3[0x4d] = 0;
  param_3[0x51] = 0x43480000;
  puVar1 = param_3 + 0x46;
  param_3[0x49] = *param_9;
  param_3[0x53] = param_9[0x16];
  puStack_7c = param_3 + 0x43;
  param_3[0x56] = (uint)param_9[0x1a] >> 2 & 1;
  pfStack_50 = (float *)*param_9;
  pfStack_54 = (float *)piVar8[7];
  puStack_58 = (undefined4 *)piVar8[6];
  puStack_5c = (undefined4 *)piVar8[3];
  iStack_60 = piVar8[2];
  iStack_64 = piVar8[1];
  iStack_68 = *piVar8;
  iStack_6c = piVar8[0xb];
  iStack_70 = piVar8[10];
  iStack_74 = piVar8[9];
  puStack_78 = (undefined4 *)piVar8[8];
  aiStack_88[1] = piVar8[0xd];
  aiStack_88[0] = piVar8[0xc];
  puStack_8c = (undefined4 *)0x56071f;
  puStack_80 = puVar1;
  FUN_0056e680();
  if (local_24 != 0) {
    pfStack_50 = (float *)param_3[0x3d];
    pfStack_54 = (float *)param_3[0x42];
    puStack_58 = param_5;
    puStack_5c = (undefined4 *)param_6;
    iStack_60 = 0x56074b;
    FUN_005675d0();
    param_3[2] = param_3[0x3d];
    iVar15 = 0;
    if (param_3[0x1a] != 0) {
      iVar17 = 0;
      do {
        iVar15 = iVar15 + 1;
        *(float *)(param_3[0x19] + iVar17) =
             *(float *)(param_3[0x19] + iVar17) + (float)param_9[0x19];
        *(float *)(iVar17 + 4 + param_3[0x19]) =
             *(float *)(iVar17 + 4 + param_3[0x19]) + (float)param_9[0x19];
        *(float *)(iVar17 + 8 + param_3[0x19]) =
             *(float *)(iVar17 + 8 + param_3[0x19]) + (float)param_9[0x19];
        iVar7 = iVar17 + 0xc;
        iVar2 = iVar17 + 0xc;
        iVar17 = iVar17 + 0x10;
        *(float *)(iVar2 + param_3[0x19]) = *(float *)(iVar7 + param_3[0x19]) + (float)param_9[0x19]
        ;
      } while (iVar15 != param_3[0x1a]);
    }
    pfStack_50 = (float *)(_DAT_005cc320 / (float)param_3[0x49]);
    param_3[0x4a] = 0;
    puStack_5c = (undefined4 *)*puVar1;
    puVar21 = param_3 + 7;
    puStack_58 = (undefined4 *)param_3[0x47];
    pfStack_54 = (float *)param_3[0x48];
    iStack_68 = param_3[10];
    if (param_9[0x16] == 2) {
      iStack_64 = param_3[0xb];
      iStack_60 = param_3[0xc];
      iStack_74 = *puVar21;
      iStack_70 = param_3[8];
      iStack_6c = param_3[9];
      puStack_80 = (undefined4 *)param_3[0x19];
      puStack_7c = (undefined4 *)param_3[0x1a];
      puStack_78 = (undefined4 *)param_3[0x1b];
      puStack_8c = (undefined4 *)param_3[0x16];
      aiStack_88[0] = param_3[0x17];
      aiStack_88[1] = param_3[0x18];
      iStack_98 = param_3[0x13];
      puStack_94 = (undefined4 *)param_3[0x14];
      puStack_90 = (undefined4 *)param_3[0x15];
      uStack_a4 = param_3[0x10];
      uStack_a0 = param_3[0x11];
      iStack_9c = param_3[0x12];
      uStack_b0 = param_3[0xd];
      uStack_ac = param_3[0xe];
      uStack_a8 = param_3[0xf];
      uStack_c0 = *param_3;
      uStack_bc = param_3[1];
      uStack_b8 = param_3[2];
      uStack_b4 = param_3[3];
      uStack_c4 = 0x5608b3;
      FUN_0056dd40();
    }
    else {
      iStack_64 = param_3[0xb];
      iStack_60 = param_3[0xc];
      iStack_74 = *puVar21;
      iStack_70 = param_3[8];
      iStack_6c = param_3[9];
      puStack_80 = (undefined4 *)param_3[0x19];
      puStack_7c = (undefined4 *)param_3[0x1a];
      puStack_78 = (undefined4 *)param_3[0x1b];
      puStack_8c = (undefined4 *)param_3[0x16];
      aiStack_88[0] = param_3[0x17];
      aiStack_88[1] = param_3[0x18];
      iStack_98 = param_3[0x13];
      puStack_94 = (undefined4 *)param_3[0x14];
      puStack_90 = (undefined4 *)param_3[0x15];
      uStack_a4 = param_3[0x10];
      uStack_a0 = param_3[0x11];
      iStack_9c = param_3[0x12];
      uStack_b0 = param_3[0xd];
      uStack_ac = param_3[0xe];
      uStack_a8 = param_3[0xf];
      uStack_c0 = *param_3;
      uStack_bc = param_3[1];
      uStack_b8 = param_3[2];
      uStack_b4 = param_3[3];
      uStack_c4 = 0x56097e;
      FUN_0056d3f0();
    }
    puStack_58 = (undefined4 *)param_3[0x3a];
    pfStack_54 = (float *)param_3[0x3b];
    pfStack_50 = (float *)param_3[0x3c];
    iStack_64 = param_3[0x37];
    iStack_60 = param_3[0x38];
    puStack_5c = (undefined4 *)param_3[0x39];
    iStack_74 = *param_3;
    iStack_70 = param_3[1];
    iStack_6c = param_3[2];
    iStack_68 = param_3[3];
    puStack_78 = param_5;
    puStack_7c = (undefined4 *)0x5609e5;
    FUN_0056d070();
    pfStack_50 = (float *)param_3[0x49];
    puStack_5c = (undefined4 *)param_3[0x43];
    puStack_58 = (undefined4 *)param_3[0x44];
    pfStack_54 = (float *)param_3[0x45];
    iStack_68 = *puVar1;
    iStack_64 = param_3[0x47];
    iStack_60 = param_3[0x48];
    iStack_74 = param_3[10];
    iStack_70 = param_3[0xb];
    iStack_6c = param_3[0xc];
    puStack_80 = (undefined4 *)*puVar21;
    puStack_7c = (undefined4 *)param_3[8];
    puStack_78 = (undefined4 *)param_3[9];
    puStack_8c = (undefined4 *)param_3[4];
    aiStack_88[0] = param_3[5];
    aiStack_88[1] = param_3[6];
    uStack_120 = param_5[0x25];
    puVar21 = param_5;
    puVar22 = auStack_114;
    for (iVar15 = 0x22; iVar15 != 0; iVar15 = iVar15 + -1) {
      *puVar22 = *puVar21;
      puVar21 = puVar21 + 1;
      puVar22 = puVar22 + 1;
    }
    uStack_11c = param_5[0x26];
    uStack_118 = param_5[0x27];
    uStack_124 = 0x560aa3;
    (*param_11)();
    piVar12 = param_3 + 0xb;
    param_3 = (undefined4 *)0x0;
    if (*piVar12 != 0) {
      iVar15 = 0;
      do {
        iVar17 = iVar15 + 0x10;
        *(float *)(iVar15 + param_5[0x22]) =
             *(float *)(iVar15 + puVar14[10]) * *(float *)(puVar14[0x22] + iVar15);
        *(float *)(param_5[0x22] + -0xc + iVar17) =
             *(float *)(iVar15 + 4 + puVar14[10]) * *(float *)(puVar14[0x22] + -0xc + iVar17);
        *(float *)(param_5[0x22] + -8 + iVar17) =
             *(float *)(iVar15 + 8 + puVar14[10]) * *(float *)(puVar14[0x22] + -8 + iVar17);
        *(float *)(param_5[0x22] + -4 + iVar17) =
             *(float *)(iVar15 + 0xc + puVar14[10]) * *(float *)(puVar14[0x22] + -4 + iVar17);
        *(undefined4 *)(iVar15 + puVar14[0x1c]) = 0;
        *(undefined4 *)(iVar15 + 4 + puVar14[0x1c]) = 0;
        *(undefined4 *)(iVar15 + 8 + puVar14[0x1c]) = 0;
        *(undefined4 *)(iVar15 + 0xc + puVar14[0x1c]) = 0;
        *(undefined4 *)(iVar15 + puVar14[0x1f]) = 0;
        *(undefined4 *)(iVar15 + 4 + puVar14[0x1f]) = 0;
        *(undefined4 *)(iVar15 + 8 + puVar14[0x1f]) = 0;
        *(undefined4 *)(iVar15 + 0xc + puVar14[0x1f]) = 0;
        param_3 = (undefined4 *)((int)param_3 + 1);
        iVar15 = iVar17;
      } while (param_3 != (undefined4 *)puVar14[0xb]);
    }
    puVar14[0x4e] = param_9[1];
    puVar14[0x50] = param_9[0x17];
    _DAT_00913284 = 0;
    pfStack_50 = (float *)param_12;
    puVar21 = puVar14 + 0x49;
    piVar12 = aiStack_88;
    for (iVar15 = 0xe; iVar15 != 0; iVar15 = iVar15 + -1) {
      *piVar12 = *puVar21;
      puVar21 = puVar21 + 1;
      piVar12 = piVar12 + 1;
    }
    iStack_98 = puVar14[0x1c];
    puStack_94 = (undefined4 *)puVar14[0x1d];
    puStack_90 = (undefined4 *)puVar14[0x1e];
    uStack_a4 = param_5[0x22];
    uStack_a0 = param_5[0x23];
    iStack_9c = param_5[0x24];
    uStack_b0 = puVar14[0x10];
    uStack_ac = puVar14[0x11];
    uStack_a8 = puVar14[0x12];
    uStack_bc = puVar14[0xd];
    uStack_b8 = puVar14[0xe];
    uStack_b4 = puVar14[0xf];
    uStack_c8 = *puVar1;
    uStack_c4 = puVar14[0x47];
    uStack_c0 = puVar14[0x48];
    puVar21 = param_5;
    puVar22 = auStack_150;
    puStack_8c = puVar14;
    for (iVar15 = 0x22; iVar15 != 0; iVar15 = iVar15 + -1) {
      *puVar22 = *puVar21;
      puVar21 = puVar21 + 1;
      puVar22 = puVar22 + 1;
    }
    (*param_13)(param_4,param_4 + 6,param_4 + 0x15,param_6);
    pfStack_50 = (float *)param_2;
    puStack_5c = (undefined4 *)param_4[6];
    puStack_58 = (undefined4 *)param_4[7];
    pfStack_54 = (float *)param_4[8];
    iStack_60 = param_7;
    iStack_64 = 0x560c95;
    FUN_005601f0();
    puVar14[0x4e] = param_9[2];
    puVar14[0x50] = param_9[0x18];
    _DAT_00913284 = 1;
    pfStack_50 = (float *)param_12;
    puVar21 = puVar14 + 0x49;
    piVar12 = aiStack_88;
    for (iVar15 = 0xe; iVar15 != 0; iVar15 = iVar15 + -1) {
      *piVar12 = *puVar21;
      puVar21 = puVar21 + 1;
      piVar12 = piVar12 + 1;
    }
    iStack_98 = puVar14[0x1f];
    puStack_94 = (undefined4 *)puVar14[0x20];
    puStack_90 = (undefined4 *)puVar14[0x21];
    uStack_a4 = param_5[0x25];
    uStack_a0 = param_5[0x26];
    iStack_9c = param_5[0x27];
    uStack_b0 = puVar14[0x16];
    uStack_ac = puVar14[0x17];
    uStack_a8 = puVar14[0x18];
    uStack_bc = puVar14[0x13];
    uStack_b8 = puVar14[0x14];
    uStack_b4 = puVar14[0x15];
    uStack_c8 = *puVar1;
    uStack_c4 = puVar14[0x47];
    uStack_c0 = puVar14[0x48];
    puVar21 = auStack_150;
    puStack_8c = puVar14;
    for (iVar15 = 0x22; iVar15 != 0; iVar15 = iVar15 + -1) {
      *puVar21 = *param_5;
      param_5 = param_5 + 1;
      puVar21 = puVar21 + 1;
    }
    (*param_13)(param_4 + 3,param_4 + 9,param_4 + 0x18,param_6);
    puStack_58 = (undefined4 *)*param_4;
    pfStack_54 = (float *)param_4[1];
    pfStack_50 = (float *)param_4[2];
    iStack_64 = param_4[3];
    iStack_60 = param_4[4];
    puStack_5c = (undefined4 *)param_4[5];
    iStack_70 = puVar14[0x2b];
    iStack_6c = puVar14[0x2c];
    iStack_68 = puVar14[0x2d];
    puStack_80 = (undefined4 *)*puVar14;
    puStack_7c = (undefined4 *)puVar14[1];
    puStack_78 = (undefined4 *)puVar14[2];
    iStack_74 = puVar14[3];
    aiStack_88[1] = piVar8[0xd];
    aiStack_88[0] = piVar8[0xc];
    puStack_8c = param_4 + 0x12;
    iStack_98 = piVar8[0x13];
    puStack_90 = param_4 + 0xf;
    iStack_9c = piVar8[0x12];
    uStack_a0 = 0x560e25;
    puStack_94 = param_4 + 0xc;
    FUN_0056caa0();
    puStack_58 = (undefined4 *)*puVar1;
    pfStack_54 = (float *)puVar14[0x47];
    pfStack_50 = (float *)puVar14[0x48];
    iStack_60 = piVar8[0x12];
    puStack_5c = (undefined4 *)piVar8[0x13];
    iStack_6c = param_4[0xc];
    iStack_68 = param_4[0xd];
    iStack_64 = param_4[0xe];
    iStack_70 = piVar8[0x11];
    iStack_74 = piVar8[0x10];
    puStack_78 = (undefined4 *)0x560e6b;
    FUN_0056c580();
  }
  pfStack_54 = (float *)piVar8[7];
  pfStack_50 = (float *)0x3f800000;
  puStack_5c = (undefined4 *)piVar8[0x15];
  puStack_58 = (undefined4 *)piVar8[6];
  iStack_60 = piVar8[0x14];
  iStack_6c = param_4[0xc];
  iStack_68 = param_4[0xd];
  iStack_64 = param_4[0xe];
  iStack_70 = piVar8[5];
  iStack_74 = piVar8[4];
  puStack_78 = (undefined4 *)piVar8[1];
  puStack_7c = (undefined4 *)*piVar8;
  puStack_80 = (undefined4 *)0x560ebc;
  FUN_0056c310();
  puStack_80 = (undefined4 *)*param_9;
  aiStack_88[1] = piVar8[0x11];
  aiStack_88[0] = piVar8[0x10];
  puStack_8c = (undefined4 *)piVar8[3];
  puStack_90 = (undefined4 *)piVar8[2];
  puStack_94 = (undefined4 *)0x560ed4;
  FUN_0056bdf0();
  piVar12 = (int *)piVar8[0x15];
  uVar20 = 0;
  if (piVar12[1] != 0) {
    param_5 = (undefined4 *)0x0;
    do {
      iVar15 = *(int *)(*piVar12 + uVar20 * 4);
      iVar17 = iVar15 * 0x20;
      pfStack_50 = (float *)(iVar17 + piVar8[0xc]);
      pfVar13 = (float *)(param_4[0xc] + (int)param_5);
      fVar3 = pfVar13[2] * pfStack_50[2] + *pfStack_50 * *pfVar13 + pfVar13[1] * pfStack_50[1];
      if (fVar3 < _DAT_005e5258) {
        pfStack_54 = &local_18;
        puStack_58 = (undefined4 *)0x560f39;
        FUN_005667c0();
        pfVar13 = (float *)(iVar17 + piVar8[2]);
        fVar4 = pfVar13[2] * local_10 +
                *pfVar13 * local_18 + *(float *)(iVar17 + 4 + piVar8[2]) * local_14;
        fVar3 = fVar3 / *(float *)(iVar15 * 0x30 + 0xc + piVar8[8]);
        fVar5 = fVar4 * fVar4 + fVar3 + fVar3;
        fVar3 = DAT_005d757c;
        if (fVar5 < (float)PTR_DAT_005ceabc == (fVar5 == (float)PTR_DAT_005ceabc)) {
          fVar3 = _DAT_005cc320;
          if (fVar4 < DAT_005d757c) {
            fVar3 = _DAT_005cc33c;
          }
          fVar3 = SQRT(fVar5) * fVar3;
        }
        fVar3 = fVar3 - fVar4;
        *pfVar13 = local_18 * fVar3 + *pfVar13;
        pfVar13 = (float *)(iVar17 + 4 + piVar8[2]);
        *pfVar13 = local_14 * fVar3 + *pfVar13;
        pfVar13 = (float *)(iVar17 + 8 + piVar8[2]);
        *pfVar13 = fVar3 * local_10 + *pfVar13;
      }
      piVar12 = (int *)piVar8[0x15];
      uVar20 = uVar20 + 1;
      param_5 = (undefined4 *)((int)param_5 + 0x20);
    } while (uVar20 < (uint)piVar12[1]);
  }
  iVar15 = piVar8[0xc];
  piVar8 = (int *)piVar8[0xd];
  iVar17 = 0;
  if (piVar8[1] != 0) {
    do {
      puVar14 = (undefined4 *)(*(int *)(*piVar8 + iVar17 * 4) * 0x20 + iVar15);
      iVar17 = iVar17 + 1;
      puVar14[2] = 0;
      puVar14[1] = 0;
      *puVar14 = 0;
      puVar14[6] = 0;
      puVar14[5] = 0;
      puVar14[4] = 0;
    } while (iVar17 != piVar8[1]);
  }
  return;
}

