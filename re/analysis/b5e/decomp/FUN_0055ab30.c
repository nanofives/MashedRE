
/* [C1 2026-05-19] Re-asserted by this bucket. Body of function unchanged vs prior plate: */

undefined4 * FUN_0055ab30(undefined4 *param_1,int param_2,int param_3)

{
  uint *puVar1;
  ushort uVar2;

  uVar2 = *(ushort *)(param_2 + 0x20);
  if ((*(uint *)(param_1[0x1a] + (uint)(uVar2 >> 5) * 4) & 1 << ((byte)uVar2 & 0x1f)) != 0) {
    FUN_00565260(*param_1,uVar2);
  }
  if (param_3 != 0) {
    FUN_00564c80(*param_1,*(undefined2 *)(param_2 + 0x20),param_3);
    puVar1 = (uint *)(param_1[0x1a] + (uint)(*(ushort *)(param_2 + 0x20) >> 5) * 4);
    *puVar1 = *puVar1 | 1 << ((byte)*(ushort *)(param_2 + 0x20) & 0x1f);
  }
  return param_1;
}

