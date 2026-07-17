
/* [C1 2026-05-19] Signature: `bool FUN_0056ba30(int param_1, int param_2)`. */

bool FUN_0056ba30(int param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;

  iVar2 = *(int *)(param_1 + 4);
  *(undefined4 *)(iVar2 + 0x84) = 0;
  if (*(uint *)(iVar2 + 0x90) < *(uint *)(iVar2 + 0x94)) {
    puVar1 = (undefined4 *)(*(int *)(iVar2 + 0xd4) + param_2 * 0x28);
    FUN_0056b7a0(param_1,*puVar1,puVar1[3],
                 *(undefined4 *)(*(int *)**(undefined4 **)(iVar2 + 0x70) + 0xc018));
    uVar3 = FUN_00568990(iVar2,*(undefined4 *)(iVar2 + 0x80),*(undefined4 *)(iVar2 + 0x84),
                         *(int *)(iVar2 + 0x90) * 0xe0 + *(int *)(iVar2 + 0x8c),
                         *(int *)(iVar2 + 0x94) - *(int *)(iVar2 + 0x90));
    if (uVar3 != 0) {
      uVar5 = 0;
      if (uVar3 != 0) {
        do {
          *(undefined4 *)((*(int *)(iVar2 + 0x90) + uVar5) * 0xe0 + 0xdc + *(int *)(iVar2 + 0x8c)) =
               puVar1[1];
          iVar4 = *(int *)(iVar2 + 0x90) + uVar5;
          uVar5 = uVar5 + 1;
          puVar1[1] = iVar4 * 0xe0 + *(int *)(iVar2 + 0x8c);
        } while (uVar5 < uVar3);
      }
      *(int *)(iVar2 + 0x90) = *(int *)(iVar2 + 0x90) + uVar3;
    }
    return uVar3 != 0;
  }
  return false;
}

