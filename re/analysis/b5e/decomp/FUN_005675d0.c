
/* [C1 2026-05-19] Signature: `void FUN_005675d0(int param_1, int param_2, int param_3, undefined4
   param_4)`. */

void FUN_005675d0(int param_1,int param_2,int param_3,undefined4 param_4)

{
  int iVar1;

  *(undefined4 *)(param_2 + 0x20) = 0;
  *(undefined4 *)(param_2 + 0x2c) = 0;
  *(undefined4 *)(param_2 + 0x38) = 0;
  *(undefined4 *)(param_2 + 0x14) = 0;
  *(undefined4 *)(param_2 + 0x44) = 0;
  *(undefined4 *)(param_2 + 8) = 0;
  *(undefined4 *)(param_2 + 0x50) = 0;
  *(undefined4 *)(param_2 + 0x74) = 0;
  *(undefined4 *)(param_2 + 0x68) = 0;
  iVar1 = (int)(param_3 + (param_3 >> 0x1f & 3U)) >> 2;
  *(undefined4 *)(param_2 + 0x7c) = param_4;
  *(int *)(param_2 + 0x8c) = iVar1;
  *(int *)(param_2 + 0x98) = iVar1;
  *(int *)(param_2 + 0x80) = iVar1 * 2;
  *(int *)(param_2 + 0x84) = iVar1;
  *(int *)(param_1 + 4) = iVar1;
  *(int *)(param_1 + 0x10) = iVar1;
  return;
}

