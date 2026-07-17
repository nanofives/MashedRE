
/* [C1 2026-05-19] Builds compressed output rows from a stride-16 source `param_2` indexed by
   `param_5[0]` (index array, count… */

void FUN_005601f0(int *param_1,int param_2,undefined4 param_3,undefined4 param_4,int *param_5)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;

  iVar4 = 0;
  if (param_5[1] != 0) {
    iVar2 = 0;
    do {
      iVar3 = iVar2 + 0x10;
      puVar1 = (undefined4 *)((*(uint *)(*param_5 + iVar4 * 4) >> 2) * 0x10 + param_2);
      iVar4 = iVar4 + 1;
      *(undefined4 *)(iVar2 + *param_1) = *puVar1;
      *(undefined4 *)(*param_1 + -0xc + iVar3) = puVar1[3];
      *(undefined4 *)(*param_1 + -8 + iVar3) = puVar1[6];
      *(undefined4 *)(*param_1 + -4 + iVar3) = puVar1[9];
      iVar2 = iVar3;
    } while (iVar4 != param_5[1]);
    param_1[1] = param_5[1];
    return;
  }
  param_1[1] = param_5[1];
  return;
}

