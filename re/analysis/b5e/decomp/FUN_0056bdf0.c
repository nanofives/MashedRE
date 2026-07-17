
/* [C1 2026-05-19] Signature: `void FUN_0056bdf0(int param_1, undefined4 param_2, int param_3, int
   *param_4, float param_5)`. */

void FUN_0056bdf0(int param_1,undefined4 param_2,int param_3,int *param_4,float param_5)

{
  float *pfVar1;
  int iVar2;
  int iVar3;

  iVar3 = 0;
  if (param_4[1] != 0) {
    do {
      iVar2 = *(int *)(*param_4 + iVar3 * 4) * 0x20;
      iVar3 = iVar3 + 1;
      pfVar1 = (float *)(iVar2 + 0x18 + param_1);
      *(float *)(iVar2 + param_1) =
           param_5 * *(float *)(iVar2 + param_3) + *(float *)(iVar2 + param_1);
      *(float *)(iVar2 + 4 + param_1) =
           param_5 * *(float *)(iVar2 + 4 + param_3) + *(float *)(iVar2 + 4 + param_1);
      *(float *)(iVar2 + 8 + param_1) =
           param_5 * *(float *)(iVar2 + 8 + param_3) + *(float *)(iVar2 + 8 + param_1);
      *(float *)(iVar2 + 0x10 + param_1) =
           param_5 * *(float *)(iVar2 + 0x10 + param_3) + *(float *)(iVar2 + 0x10 + param_1);
      *(float *)(iVar2 + 0x14 + param_1) =
           param_5 * *(float *)(iVar2 + 0x14 + param_3) + *(float *)(iVar2 + 0x14 + param_1);
      *pfVar1 = param_5 * *(float *)(iVar2 + 0x18 + param_3) + *pfVar1;
    } while (iVar3 != param_4[1]);
  }
  return;
}

