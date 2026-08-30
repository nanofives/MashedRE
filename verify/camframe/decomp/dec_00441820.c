
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-06-01] `void FUN_00441820(int param_1, undefined4 *param_2 /*out orientation*/, float
   *param_3 /*outâ€¦ */

void FUN_00441820(int param_1,undefined4 *param_2,float *param_3)

{
  int iVar1;
  undefined4 *puVar2;
  float *pfVar3;
  float local_64;
  float local_60;
  float local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  float local_44;
  undefined1 local_40 [64];
  
  local_58 = 0;
  local_54 = 0xbf800000;
  local_50 = 0;
  *param_3 = 0.0;
  FUN_00409790();
  iVar1 = FUN_004098a0();
  if (iVar1 != 0) {
    pfVar3 = (float *)(iVar1 + param_1 * 0xc);
    if (pfVar3[2] != -1.0) {
      *param_3 = pfVar3[2];
    }
    if (*pfVar3 != -1.0) {
      FUN_004c4d20(local_40,&DAT_006146f0,_DAT_005ccad0 - *pfVar3,0);
      FUN_004c3df0(param_2,&local_58,1,local_40);
      FUN_004c4d20(local_40,&DAT_006146fc,pfVar3[1] + _DAT_005cd09c,0);
      FUN_004c3df0(param_2,param_2,1,local_40);
      return;
    }
  }
  puVar2 = (undefined4 *)FUN_00426cc0(param_1);
  *param_2 = *puVar2;
  param_2[1] = puVar2[1];
  param_2[2] = puVar2[2];
  pfVar3 = (float *)FUN_00426d00(param_1,0);
  local_64 = *pfVar3;
  local_60 = pfVar3[1];
  local_5c = pfVar3[2];
  pfVar3 = (float *)FUN_00426d00(param_1,3);
  local_44 = pfVar3[2];
  local_64 = local_64 - *pfVar3;
  local_60 = local_60 - pfVar3[1];
  local_5c = local_5c - local_44;
  FUN_004c4d20(local_40,&local_64,0xc1c80000,0);
  FUN_004c3df0(param_2,param_2,1,local_40);
  return;
}

