
/* [C1 2026-05-19] Signature: `void FUN_0056caa0(int param_1, ..., int *param_3, int *param_4, int
   *param_5, int param_6, int *param_7… */

void FUN_0056caa0(int param_1,undefined4 param_2,int *param_3,int *param_4,int *param_5,int param_6,
                 int *param_7,int param_8,uint param_9,undefined4 param_10,undefined4 param_11,
                 int param_12,int param_13,undefined4 param_14,int param_15,undefined4 param_16,
                 undefined4 param_17,int param_18)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  float *pfVar5;
  int iVar6;
  int iVar7;
  float *pfVar8;
  int local_a0;
  int local_9c;
  uint local_90;
  float local_80 [4];
  float local_70;
  float local_6c;
  float local_68;
  float local_60 [4];
  float local_50;
  float local_4c;
  float local_48;
  float local_40 [4];
  float local_30;
  float local_2c;
  float local_28;
  float local_20 [4];
  float local_10;
  float local_c;
  float local_8;

  iVar1 = *param_7;
  iVar7 = param_7[1];
  iVar3 = 0;
  if (param_13 != 0) {
    iVar4 = 0;
    do {
      iVar3 = iVar3 + 1;
      iVar6 = iVar4 + 0x40;
      **(undefined4 **)(*param_4 + -4 + iVar3 * 4) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 4) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 8) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x10) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x14) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x18) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x20) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x24) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x28) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x30) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x34) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x38) = 0;
      *(undefined4 *)(iVar4 + *param_5) = 0;
      *(undefined4 *)(*param_5 + -0x3c + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x38 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x30 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x2c + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x28 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x20 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x1c + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x18 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x10 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0xc + iVar6) = 0;
      *(undefined4 *)(*param_5 + -8 + iVar6) = 0;
      iVar4 = iVar6;
    } while (iVar3 != param_13);
  }
  local_90 = param_9 >> 2;
  param_4[1] = param_13;
  param_5[1] = param_13;
  if (local_90 != 0) {
    local_a0 = param_18;
    do {
      iVar3 = *(int *)(param_8 + 0x2c);
      iVar4 = *(int *)(param_8 + 0x3c);
      iVar6 = *(int *)(param_8 + 0x1c);
      if (*(int *)(param_8 + 0xc) != -1) {
        FUN_0056c8e0(*(undefined4 *)(*param_4 + iVar6 * 4),param_8,(param_15 - param_18) + local_a0,
                     0);
        FUN_0056c8e0(iVar6 * 0x40 + *param_5,param_8,local_a0,0);
      }
      if (iVar3 != -1) {
        FUN_0056c8e0(*(undefined4 *)(*param_4 + iVar4 * 4),param_8,(param_15 - param_18) + local_a0,
                     1);
        FUN_0056c8e0(iVar4 * 0x40 + *param_5,param_8,local_a0,1);
      }
      local_a0 = local_a0 + 0x10;
      param_8 = param_8 + 0x100;
      local_90 = local_90 - 1;
    } while (local_90 != 0);
  }
  local_a0 = 0;
  if (iVar7 != 0) {
    iVar3 = 0;
    do {
      iVar4 = *(int *)(iVar1 + local_a0 * 4) * 0x20;
      *(undefined4 *)(iVar4 + param_1) = *(undefined4 *)(iVar4 + param_6);
      *(undefined4 *)(iVar4 + 4 + param_1) = *(undefined4 *)(iVar4 + 4 + param_6);
      uVar2 = *(undefined4 *)(iVar4 + 8 + param_6);
      *(undefined4 *)(iVar4 + 0xc + param_1) = 0;
      *(undefined4 *)(iVar4 + 8 + param_1) = uVar2;
      *(undefined4 *)(iVar4 + 0x10 + param_1) = *(undefined4 *)(iVar4 + 0x10 + param_6);
      *(undefined4 *)(iVar4 + 0x14 + param_1) = *(undefined4 *)(iVar4 + 0x14 + param_6);
      uVar2 = *(undefined4 *)(iVar4 + 0x18 + param_6);
      *(undefined4 *)(iVar4 + 0x1c + param_1) = 0;
      *(undefined4 *)(iVar4 + 0x18 + param_1) = uVar2;
      *(undefined4 *)(iVar3 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 4 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 8 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 0xc + *param_3) = 0;
      *(undefined4 *)(iVar3 + 0x10 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 0x14 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 0x18 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 0x1c + *param_3) = 0;
      local_a0 = local_a0 + 1;
      iVar3 = iVar3 + 0x20;
    } while (local_a0 != iVar7);
  }
  param_3[1] = iVar7;
  local_a0 = 0;
  if (param_13 != 0) {
    local_9c = 0;
    do {
      iVar7 = *(int *)(param_12 + local_a0 * 8);
      iVar3 = *(int *)(param_12 + 4 + local_a0 * 8);
      if (iVar7 != -1) {
        pfVar5 = *(float **)(*param_4 + local_a0 * 4);
        pfVar8 = local_80;
        for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
          *pfVar8 = *pfVar5;
          pfVar5 = pfVar5 + 1;
          pfVar8 = pfVar8 + 1;
        }
        pfVar5 = (float *)(*(int *)(iVar1 + iVar7 * 4) * 0x20 + param_1);
        *pfVar5 = local_80[0] + *pfVar5;
        pfVar5[1] = local_80[1] + pfVar5[1];
        pfVar5[2] = local_80[2] + pfVar5[2];
        pfVar5[4] = local_70 + pfVar5[4];
        pfVar5[5] = local_6c + pfVar5[5];
        pfVar5[6] = local_68 + pfVar5[6];
        pfVar5 = (float *)(*param_5 + local_9c);
        pfVar8 = local_40;
        for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
          *pfVar8 = *pfVar5;
          pfVar5 = pfVar5 + 1;
          pfVar8 = pfVar8 + 1;
        }
        pfVar5 = (float *)(iVar7 * 0x20 + *param_3);
        *pfVar5 = local_40[0] + *pfVar5;
        pfVar5[1] = local_40[1] + pfVar5[1];
        pfVar5[2] = local_40[2] + pfVar5[2];
        pfVar5[4] = local_30 + pfVar5[4];
        pfVar5[5] = local_2c + pfVar5[5];
        pfVar5[6] = local_28 + pfVar5[6];
      }
      if (iVar3 != -1) {
        pfVar5 = (float *)(*(int *)(*param_4 + local_a0 * 4) + 0x20);
        pfVar8 = local_60;
        for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
          *pfVar8 = *pfVar5;
          pfVar5 = pfVar5 + 1;
          pfVar8 = pfVar8 + 1;
        }
        pfVar5 = (float *)(*(int *)(iVar1 + iVar3 * 4) * 0x20 + param_1);
        *pfVar5 = local_60[0] + *pfVar5;
        pfVar5[1] = local_60[1] + pfVar5[1];
        pfVar5[2] = local_60[2] + pfVar5[2];
        pfVar5[4] = local_50 + pfVar5[4];
        pfVar5[5] = local_4c + pfVar5[5];
        pfVar5[6] = local_48 + pfVar5[6];
        pfVar5 = (float *)(*param_5 + 0x20 + local_9c);
        pfVar8 = local_20;
        for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
          *pfVar8 = *pfVar5;
          pfVar5 = pfVar5 + 1;
          pfVar8 = pfVar8 + 1;
        }
        pfVar5 = (float *)(iVar3 * 0x20 + *param_3);
        *pfVar5 = local_20[0] + *pfVar5;
        pfVar5[1] = local_20[1] + pfVar5[1];
        pfVar5[2] = local_20[2] + pfVar5[2];
        pfVar5[4] = local_10 + pfVar5[4];
        pfVar5[5] = local_c + pfVar5[5];
        pfVar5[6] = local_8 + pfVar5[6];
      }
      local_a0 = local_a0 + 1;
      local_9c = local_9c + 0x40;
    } while (local_a0 != param_13);
  }
  return;
}

