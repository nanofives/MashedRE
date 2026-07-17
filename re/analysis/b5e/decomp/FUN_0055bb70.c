
/* [C1 2026-05-19] If `(*(byte *)(*(int *)(param_1 + 0x5c) + 0x40) & 2) == 0` AND `(*(uint
   *)(param_1 + 0xc) & 0x20000) == 0` (cited… */

void FUN_0055bb70(int param_1,int param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined1 local_40 [64];

  if (((*(byte *)(*(int *)(param_1 + 0x5c) + 0x40) & 2) == 0) &&
     ((*(uint *)(param_1 + 0xc) & 0x20000) == 0)) {
    if (param_2 != 0) {
      uVar1 = FUN_004c4600(local_40,param_1,param_2);
      FUN_0055bae0(param_1,uVar1,param_3);
      return;
    }
    FUN_0055bae0(param_1,param_1,param_3);
    return;
  }
  FUN_0055bae0(param_1,param_2,param_3);
  return;
}

