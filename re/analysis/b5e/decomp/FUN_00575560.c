
/* [C1 2026-05-20] Signature recovered: `int FUN_00575560(int world, int pair_array, uint
   pair_count, int manifold_pool,… */

int FUN_00575560(int param_1,uint param_2,uint param_3,int param_4,int param_5)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int local_8;
  uint local_4;

  iVar4 = param_2;
  iVar3 = param_1;
  uVar6 = 0;
  if (param_3 != 0) {
    if (*(short *)(param_2 + 0x5c) == -1) {
      local_4 = 0;
    }
    else {
      local_4 = *(uint *)(*(int *)(param_2 + 0x58) + 8);
    }
    if (*(short *)(param_2 + 0xdc) == -1) {
      param_2 = 0;
    }
    else {
      param_2 = *(uint *)(*(int *)(param_2 + 0xd8) + 8);
    }
    if (((local_4 & 0x20) == 0) && ((param_2 & 0x20) == 0)) {
      local_8 = param_4 + 0xe0;
      uVar7 = 0;
      if (param_3 != 0) {
        param_1 = iVar4;
        do {
          if (param_5 - 1U <= uVar7) break;
          iVar5 = FUN_005752b0(param_1,*(undefined4 *)(iVar3 + 0x70),local_8);
          if (iVar5 != 0) {
            uVar7 = uVar7 + 1;
            local_8 = local_8 + 0xe0;
          }
          uVar6 = uVar6 + 1;
          param_1 = param_1 + 0x120;
        } while (uVar6 < param_3);
      }
      param_3 = 0;
      if (uVar7 != 0) {
        param_3 = FUN_00578ff0(param_4 + 0xe0,uVar7,*(undefined4 *)(iVar3 + 0x7c),param_4,param_5,
                               *(undefined4 *)(iVar3 + 0x74));
      }
      if ((((param_2 | local_4) & 0x40) != 0) && (param_3 != 0)) {
        iVar5 = param_4;
        param_4 = param_3;
        do {
          if ((local_4 & 0x40) != 0) {
            uVar6 = *(uint *)(iVar3 + 0x68);
            iVar2 = *(int *)(iVar4 + 0x58);
            if (uVar6 < *(uint *)(iVar3 + 0x6c)) {
              piVar1 = (int *)(*(int *)(iVar3 + 100) + uVar6 * 0x14);
              *(uint *)(iVar3 + 0x68) = uVar6 + 1;
              *piVar1 = iVar2;
              piVar1[1] = *(int *)(iVar4 + 0xd8);
              piVar1[2] = *(int *)(iVar4 + 0xdc);
              piVar1[3] = iVar5;
              piVar1[4] = *(int *)(iVar2 + 0x2c);
              *(int **)(iVar2 + 0x2c) = piVar1;
            }
          }
          if ((param_2 & 0x40) != 0) {
            uVar6 = *(uint *)(iVar3 + 0x68);
            iVar2 = *(int *)(iVar4 + 0xd8);
            if (uVar6 < *(uint *)(iVar3 + 0x6c)) {
              piVar1 = (int *)(*(int *)(iVar3 + 100) + uVar6 * 0x14);
              *(uint *)(iVar3 + 0x68) = uVar6 + 1;
              *piVar1 = iVar2;
              piVar1[1] = *(int *)(iVar4 + 0x58);
              piVar1[2] = *(int *)(iVar4 + 0x5c);
              piVar1[3] = iVar5;
              piVar1[4] = *(int *)(iVar2 + 0x2c);
              *(int **)(iVar2 + 0x2c) = piVar1;
            }
          }
          iVar5 = iVar5 + 0xe0;
          param_4 = param_4 + -1;
        } while (param_4 != 0);
      }
      return param_3;
    }
    if ((local_4 & 0x40) != 0) {
      uVar6 = *(uint *)(param_1 + 0x68);
      iVar3 = *(int *)(iVar4 + 0x58);
      if (uVar6 < *(uint *)(param_1 + 0x6c)) {
        piVar1 = (int *)(*(int *)(param_1 + 100) + uVar6 * 0x14);
        *(uint *)(param_1 + 0x68) = uVar6 + 1;
        *piVar1 = iVar3;
        piVar1[1] = *(int *)(iVar4 + 0xd8);
        piVar1[2] = *(int *)(iVar4 + 0xdc);
        piVar1[3] = 0;
        piVar1[4] = *(int *)(iVar3 + 0x2c);
        *(int **)(iVar3 + 0x2c) = piVar1;
      }
    }
    if ((param_2 & 0x40) != 0) {
      uVar6 = *(uint *)(param_1 + 0x68);
      iVar3 = *(int *)(iVar4 + 0xd8);
      if (uVar6 < *(uint *)(param_1 + 0x6c)) {
        piVar1 = (int *)(*(int *)(param_1 + 100) + uVar6 * 0x14);
        *(uint *)(param_1 + 0x68) = uVar6 + 1;
        *piVar1 = iVar3;
        piVar1[1] = *(int *)(iVar4 + 0x58);
        piVar1[2] = *(int *)(iVar4 + 0x5c);
        piVar1[3] = 0;
        piVar1[4] = *(int *)(iVar3 + 0x2c);
        *(int **)(iVar3 + 0x2c) = piVar1;
      }
    }
  }
  return 0;
}

