
/* [C1 2026-05-19] Signature: `void FUN_0056f1f0(int *param_1, undefined4 *param_2)`. */

void FUN_0056f1f0(int *param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  undefined4 *puVar6;

  puVar6 = (undefined4 *)(param_1[0x2b] + param_1[0x3e] * 8);
  uVar2 = *puVar6;
  uVar3 = puVar6[1];
  uVar4 = param_1[0x42] + *(int *)(param_1[0x2e] + param_1[0x3e] * 4);
  iVar1 = (uVar4 & 3) + ((int)uVar4 >> 2) * 4;
  *(undefined4 *)(param_1[10] + iVar1 * 4) = param_2[0x10];
  *(undefined4 *)(param_1[7] + iVar1 * 4) = param_2[0x11];
  *(undefined4 *)(param_1[0x19] + iVar1 * 4) = param_2[0x12];
  *(undefined4 *)(param_1[4] + iVar1 * 4) = param_2[0x13];
  *(undefined4 *)(param_1[0xd] + iVar1 * 4) = param_2[0x14];
  *(undefined4 *)(param_1[0x10] + iVar1 * 4) = param_2[0x15];
  *(undefined4 *)(param_1[0x13] + iVar1 * 4) = param_2[0x16];
  *(undefined4 *)(param_1[0x16] + iVar1 * 4) = param_2[0x17];
  *(undefined4 *)(param_1[0x22] + iVar1 * 4) = param_2[0x18];
  iVar5 = iVar1 * 0x40;
  puVar6 = (undefined4 *)(*param_1 + iVar5);
  *puVar6 = *param_2;
  puVar6[1] = param_2[1];
  puVar6[2] = param_2[2];
  puVar6 = (undefined4 *)(iVar5 + 0x10 + *param_1);
  *puVar6 = param_2[4];
  puVar6[1] = param_2[5];
  puVar6[2] = param_2[6];
  *(undefined4 *)(iVar5 + 0xc + *param_1) = uVar2;
  *(int *)(iVar5 + 0x1c + *param_1) = param_1[0x3e];
  puVar6 = (undefined4 *)(iVar5 + 0x20 + *param_1);
  *puVar6 = param_2[8];
  puVar6[1] = param_2[9];
  puVar6[2] = param_2[10];
  puVar6 = (undefined4 *)(iVar5 + 0x30 + *param_1);
  *puVar6 = param_2[0xc];
  puVar6[1] = param_2[0xd];
  puVar6[2] = param_2[0xe];
  *(undefined4 *)(iVar5 + 0x2c + *param_1) = uVar3;
  *(int *)(iVar5 + 0x3c + *param_1) = param_1[0x3e];
  *(undefined4 *)(param_1[0x25] + iVar1 * 4) = param_2[0x1a];
  *(int *)(param_1[0x2e] + param_1[0x3e] * 4) = *(int *)(param_1[0x2e] + param_1[0x3e] * 4) + 1;
  return;
}

