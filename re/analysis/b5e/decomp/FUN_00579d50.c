
/* [C1 2026-05-20] Signature recovered: `float10 FUN_00579d50(float *out_simplex_entry, float
   *direction, int side_flag,… */

float10 FUN_00579d50(float *param_1,float *param_2,int param_3,int *param_4,float *param_5)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  float *pfVar4;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  float local_8;
  float local_4;

  local_18 = -*param_2;
  local_14 = -param_2[1];
  local_10 = -param_2[2];
  iVar1 = *param_4;
  if (param_3 < 1) {
    FUN_0055c000(*(undefined4 *)(iVar1 + 8),*(undefined4 *)(iVar1 + 0x10),&local_18,&local_c);
    uVar3 = *(undefined4 *)(param_4[1] + 0x10);
    uVar2 = *(undefined4 *)(param_4[1] + 8);
    pfVar4 = param_2;
  }
  else {
    FUN_0055c000(*(undefined4 *)(iVar1 + 8),*(undefined4 *)(iVar1 + 0x10),param_2,&local_c);
    uVar3 = *(undefined4 *)(param_4[1] + 0x10);
    uVar2 = *(undefined4 *)(param_4[1] + 8);
    pfVar4 = &local_18;
  }
  FUN_0055c000(uVar2,uVar3,pfVar4,param_1 + 3);
  local_c = local_c - param_1[3];
  *param_1 = local_c;
  param_1[1] = local_8 - param_1[4];
  param_1[2] = local_4 - param_1[5];
  if (param_5 != (float *)0x0) {
    *param_1 = local_c + *param_5;
    param_1[1] = param_5[1] + (local_8 - param_1[4]);
    param_1[2] = param_5[2] + (local_4 - param_1[5]);
  }
  return (float10)param_2[2] * (float10)param_1[2] +
         (float10)*param_1 * (float10)*param_2 + (float10)param_2[1] * (float10)param_1[1];
}

