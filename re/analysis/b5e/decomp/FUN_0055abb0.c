
/* [C1 2026-05-19] Reads `uVar1 = *(ushort *)(param_2 + 0x20)` (cited 0x0055abb5) — shape index.
    */

undefined4 FUN_0055abb0(undefined4 *param_1,int param_2,undefined4 param_3)

{
  ushort uVar1;
  undefined4 uVar2;

  uVar1 = *(ushort *)(param_2 + 0x20);
  uVar2 = 0;
  if ((*(uint *)(param_1[0x1a] + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)) != 0) {
    FUN_00565200(*param_1,uVar1,param_3);
    uVar2 = param_3;
  }
  return uVar2;
}

