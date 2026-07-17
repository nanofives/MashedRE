
/* [C1 2026-05-19] Inserts pair `(param_2, param_3)` into a hash-bucketed doubly-linked-list. */

void FUN_00563e70(int param_1,ushort param_2,ushort param_3)

{
  ushort uVar1;
  ushort uVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  int iVar6;

  uVar1 = *(ushort *)(param_1 + 0xc);
  if (uVar1 != 0xffff) {
    iVar4 = *(int *)(param_1 + 8);
    iVar5 = (uint)uVar1 * 8;
    uVar2 = *(ushort *)(iVar4 + 2 + iVar5);
    *(ushort *)(param_1 + 0xc) = uVar2;
    iVar6 = (uint)uVar2 * 8;
    *(undefined2 *)(param_1 + 0xc) = *(undefined2 *)(iVar6 + 2 + iVar4);
    *(ushort *)(iVar4 + 4 + iVar5) = param_3;
    *(ushort *)(*(int *)(param_1 + 8) + 6 + iVar5) = uVar2;
    *(ushort *)(iVar6 + 4 + *(int *)(param_1 + 8)) = param_2;
    *(ushort *)(iVar6 + 6 + *(int *)(param_1 + 8)) = uVar1;
    uVar3 = *(ushort *)((uint)param_2 * 8 + 2 + *(int *)(param_1 + 8));
    *(ushort *)(*(int *)(param_1 + 8) + 2 + iVar5) = uVar3;
    *(ushort *)(iVar5 + *(int *)(param_1 + 8)) = param_2;
    *(ushort *)(*(int *)(param_1 + 8) + (uint)uVar3 * 8) = uVar1;
    *(ushort *)(*(int *)(param_1 + 8) + 2 + (uint)param_2 * 8) = uVar1;
    uVar1 = *(ushort *)((uint)param_3 * 8 + 2 + *(int *)(param_1 + 8));
    *(ushort *)(iVar6 + 2 + *(int *)(param_1 + 8)) = uVar1;
    *(ushort *)(iVar6 + *(int *)(param_1 + 8)) = param_3;
    *(ushort *)(*(int *)(param_1 + 8) + (uint)uVar1 * 8) = uVar2;
    *(ushort *)(*(int *)(param_1 + 8) + 2 + (uint)param_3 * 8) = uVar2;
  }
  return;
}

