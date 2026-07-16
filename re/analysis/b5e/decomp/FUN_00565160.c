
/* [C1 2026-05-19] Used by FUN_00564c80 octree-split (per `0x00564c80.md` callees list — one of
   the three octant-relocate helpers). */

void FUN_00565160(int param_1,uint param_2,uint param_3)

{
  uint *puVar1;
  int iVar2;

  iVar2 = (param_2 & 0xffff) + 0x79b;
  puVar1 = (uint *)(param_1 + iVar2 * 0x14);
  *(ushort *)(param_1 + 0x8820 + (param_3 & 0xffff) * 4) =
       *(ushort *)(param_1 + iVar2 * 0x14) & 0x3ff;
  *puVar1 = *puVar1 & 0xfffffc00 | param_3 & 0xffff;
  return;
}

