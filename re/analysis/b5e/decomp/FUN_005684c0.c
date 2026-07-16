
/* [C1 2026-05-19] Signature: `void FUN_005684c0(int param_1, int *param_2, int *param_3, undefined4
   *param_4, undefined4 *param_5)`. */

void FUN_005684c0(int param_1,int *param_2,int *param_3,undefined4 *param_4,undefined4 *param_5)

{
  undefined4 *puVar1;
  int iVar2;

  iVar2 = *(int *)(param_1 + 4);
  *(int *)(param_1 + 4) = iVar2 + 1;
  puVar1 = (undefined4 *)(param_1 + 8 + iVar2 * 0x28);
  *puVar1 = 1;
  if ((param_3 == (int *)0x0) || (param_2 == param_3)) {
    *param_2 = (int)param_2;
    param_2[1] = (int)puVar1;
    param_2[2] = 0;
    puVar1[2] = param_2;
    puVar1[1] = 1;
  }
  else {
    *param_2 = (int)param_2;
    param_2[1] = (int)puVar1;
    param_2[2] = (int)param_3;
    *param_3 = (int)param_2;
    param_3[1] = 0;
    param_3[2] = 0;
    puVar1[2] = param_2;
    param_2 = (int *)param_2[2];
    puVar1[1] = 2;
  }
  puVar1[3] = param_2;
  if (param_4 != (undefined4 *)0x0) {
    *param_4 = 0;
    puVar1[6] = param_4;
    puVar1[5] = param_4;
    puVar1[4] = 1;
    puVar1[7] = 0;
  }
  if (param_5 != (undefined4 *)0x0) {
    *param_5 = 0;
    puVar1[4] = 0;
    puVar1[9] = param_5;
    puVar1[8] = param_5;
    puVar1[7] = 1;
  }
  return;
}

