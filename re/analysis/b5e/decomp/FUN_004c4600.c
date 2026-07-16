
/* [C1 2026-05-07] RwMatrixMultiply */

undefined4 * FUN_004c4600(undefined4 *param_1,undefined4 *param_2,undefined4 *param_3)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;

  uVar1 = param_2[3];
  uVar2 = param_3[3];
  uVar3 = *(uint *)(DAT_007d4028 + 4 + DAT_007d3ff8) & 0x20000;
  if ((uVar1 & uVar3) != 0) {
    puVar5 = param_1;
    for (iVar4 = 0x10; iVar4 != 0; iVar4 = iVar4 + -1) {
      *puVar5 = *param_3;
      param_3 = param_3 + 1;
      puVar5 = puVar5 + 1;
    }
    return param_1;
  }
  if ((uVar2 & uVar3) != 0) {
    puVar5 = param_1;
    for (iVar4 = 0x10; iVar4 != 0; iVar4 = iVar4 + -1) {
      *puVar5 = *param_2;
      param_2 = param_2 + 1;
      puVar5 = puVar5 + 1;
    }
    return param_1;
  }
  (**(code **)(DAT_007d4028 + 8 + DAT_007d3ff8))(param_1,param_2,param_3);
  param_1[3] = uVar2 & uVar1;
  return param_1;
}

