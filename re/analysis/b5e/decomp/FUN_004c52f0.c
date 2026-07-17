
/* [C1 2026-05-13] Ghidra comment confirms: `RwMatrixCombine(dst, src, combineOp)` — combines two
   RW 4×4 matrices. */

uint * FUN_004c52f0(uint *param_1,uint *param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  undefined4 local_48;
  undefined4 local_44;
  uint local_40 [16];

  if (param_3 == 0) {
    puVar4 = param_1;
    for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *param_2;
      param_2 = param_2 + 1;
      puVar4 = puVar4 + 1;
    }
    return param_1;
  }
  if (param_3 == 1) {
    uVar3 = param_2[3];
    uVar6 = param_1[3];
    uVar1 = *(uint *)(DAT_007d4028 + 4 + DAT_007d3ff8) & 0x20000;
    if ((uVar3 & uVar1) != 0) {
      puVar4 = param_1;
      puVar5 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      puVar4 = local_40;
      puVar5 = param_1;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
    puVar4 = param_2;
    puVar5 = param_1;
    if ((uVar6 & uVar1) != 0) {
      puVar4 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar4 = *param_2;
        param_2 = param_2 + 1;
        puVar4 = puVar4 + 1;
      }
      puVar4 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
  }
  else {
    if (param_3 != 2) {
      local_48 = 1;
      local_44 = FUN_004d7ff0(0x80000003,s_Invalid_combination_type_0061811c);
      FUN_004d8480(&local_48);
      return (uint *)0x0;
    }
    uVar3 = param_1[3];
    uVar6 = param_2[3];
    uVar1 = *(uint *)(DAT_007d4028 + 4 + DAT_007d3ff8) & 0x20000;
    if ((uVar3 & uVar1) != 0) {
      puVar4 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar4 = *param_2;
        param_2 = param_2 + 1;
        puVar4 = puVar4 + 1;
      }
      puVar4 = local_40;
      puVar5 = param_1;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
    puVar4 = param_1;
    puVar5 = param_2;
    if ((uVar6 & uVar1) != 0) {
      puVar5 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      puVar4 = local_40;
      puVar5 = param_1;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
  }
  (**(code **)(DAT_007d4028 + 8 + DAT_007d3ff8))(local_40,puVar4,puVar5);
  local_40[3] = uVar6 & uVar3;
  puVar4 = local_40;
  puVar5 = param_1;
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar5 = *puVar4;
    puVar4 = puVar4 + 1;
    puVar5 = puVar5 + 1;
  }
  return param_1;
}

