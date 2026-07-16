
/* [C1 2026-05-19] Bucket-clear: removes all nodes in the linked-list chain headed by `param_2`'s
   bucket and returns them to the free-list… */

void FUN_00563f60(int param_1,ushort param_2)

{
  ushort uVar1;
  ushort uVar2;
  ushort uVar3;
  ushort uVar4;
  int iVar5;
  int iVar6;
  int iVar7;

  iVar5 = *(int *)(param_1 + 8);
  uVar1 = *(ushort *)(iVar5 + 2 + (uint)param_2 * 8);
  while (uVar1 != param_2) {
    iVar6 = (uint)uVar1 * 8;
    uVar2 = *(ushort *)(iVar6 + 2 + iVar5);
    uVar3 = *(ushort *)(iVar6 + 6 + iVar5);
    uVar4 = *(ushort *)(iVar6 + iVar5);
    *(ushort *)(iVar5 + (uint)uVar2 * 8) = uVar4;
    *(ushort *)(*(int *)(param_1 + 8) + 2 + (uint)uVar4 * 8) = uVar2;
    iVar5 = *(int *)(param_1 + 8);
    iVar7 = (uint)uVar3 * 8;
    uVar2 = *(ushort *)(iVar7 + 2 + iVar5);
    uVar4 = *(ushort *)(iVar7 + iVar5);
    *(ushort *)(iVar5 + (uint)uVar2 * 8) = uVar4;
    *(ushort *)(*(int *)(param_1 + 8) + 2 + (uint)uVar4 * 8) = uVar2;
    *(undefined2 *)(*(int *)(param_1 + 8) + 2 + iVar6) = *(undefined2 *)(param_1 + 0xc);
    *(ushort *)(*(int *)(param_1 + 8) + 2 + iVar7) = uVar1;
    *(ushort *)(param_1 + 0xc) = uVar3;
    iVar5 = *(int *)(param_1 + 8);
    uVar1 = *(ushort *)(iVar5 + 2 + (uint)param_2 * 8);
  }
  return;
}

