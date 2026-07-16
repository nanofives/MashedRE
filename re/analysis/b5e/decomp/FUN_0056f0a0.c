
/* [C1 2026-05-19] Signature: `void FUN_0056f0a0(int param_1)`. */

void FUN_0056f0a0(int param_1)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;

  uVar2 = *(uint *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4);
  uVar4 = uVar2 + 3 & 0xfffffffc;
  if (uVar2 != uVar4) {
    iVar3 = uVar4 - uVar2;
    do {
      FUN_0056f1f0(param_1,&DAT_005e5738);
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  *(uint *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4) =
       *(int *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4) + (uVar2 - uVar4);
  *(uint *)(param_1 + 4) = *(int *)(param_1 + 4) + uVar4;
  iVar3 = (int)(uVar4 + ((int)(uVar2 + 3) >> 0x1f & 3U)) >> 2;
  *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + iVar3;
  *(int *)(param_1 + 0x20) = *(int *)(param_1 + 0x20) + iVar3;
  *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + iVar3;
  *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + iVar3;
  *(int *)(param_1 + 0x44) = *(int *)(param_1 + 0x44) + iVar3;
  *(int *)(param_1 + 0x50) = *(int *)(param_1 + 0x50) + iVar3;
  *(int *)(param_1 + 0x5c) = *(int *)(param_1 + 0x5c) + iVar3;
  *(int *)(param_1 + 0x8c) = *(int *)(param_1 + 0x8c) + iVar3;
  *(int *)(param_1 + 0xb0) = *(int *)(param_1 + 0xb0) + 1;
  *(int *)(param_1 + 0x68) = *(int *)(param_1 + 0x68) + iVar3;
  *(uint *)(param_1 + 0x98) = *(int *)(param_1 + 0x98) + uVar4;
  *(uint *)(*(int *)(param_1 + 0xdc) + *(int *)(param_1 + 0xe0) * 4) =
       *(int *)(*(int *)(param_1 + 0xdc) + *(int *)(param_1 + 0xe0) * 4) + uVar4;
  *(uint *)(*(int *)(param_1 + 0xd0) + *(int *)(param_1 + 0xd4) * 4) =
       *(int *)(*(int *)(param_1 + 0xd0) + *(int *)(param_1 + 0xd4) * 4) + uVar2;
  piVar1 = (int *)(*(int *)(param_1 + 0xc4) + *(int *)(param_1 + 0xd4) * 4);
  *piVar1 = *piVar1 + 1;
  *(uint *)(param_1 + 0x100) = *(int *)(param_1 + 0x100) + uVar2;
  *(uint *)(param_1 + 0x104) = *(int *)(param_1 + 0x104) + uVar2;
  *(uint *)(param_1 + 0x108) = *(int *)(param_1 + 0x108) + uVar4;
  *(int *)(param_1 + 0xf8) = *(int *)(param_1 + 0xf8) + 1;
  return;
}

