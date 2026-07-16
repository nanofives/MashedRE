
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `float10 FUN_0057a660(int *body_pair, float *out_axis)` —
   initial-axis selection… */

float10 FUN_0057a660(int *param_1,float *param_2)

{
  int iVar1;
  int iVar2;
  float *pfVar3;
  float fVar4;
  float *pfVar5;
  float10 fVar6;
  float10 fVar7;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  undefined1 local_38 [24];
  float local_20;
  float fStack_1c;
  float fStack_18;
  float fStack_10;
  float fStack_c;
  float fStack_8;

  pfVar3 = param_2;
  iVar1 = *(int *)(*param_1 + 8);
  if ((**(short **)(iVar1 + 0x5c) == 4) && ((*(byte *)(iVar1 + 0x48) & 1) != 0)) {
    pfVar5 = (float *)FUN_0057ae20(iVar1);
    local_50 = *pfVar5;
    local_4c = pfVar5[1];
    local_44 = local_50 * _DAT_005cc33c;
    local_48 = pfVar5[2];
    local_40 = local_4c * _DAT_005cc33c;
    *param_2 = local_44;
    local_3c = local_48 * _DAT_005cc33c;
    param_2[1] = local_40;
    param_2[2] = local_3c;
  }
  else {
    iVar2 = *(int *)(param_1[1] + 8);
    if ((**(short **)(iVar2 + 0x5c) == 4) && ((*(byte *)(iVar2 + 0x48) & 1) != 0)) {
      pfVar5 = (float *)FUN_0057ae20(iVar2);
      local_44 = *pfVar5;
      local_40 = pfVar5[1];
      local_3c = pfVar5[2];
      *param_2 = local_44;
      param_2[1] = local_40;
      param_2[2] = local_3c;
    }
    else {
      FUN_0055bd70(iVar1,*(undefined4 *)(*param_1 + 0x10),0,&local_20);
      local_44 = (local_20 - fStack_10) * _DAT_005cc32c + fStack_10;
      local_40 = (fStack_1c - fStack_c) * _DAT_005cc32c + fStack_c;
      local_3c = (fStack_18 - fStack_8) * _DAT_005cc32c + fStack_8;
      FUN_0055bd70(*(undefined4 *)(param_1[1] + 8),*(undefined4 *)(param_1[1] + 0x10),0,&local_20);
      local_50 = (local_20 - fStack_10) * _DAT_005cc32c + fStack_10;
      local_4c = (fStack_1c - fStack_c) * _DAT_005cc32c + fStack_c;
      local_48 = (fStack_18 - fStack_8) * _DAT_005cc32c + fStack_8;
      local_44 = local_44 - local_50;
      local_40 = local_40 - local_4c;
      local_3c = local_3c - local_48;
      FUN_005667c0(param_2,&local_44);
    }
  }
  fVar6 = (float10)FUN_00579d50(local_38,param_2,0xffffffff,param_1,0);
  local_50 = -1.0;
  if (DAT_005d757c <= local_44) {
    local_50 = 1.0;
  }
  local_4c = 0.0;
  local_48 = 0.0;
  fVar7 = (float10)FUN_00579d50(local_38,&local_50,0xffffffff,param_1,0);
  fVar4 = (float)-fVar6;
  if (-fVar7 < (float10)(float)-fVar6) {
    *param_2 = local_50;
    param_2[1] = local_4c;
    param_2[2] = local_48;
    fVar4 = (float)-fVar7;
  }
  param_2 = (float *)fVar4;
  local_50 = 0.0;
  local_4c = -1.0;
  if (DAT_005d757c <= local_40) {
    local_4c = 1.0;
  }
  local_48 = 0.0;
  fVar6 = (float10)FUN_00579d50(local_38,&local_50,0xffffffff,param_1,0);
  if (-fVar6 < (float10)(float)param_2) {
    param_2 = (float *)(float)-fVar6;
    *pfVar3 = local_50;
    pfVar3[1] = local_4c;
    pfVar3[2] = local_48;
  }
  local_50 = 0.0;
  local_4c = 0.0;
  local_48 = -1.0;
  if (DAT_005d757c <= local_3c) {
    local_48 = 1.0;
  }
  fVar6 = (float10)FUN_00579d50(local_38,&local_50,0xffffffff,param_1,0);
  if ((float10)(float)param_2 <= -fVar6) {
    return (float10)(float)param_2;
  }
  *pfVar3 = local_50;
  pfVar3[1] = local_4c;
  pfVar3[2] = local_48;
  return -fVar6;
}

