
/* [C1 2026-05-19] If `(*(byte *)(*(int *)(param_1 + 0x5c) + 0x40) & 2) == 0` AND `(*(uint
   *)(param_1 + 0xc) & 0x20000) == 0` AND `param_2… */

void FUN_0055bd80(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined1 local_40 [64];

  iVar1 = param_2;
  if ((((*(byte *)(*(int *)(param_1 + 0x5c) + 0x40) & 2) == 0) &&
      ((*(uint *)(param_1 + 0xc) & 0x20000) == 0)) && (iVar1 = param_1, param_2 != 0)) {
    iVar1 = FUN_004c4600(local_40,param_1,param_2);
  }
  (**(code **)(*(int *)(param_1 + 0x5c) + 0x10))(param_1,iVar1,param_3,param_4);
  return;
}

