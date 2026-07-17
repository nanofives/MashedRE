
/* [C1 2026-05-19] Collects all unique pair-IDs that key `param_2` is hashed with, filtered by
   bitmap `param_4`. Stores into output buffer… */

void FUN_00564040(int param_1,ushort param_2,int param_3,int param_4,int param_5)

{
  ushort uVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  uint local_4;

  iVar3 = *(int *)(param_1 + 8);
  local_4 = 0;
  for (uVar1 = *(ushort *)(iVar3 + 2 + (uint)param_2 * 8); uVar1 != param_2;
      uVar1 = *(ushort *)((uint)uVar1 * 8 + 2 + iVar3)) {
    uVar2 = *(ushort *)((uint)uVar1 * 8 + 4 + iVar3);
    if ((*(uint *)(param_4 + (uint)(uVar2 >> 5) * 4) & 1 << ((byte)uVar2 & 0x1f)) == 0) {
      *(ushort *)(param_3 + local_4 * 2) = uVar2;
      local_4 = local_4 + 1;
    }
    iVar3 = *(int *)(param_1 + 8);
  }
  if (param_5 != 0) {
    while (1 < local_4) {
      local_4 = local_4 - 1;
      uVar4 = 0;
      if (local_4 != 0) {
        do {
          uVar1 = *(ushort *)(param_3 + uVar4 * 2);
          uVar2 = *(ushort *)(param_3 + 2 + uVar4 * 2);
          if (uVar2 < uVar1) {
            *(ushort *)(param_3 + uVar4 * 2) = uVar2;
            *(ushort *)(param_3 + 2 + uVar4 * 2) = uVar1;
          }
          uVar4 = uVar4 + 1;
        } while (uVar4 < local_4);
      }
    }
  }
  return;
}

