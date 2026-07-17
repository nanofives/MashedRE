
/* [C1 2026-05-19] Signature: `void FUN_0056cf90(undefined4 *param_1, int param_2, int param_3)`. */

void FUN_0056cf90(undefined4 *param_1,int param_2,int param_3)

{
  int iVar1;

  iVar1 = param_3 * 0x20 + param_2;
  *param_1 = *(undefined4 *)(param_3 * 0x20 + param_2);
  param_1[1] = *(undefined4 *)((param_3 + 2) * 0x20 + param_2);
  param_1[2] = *(undefined4 *)((param_3 + 4) * 0x20 + param_2);
  param_1[3] = *(undefined4 *)((param_3 + 6) * 0x20 + param_2);
  param_1[4] = *(undefined4 *)(iVar1 + 4);
  param_1[5] = *(undefined4 *)(iVar1 + 0x44);
  param_1[6] = *(undefined4 *)(iVar1 + 0x84);
  param_1[7] = *(undefined4 *)(iVar1 + 0xc4);
  param_1[8] = *(undefined4 *)(iVar1 + 8);
  param_1[9] = *(undefined4 *)(iVar1 + 0x48);
  param_1[10] = *(undefined4 *)(iVar1 + 0x88);
  param_1[0xb] = *(undefined4 *)(iVar1 + 200);
  param_1[0xc] = *(undefined4 *)(iVar1 + 0x10);
  param_1[0xd] = *(undefined4 *)(iVar1 + 0x50);
  param_1[0xe] = *(undefined4 *)(iVar1 + 0x90);
  param_1[0xf] = *(undefined4 *)(iVar1 + 0xd0);
  param_1[0x10] = *(undefined4 *)(iVar1 + 0x14);
  param_1[0x11] = *(undefined4 *)(iVar1 + 0x54);
  param_1[0x12] = *(undefined4 *)(iVar1 + 0x94);
  param_1[0x13] = *(undefined4 *)(iVar1 + 0xd4);
  param_1[0x14] = *(undefined4 *)(iVar1 + 0x18);
  param_1[0x15] = *(undefined4 *)(iVar1 + 0x58);
  param_1[0x16] = *(undefined4 *)(iVar1 + 0x98);
  param_1[0x17] = *(undefined4 *)(iVar1 + 0xd8);
  return;
}

