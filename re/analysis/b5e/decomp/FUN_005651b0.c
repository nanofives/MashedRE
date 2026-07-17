
/* [C1 2026-05-19] Octant-table primitive-count incrementer (matches FUN_00564c80 split-trigger
   logic). */

void FUN_005651b0(int param_1,uint param_2,uint param_3)

{
  int iVar1;
  ushort uVar2;

  iVar1 = (param_3 & 0xffff) + (param_2 & 0xffff) * 10;
  uVar2 = *(ushort *)(param_1 + 0x9820 + iVar1 * 2);
  if ((uVar2 & 0x7c00) != 0x7c00) {
    *(ushort *)(param_1 + 0x9820 + iVar1 * 2) = uVar2 + 0x400;
  }
  return;
}

