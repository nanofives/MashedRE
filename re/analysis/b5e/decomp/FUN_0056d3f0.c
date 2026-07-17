
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_0056d3f0(int param_1)` (most args on stack). */

void FUN_0056d3f0(int param_1)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float *pfVar4;
  int iVar5;
  float *pfVar6;
  float *pfVar7;
  float *pfVar8;
  float *pfVar9;
  float *pfVar10;
  int in_stack_00000014;
  int in_stack_00000020;
  int in_stack_0000002c;
  int in_stack_00000038;
  float *in_stack_00000044;
  int in_stack_00000050;
  int in_stack_0000005c;
  int in_stack_00000060;
  int in_stack_00000068;
  float in_stack_00000074;
  float local_94;
  float local_90;
  float local_8c;
  float local_88;
  float local_84;
  float local_80;
  float local_7c;
  float local_74;
  float local_70;
  float local_6c;
  int local_64;
  int local_60;
  int *local_5c;
  float local_58;
  float local_54;
  float local_50;
  float local_4c;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  int local_4;

  local_64 = 0;
  if (in_stack_00000060 != 0) {
    pfVar10 = (float *)(in_stack_00000020 + 0xc);
    local_30 = in_stack_00000020 - in_stack_00000014;
    local_4 = in_stack_0000002c - in_stack_00000014;
    local_3c = in_stack_00000038 - in_stack_00000014;
    pfVar9 = (float *)(in_stack_00000014 + 8);
    local_48 = (int)in_stack_00000044 - in_stack_00000014;
    local_20 = in_stack_00000050 - in_stack_00000014;
    local_5c = (int *)(param_1 + 0x2c);
    local_24 = in_stack_0000005c - in_stack_00000014;
    local_10 = in_stack_0000002c - in_stack_00000020;
    local_18 = in_stack_00000038 - in_stack_00000020;
    local_44 = (int)in_stack_00000044 - in_stack_00000020;
    local_38 = in_stack_00000050 - in_stack_00000020;
    local_1c = in_stack_0000005c - in_stack_00000020;
    local_c = in_stack_0000002c - in_stack_00000038;
    local_40 = (int)in_stack_00000044 - in_stack_00000038;
    local_8 = in_stack_00000050 - in_stack_00000038;
    local_2c = in_stack_0000005c - in_stack_00000038;
    local_14 = in_stack_0000002c - (int)in_stack_00000044;
    local_28 = in_stack_00000050 - (int)in_stack_00000044;
    local_34 = in_stack_0000005c - (int)in_stack_00000044;
    pfVar6 = (float *)(in_stack_00000038 + 4);
    pfVar7 = in_stack_00000044;
    do {
      local_94 = 0.0;
      local_90 = 0.0;
      local_8c = 0.0;
      local_88 = 0.0;
      local_58 = 0.0;
      local_54 = 0.0;
      local_50 = 0.0;
      local_4c = 0.0;
      if (local_5c[-8] != -1) {
        local_60 = local_5c[-8] * 0x30 + in_stack_00000068;
        FUN_0056d350(&param_1,&local_84,local_60,local_64,0,0);
        local_94 = local_84 * local_84 +
                   local_80 * local_80 +
                   local_7c * local_7c +
                   local_74 * local_74 + local_70 * local_70 + local_6c * local_6c;
        FUN_0056d350(&param_1,&local_84,local_60,local_64,1,0);
        local_90 = local_84 * local_84 +
                   local_80 * local_80 +
                   local_7c * local_7c +
                   local_74 * local_74 + local_70 * local_70 + local_6c * local_6c;
        FUN_0056d350(&param_1,&local_84,local_60,local_64,2,0);
        local_8c = local_84 * local_84 +
                   local_80 * local_80 +
                   local_7c * local_7c +
                   local_74 * local_74 + local_70 * local_70 + local_6c * local_6c;
        FUN_0056d350(&param_1,&local_84,local_60,local_64,3,0);
        local_88 = local_84 * local_84 +
                   local_80 * local_80 +
                   local_7c * local_7c +
                   local_74 * local_74 + local_70 * local_70 + local_6c * local_6c;
      }
      if (*local_5c != -1) {
        local_60 = *local_5c * 0x30 + in_stack_00000068;
        FUN_0056d350(&param_1,&local_84,local_60,local_64,0,1);
        local_94 = local_84 * local_84 +
                   local_80 * local_80 +
                   local_7c * local_7c +
                   local_74 * local_74 + local_70 * local_70 + local_6c * local_6c + local_94;
        FUN_0056d350(&param_1,&local_84,local_60,local_64,1,1);
        local_90 = local_84 * local_84 +
                   local_80 * local_80 +
                   local_7c * local_7c +
                   local_74 * local_74 + local_70 * local_70 + local_6c * local_6c + local_90;
        FUN_0056d350(&param_1,&local_84,local_60,local_64,2,1);
        local_8c = local_84 * local_84 +
                   local_80 * local_80 +
                   local_7c * local_7c +
                   local_74 * local_74 + local_70 * local_70 + local_6c * local_6c + local_8c;
        FUN_0056d350(&param_1,&local_84,local_60,local_64,3,1);
        local_88 = local_84 * local_84 +
                   local_80 * local_80 +
                   local_7c * local_7c +
                   local_74 * local_74 + local_70 * local_70 + local_6c * local_6c + local_88;
      }
      fVar1 = in_stack_00000074 * *pfVar7 + local_94;
      fVar2 = in_stack_00000074 * *(float *)(local_40 + (int)pfVar6) + local_90;
      fVar3 = in_stack_00000074 * *(float *)(local_48 + (int)pfVar9) + local_8c;
      local_88 = in_stack_00000074 * *(float *)(local_44 + (int)pfVar10) + local_88;
      local_94 = fVar1;
      if (fVar1 != DAT_005d757c) {
        local_94 = _DAT_005cc320 / SQRT(fVar1);
        local_58 = fVar1 * local_94;
      }
      local_90 = fVar2;
      if (fVar2 != DAT_005d757c) {
        local_90 = _DAT_005cc320 / SQRT(fVar2);
        local_54 = fVar2 * local_90;
      }
      local_8c = fVar3;
      if (fVar3 != DAT_005d757c) {
        local_8c = _DAT_005cc320 / SQRT(fVar3);
        local_50 = fVar3 * local_8c;
      }
      fVar1 = local_88;
      if (local_88 != DAT_005d757c) {
        fVar1 = _DAT_005cc320 / SQRT(local_88);
        local_4c = local_88 * fVar1;
      }
      iVar5 = 2;
      pfVar4 = (float *)(local_5c + -9);
      do {
        iVar5 = iVar5 + -1;
        pfVar4[-2] = local_94 * pfVar4[-2];
        pfVar4[-1] = local_94 * pfVar4[-1];
        *pfVar4 = local_94 * *pfVar4;
        pfVar4[2] = local_94 * pfVar4[2];
        pfVar4[3] = local_94 * pfVar4[3];
        pfVar4[4] = local_94 * pfVar4[4];
        pfVar4[0xe] = local_90 * pfVar4[0xe];
        pfVar4[0xf] = local_90 * pfVar4[0xf];
        pfVar4[0x10] = local_90 * pfVar4[0x10];
        pfVar4[0x12] = local_90 * pfVar4[0x12];
        pfVar4[0x13] = local_90 * pfVar4[0x13];
        pfVar4[0x14] = local_90 * pfVar4[0x14];
        pfVar4[0x1e] = local_8c * pfVar4[0x1e];
        pfVar4[0x1f] = local_8c * pfVar4[0x1f];
        pfVar4[0x20] = local_8c * pfVar4[0x20];
        pfVar4[0x22] = local_8c * pfVar4[0x22];
        pfVar4[0x23] = local_8c * pfVar4[0x23];
        pfVar4[0x24] = local_8c * pfVar4[0x24];
        pfVar4[0x2e] = fVar1 * pfVar4[0x2e];
        pfVar4[0x2f] = fVar1 * pfVar4[0x2f];
        pfVar4[0x30] = fVar1 * pfVar4[0x30];
        pfVar4[0x32] = fVar1 * pfVar4[0x32];
        pfVar4[0x33] = fVar1 * pfVar4[0x33];
        pfVar4[0x34] = fVar1 * pfVar4[0x34];
        pfVar4 = pfVar4 + 8;
      } while (iVar5 != 0);
      if (pfVar9[-2] != -3.4028235e+38) {
        pfVar9[-2] = local_58 * pfVar9[-2];
      }
      if (pfVar9[-1] != -3.4028235e+38) {
        pfVar9[-1] = local_54 * pfVar9[-1];
      }
      if (*pfVar9 != -3.4028235e+38) {
        *pfVar9 = local_50 * *pfVar9;
      }
      if (pfVar9[1] != -3.4028235e+38) {
        pfVar9[1] = local_4c * pfVar9[1];
      }
      if (pfVar10[-3] != 3.4028235e+38) {
        pfVar10[-3] = local_58 * pfVar10[-3];
      }
      if (pfVar10[-2] != 3.4028235e+38) {
        pfVar10[-2] = local_54 * pfVar10[-2];
      }
      if (*(int *)(local_30 + (int)pfVar9) != 0x7f7fffff) {
        *(float *)(local_30 + (int)pfVar9) = local_50 * *(float *)(local_30 + (int)pfVar9);
      }
      if (*pfVar10 != 3.4028235e+38) {
        *pfVar10 = local_4c * *pfVar10;
      }
      if (*(int *)(local_14 + (int)pfVar7) != -0x800001) {
        *(float *)(local_14 + (int)pfVar7) = local_58 * *(float *)(local_14 + (int)pfVar7);
      }
      if (*(int *)(local_c + (int)pfVar6) != -0x800001) {
        *(float *)(local_c + (int)pfVar6) = local_54 * *(float *)(local_c + (int)pfVar6);
      }
      if (*(int *)(local_4 + (int)pfVar9) != -0x800001) {
        *(float *)(local_4 + (int)pfVar9) = local_50 * *(float *)(local_4 + (int)pfVar9);
      }
      if (*(int *)(local_10 + (int)pfVar10) != -0x800001) {
        *(float *)(local_10 + (int)pfVar10) = local_4c * *(float *)(local_10 + (int)pfVar10);
      }
      if (pfVar6[-1] != 3.4028235e+38) {
        pfVar6[-1] = local_58 * pfVar6[-1];
      }
      if (*pfVar6 != 3.4028235e+38) {
        *pfVar6 = local_54 * *pfVar6;
      }
      if (*(int *)(local_3c + (int)pfVar9) != 0x7f7fffff) {
        *(float *)(local_3c + (int)pfVar9) = local_50 * *(float *)(local_3c + (int)pfVar9);
      }
      if (*(int *)(local_18 + (int)pfVar10) != 0x7f7fffff) {
        *(float *)(local_18 + (int)pfVar10) = local_4c * *(float *)(local_18 + (int)pfVar10);
      }
      local_64 = local_64 + 1;
      local_5c = local_5c + 0x40;
      pfVar9 = pfVar9 + 4;
      pfVar10 = pfVar10 + 4;
      pfVar4 = pfVar6 + 4;
      pfVar8 = pfVar7 + 4;
      *pfVar7 = local_94 * *pfVar7 * local_94;
      *(float *)(local_40 + -0x10 + (int)pfVar4) =
           local_90 * local_90 * *(float *)(local_40 + -0x10 + (int)pfVar4);
      *(float *)(local_48 + -0x10 + (int)pfVar9) =
           local_8c * *(float *)(local_48 + -0x10 + (int)pfVar9) * local_8c;
      *(float *)(local_44 + -0x10 + (int)pfVar10) =
           fVar1 * *(float *)(local_44 + -0x10 + (int)pfVar10) * fVar1;
      *(float *)(local_28 + -0x10 + (int)pfVar8) =
           local_94 * *(float *)(local_28 + -0x10 + (int)pfVar8);
      *(float *)((int)pfVar6 + local_8) = local_90 * *(float *)((int)pfVar6 + local_8);
      *(float *)(local_20 + -0x10 + (int)pfVar9) =
           local_8c * *(float *)(local_20 + -0x10 + (int)pfVar9);
      *(float *)(local_38 + -0x10 + (int)pfVar10) =
           fVar1 * *(float *)(local_38 + -0x10 + (int)pfVar10);
      *(float *)(local_34 + -0x10 + (int)pfVar8) =
           local_94 * *(float *)(local_34 + -0x10 + (int)pfVar8);
      *(float *)((int)pfVar6 + local_2c) = local_90 * *(float *)((int)pfVar6 + local_2c);
      *(float *)(local_24 + -0x10 + (int)pfVar9) =
           local_8c * *(float *)(local_24 + -0x10 + (int)pfVar9);
      *(float *)(local_1c + -0x10 + (int)pfVar10) =
           fVar1 * *(float *)(local_1c + -0x10 + (int)pfVar10);
      pfVar6 = pfVar4;
      pfVar7 = pfVar8;
    } while (local_64 != in_stack_00000060);
  }
  return;
}

