
/* [C1 2026-05-19] Writes the low 6 bits (`& 0x03ff` mask applied via complement) of an octant-table
   entry at `this + 0x9820 + ((octant +… */

void FUN_00565120(int param_1,uint param_2,uint param_3,ushort param_4)

{
  int iVar1;

  iVar1 = (param_3 & 0xffff) + (param_2 & 0xffff) * 10;
  *(ushort *)(param_1 + 0x9820 + iVar1 * 2) =
       *(ushort *)(param_1 + 0x9820 + iVar1 * 2) & 0xfc00 | param_4;
  return;
}

