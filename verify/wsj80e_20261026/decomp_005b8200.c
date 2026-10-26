
/* [C1 2026-05-20] LIBRARY RESIDUE — D3DX9 / FX framework (effect-table assembly)**. */

int * FUN_005b8200(int *param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int *piVar7;
  int iStack_8;
  uint uStack_4;
  
  piVar2 = param_1;
  piVar7 = param_1 + 0xb;
  if (*param_1 != 0) {
    FUN_005ae010(param_1,piVar7);
    piVar7 = param_1 + 0xf;
  }
  if (param_1[1] != 0) {
    FUN_005adfe0(param_1,piVar7);
    iVar3 = (**(code **)(DAT_007d3ff8 + 0xf4))(piVar7);
    piVar7 = (int *)((int)piVar7 + (iVar3 + DAT_00914700 & ~(DAT_00914700 - 1U)));
  }
  iVar3 = param_1[5];
  param_1[4] = (int)piVar7;
  piVar7 = piVar7 + param_1[3] * 8;
  uVar5 = 0;
  if (param_1[3] != 0) {
    iVar6 = 0;
    do {
      if (*(int *)(param_1[4] + iVar6 + 0xc) != 0) {
        *(int **)(param_1[4] + iVar6 + 0xc) = piVar7;
        piVar7 = piVar7 + iVar3;
      }
      uVar5 = uVar5 + 1;
      iVar6 = iVar6 + 0x20;
    } while (uVar5 < (uint)param_1[3]);
  }
  uVar5 = 0;
  if (param_1[3] != 0) {
    iVar3 = 0;
    do {
      if (*(int *)(param_1[4] + iVar3) != 0) {
        FUN_005ae010((int *)(param_1[4] + iVar3),piVar7);
        piVar7 = piVar7 + 4;
      }
      uVar5 = uVar5 + 1;
      iVar3 = iVar3 + 0x20;
    } while (uVar5 < (uint)param_1[3]);
  }
  uVar5 = 0;
  if (param_1[3] != 0) {
    iVar3 = 0;
    do {
      if (*(int *)(iVar3 + 4 + param_1[4]) != 0) {
        FUN_005adfe0(iVar3 + param_1[4],piVar7);
        iVar6 = (**(code **)(DAT_007d3ff8 + 0xf4))(piVar7);
        piVar7 = (int *)((int)piVar7 + (iVar6 + DAT_00914700 & ~(DAT_00914700 - 1U)));
      }
      uVar5 = uVar5 + 1;
      iVar3 = iVar3 + 0x20;
    } while (uVar5 < (uint)param_1[3]);
  }
  iVar3 = 0;
  uVar5 = 0;
  if (param_1[3] != 0) {
    do {
      iVar6 = param_1[4] + iVar3;
      if (*(int *)(iVar6 + 0x14) != 0) {
        *(int **)(iVar6 + 0x14) = piVar7;
        piVar7 = piVar7 + *(int *)(iVar6 + 0x10) * 5;
      }
      uVar5 = uVar5 + 1;
      iVar3 = iVar3 + 0x20;
    } while (uVar5 < (uint)param_1[3]);
  }
  piVar1 = param_1 + 3;
  param_1 = (int *)0x0;
  if (*piVar1 != 0) {
    iStack_8 = 0;
    do {
      iVar3 = piVar2[4] + iStack_8;
      uStack_4 = 0;
      if (*(int *)(iVar3 + 0x10) != 0) {
        iVar6 = 0;
        do {
          if (*(int *)(*(int *)(iVar3 + 0x14) + iVar6) != 0) {
            FUN_005ae010(*(int *)(iVar3 + 0x14) + iVar6,piVar7);
            piVar7 = piVar7 + 4;
          }
          uStack_4 = uStack_4 + 1;
          iVar6 = iVar6 + 0x14;
        } while (uStack_4 < *(uint *)(iVar3 + 0x10));
      }
      param_1 = (int *)((int)param_1 + 1);
      iStack_8 = iStack_8 + 0x20;
    } while (param_1 < (uint)piVar2[3]);
  }
  param_1 = (int *)0x0;
  if (piVar2[3] != 0) {
    iStack_8 = 0;
    do {
      iVar3 = piVar2[4] + iStack_8;
      uStack_4 = 0;
      if (*(int *)(iVar3 + 0x10) != 0) {
        iVar6 = 0;
        do {
          if (*(int *)(*(int *)(iVar3 + 0x14) + 4 + iVar6) != 0) {
            FUN_005adfe0(*(int *)(iVar3 + 0x14) + iVar6,piVar7);
            iVar4 = (**(code **)(DAT_007d3ff8 + 0xf4))(piVar7);
            piVar7 = (int *)((int)piVar7 + (iVar4 + DAT_00914700 & ~(DAT_00914700 - 1U)));
          }
          uStack_4 = uStack_4 + 1;
          iVar6 = iVar6 + 0x14;
        } while (uStack_4 < *(uint *)(iVar3 + 0x10));
      }
      param_1 = (int *)((int)param_1 + 1);
      iStack_8 = iStack_8 + 0x20;
    } while (param_1 < (uint)piVar2[3]);
  }
  uVar5 = 0;
  piVar2[6] = (int)piVar7;
  piVar7 = piVar7 + piVar2[5] * 10;
  if (piVar2[5] != 0) {
    iVar3 = 0;
    do {
      if (*(int *)(piVar2[6] + 0x14 + iVar3) != 0) {
        *(int **)(piVar2[6] + 0x14 + iVar3) = piVar7;
        iVar6 = FUN_005acaa0(piVar7,piVar7,0,0);
        if (iVar6 == 0) {
          return (int *)0x0;
        }
        iVar6 = FUN_005aca80(piVar7);
        piVar7 = (int *)((int)piVar7 + (iVar6 + -1 + DAT_00914700 & ~(DAT_00914700 - 1U)));
      }
      uVar5 = uVar5 + 1;
      iVar3 = iVar3 + 0x28;
    } while (uVar5 < (uint)piVar2[5]);
  }
  iVar3 = FUN_005adf30(*(undefined4 *)(*(int *)(piVar2[6] + 0x14) + 4),&DAT_005e6454);
  if (((iVar3 == 0) ||
      (iVar3 = FUN_005adf30(*(undefined4 *)(*(int *)(piVar2[6] + 0x14) + 4),&DAT_005e64b4),
      iVar3 == 0)) && (*(char *)(*(int *)(piVar2[6] + 0x14) + 0xd) == '\x02')) {
    *(undefined4 *)(piVar2[6] + 0xc) = 0x40;
    *(undefined2 *)(piVar2[6] + 0x1a) = 0x24;
    *(undefined2 *)(piVar2[6] + 0x18) = 4;
  }
  uVar5 = 0;
  if (piVar2[5] != 0) {
    iVar3 = 0;
    do {
      if (*(int *)(iVar3 + piVar2[6]) != 0) {
        FUN_005ae010(iVar3 + piVar2[6],piVar7);
        piVar7 = piVar7 + 4;
      }
      uVar5 = uVar5 + 1;
      iVar3 = iVar3 + 0x28;
    } while (uVar5 < (uint)piVar2[5]);
  }
  uVar5 = 0;
  if (piVar2[5] != 0) {
    iVar3 = 0;
    do {
      if (*(int *)(iVar3 + 4 + piVar2[6]) != 0) {
        FUN_005adfe0(iVar3 + piVar2[6],piVar7);
        iVar6 = (**(code **)(DAT_007d3ff8 + 0xf4))(piVar7);
        piVar7 = (int *)((int)piVar7 + (iVar6 + DAT_00914700 & ~(DAT_00914700 - 1U)));
      }
      uVar5 = uVar5 + 1;
      iVar3 = iVar3 + 0x28;
    } while (uVar5 < (uint)piVar2[5]);
  }
  return piVar2;
}


