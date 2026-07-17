
/* [C1 2026-05-19] Signature: `int FUN_0056bb30(int param_1)`. */

int FUN_0056bb30(int param_1)

{
  int iVar1;
  uint uVar2;

  uVar2 = 0;
  if (*(int *)(param_1 + 200) != 0) {
    iVar1 = 0;
    do {
      if ((*(byte *)(*(int *)(param_1 + 0xd4) + 0x18 + iVar1) & 1) == 0) {
        FUN_0056ba30(*(undefined4 *)(param_1 + 0x70),uVar2);
      }
      uVar2 = uVar2 + 1;
      iVar1 = iVar1 + 0x28;
    } while (uVar2 < *(uint *)(param_1 + 200));
  }
  return param_1;
}

