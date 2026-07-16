
/* [C1 2026-05-19] Outer driver that walks pair-record list (stride 0x28 starting at `*(this+0xd4)`,
   count `*(this+0xc8)`) and slices into… */

int FUN_00561040(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int *piVar5;
  int local_84;
  int local_80;
  undefined4 local_78;
  int *local_74;
  undefined4 local_70;
  int *local_6c;
  undefined4 local_68;
  int *local_64;
  undefined4 local_60;
  int *local_5c;
  undefined4 local_58;
  int *local_54;
  undefined4 local_50;
  int *local_4c;
  undefined4 local_48;
  int *local_44;
  undefined4 local_38;
  int *local_34;
  undefined4 local_30;
  int *local_2c;
  undefined4 local_28;
  int *local_24;
  int local_20;
  int local_1c;
  int local_18;
  undefined4 local_14;
  int local_10;
  int local_4;

  local_80 = 0;
  puVar1 = *(undefined4 **)(**(int **)(param_1 + 0x70) + 0x10);
  local_78 = *puVar1;
  puVar1[0x17] = 0;
  local_74 = &local_20;
  local_70 = puVar1[2];
  local_6c = &local_20;
  local_68 = puVar1[4];
  local_64 = &local_20;
  local_38 = puVar1[0x10];
  local_34 = &local_20;
  local_28 = puVar1[0x14];
  local_24 = &local_20;
  local_60 = puVar1[6];
  local_5c = &local_20;
  local_58 = puVar1[8];
  local_54 = &local_20;
  local_50 = puVar1[10];
  local_4c = &local_20;
  local_48 = puVar1[0xc];
  local_44 = &local_20;
  local_30 = puVar1[0x12];
  local_2c = &local_20;
  iVar4 = *(int *)(param_1 + 200);
  if (iVar4 != 0) {
    do {
      uVar3 = 0;
      local_84 = 0;
      if (local_80 == iVar4) {
        return 0;
      }
      local_4 = *(int *)(param_1 + 0xd4) + local_80 * 0x28;
      piVar5 = (int *)(local_4 + 0x1c);
      iVar2 = local_80;
      do {
        uVar3 = uVar3 + *piVar5;
        if (*(uint *)(param_1 + 0x48) < uVar3) break;
        piVar5 = piVar5 + 10;
        local_84 = local_84 + 1;
        iVar2 = iVar2 + 1;
      } while (iVar2 != iVar4);
      if (local_84 == 0) {
        return 0;
      }
      local_14 = *(undefined4 *)(param_1 + 0xc4);
      local_10 = local_84;
      local_20 = puVar1[0x16] + puVar1[0x17] * 4;
      local_18 = puVar1[0x18] - puVar1[0x17];
      FUN_00560260(&local_78,param_1 + 0x9c,param_1 + 0x168,param_1 + 0xfc,param_1 + 0x2c4,
                   param_1 + 0x364,param_1 + 0xa8,&local_14,param_1,
                   *(undefined4 *)(**(int **)(param_1 + 0x70) + 0xc),*(undefined4 *)(param_1 + 0xf4)
                   ,*(undefined4 *)(param_1 + 0xf8),*(undefined4 *)(param_1 + 0x404),param_1 + 0x408
                  );
      puVar1[0x17] = puVar1[0x17] + local_1c;
      iVar4 = *(int *)(param_1 + 200);
      local_80 = local_80 + local_84;
    } while (local_80 != iVar4);
  }
  return param_1;
}

