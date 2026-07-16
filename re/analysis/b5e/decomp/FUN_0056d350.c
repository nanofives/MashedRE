
/* [C1 2026-05-19] Signature: `void FUN_0056d350(int *param_1, float *param_2, float *param_3, int
   param_4, int param_5, int param_6)`. */

void FUN_0056d350(int *param_1,float *param_2,float *param_3,int param_4,int param_5,int param_6)

{
  int iVar1;
  int iVar2;

  iVar1 = (param_6 + (param_5 + param_4 * 4) * 2) * 0x20;
  *param_2 = *(float *)(*param_1 + iVar1) * param_3[3];
  param_2[1] = *(float *)(*param_1 + 4 + iVar1) * param_3[3];
  param_2[2] = *(float *)(*param_1 + 8 + iVar1) * param_3[3];
  iVar2 = *param_1 + iVar1;
  param_2[4] = *(float *)(iVar2 + 0x10) * *param_3 +
               *(float *)(iVar2 + 0x14) * param_3[1] + *(float *)(iVar2 + 0x18) * param_3[2];
  iVar2 = *param_1 + iVar1;
  param_2[5] = *(float *)(iVar2 + 0x10) * param_3[4] +
               *(float *)(iVar2 + 0x14) * param_3[5] + *(float *)(iVar2 + 0x18) * param_3[6];
  iVar1 = iVar1 + *param_1;
  param_2[6] = *(float *)(iVar1 + 0x10) * param_3[8] +
               *(float *)(iVar1 + 0x14) * param_3[9] + *(float *)(iVar1 + 0x18) * param_3[10];
  return;
}

