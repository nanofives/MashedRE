
/* [C1 2026-05-19] Two-loop pass over solver context's body arrays. */

int FUN_0055fea0(int param_1)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;

  uVar6 = 0;
  if (*(int *)(param_1 + 0x78) != 0) {
    do {
      iVar5 = *(int *)(*(int *)(param_1 + 0x74) + uVar6 * 4);
      if (((1 < *(ushort *)(iVar5 + 0xc)) && ((*(byte *)(iVar5 + 8) & 0x10) != 0)) &&
         (uVar2 = *(uint *)(param_1 + 0x84), uVar2 < *(uint *)(param_1 + 0x88))) {
        piVar1 = (int *)(*(int *)(param_1 + 0x80) + uVar2 * 0x14);
        *(uint *)(param_1 + 0x84) = uVar2 + 1;
        *(undefined2 *)(piVar1 + 1) = 0x8000;
        *piVar1 = iVar5;
        *(undefined2 *)(piVar1 + 3) = 0x8000;
        piVar1[2] = iVar5;
        piVar1[4] = *(int *)(iVar5 + 0x1c);
      }
      uVar6 = uVar6 + 1;
    } while (uVar6 < *(uint *)(param_1 + 0x78));
  }
  uVar6 = 0;
  if (*(int *)(param_1 + 0x3e4) != 0) {
    iVar5 = 0;
    do {
      iVar3 = *(int *)(iVar5 + *(int *)(param_1 + 0x3e0));
      if (iVar3 != 0) {
        *(undefined4 *)(iVar3 + 0x2c) = 0;
      }
      uVar6 = uVar6 + 1;
      iVar5 = iVar5 + 0x14;
    } while (uVar6 < *(uint *)(param_1 + 0x3e4));
  }
  *(undefined4 *)(param_1 + 0x3e4) = 0;
  uVar4 = FUN_00568990(param_1,*(undefined4 *)(param_1 + 0x80),*(undefined4 *)(param_1 + 0x84),
                       *(undefined4 *)(param_1 + 0x8c),*(undefined4 *)(param_1 + 0x94));
  *(undefined4 *)(param_1 + 0x90) = uVar4;
  return param_1;
}

