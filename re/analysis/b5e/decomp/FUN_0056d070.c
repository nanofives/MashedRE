
/* [C1 2026-05-19] Signature: `void FUN_0056d070(int *param_1, int param_2, uint param_3, int
   param_4, ..., int param_6, int param_7… */

void FUN_0056d070(int *param_1,int param_2,uint param_3,int param_4,undefined4 param_5,int param_6,
                 int param_7,undefined4 param_8,int *param_9,int param_10,int param_11)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int local_c;

  piVar2 = param_1;
  param_1[0x16] = (int)param_9;
  param_1[0x17] = param_10;
  param_1[0x18] = param_11;
  iVar3 = 0;
  if (param_4 != 0) {
    do {
      iVar3 = iVar3 + 1;
      *(undefined4 *)(param_1[0x10] + -4 + iVar3 * 4) = 0;
      *(undefined4 *)(param_1[1] + -4 + iVar3 * 4) = 0;
    } while (iVar3 != param_4);
  }
  param_1[0x11] = param_4;
  param_3 = param_3 >> 2;
  param_1[2] = param_4;
  if (param_3 != 0) {
    piVar5 = (int *)(param_2 + 0x2c);
    uVar7 = param_3;
    do {
      iVar3 = piVar5[-8];
      iVar6 = *piVar5;
      if (iVar3 != -1) {
        piVar1 = (int *)(param_1[0x10] + iVar3 * 4);
        *piVar1 = *piVar1 + 1;
        piVar1 = (int *)(param_1[1] + iVar3 * 4);
        *piVar1 = *piVar1 + 1;
      }
      if (iVar6 != -1) {
        *(int *)(param_1[0x10] + iVar6 * 4) = *(int *)(param_1[0x10] + iVar6 * 4) + 1;
        *(int *)(param_1[1] + iVar6 * 4) = *(int *)(param_1[1] + iVar6 * 4) + 1;
      }
      piVar5 = piVar5 + 0x40;
      uVar7 = uVar7 - 1;
    } while (uVar7 != 0);
  }
  iVar6 = 0;
  iVar3 = 0;
  if (param_10 != 0) {
    param_1 = (int *)param_10;
    do {
      iVar8 = 0;
      iVar4 = 0;
      if (*param_9 != 0) {
        do {
          *(int *)(piVar2[0xd] + iVar3 * 4) = iVar6;
          *(int *)(piVar2[4] + iVar3 * 4) = iVar4;
          iVar9 = *(int *)(piVar2[0x10] + iVar3 * 4);
          iVar6 = iVar6 + iVar9;
          iVar4 = iVar4 + iVar9;
          iVar3 = iVar3 + 1;
          iVar8 = iVar8 + 1;
        } while (iVar8 != *param_9);
      }
      param_9 = param_9 + 1;
      param_1 = (int *)((int)param_1 + -1);
    } while (param_1 != (int *)0x0);
  }
  iVar3 = 0;
  piVar2[0x21] = param_3;
  piVar2[0xe] = param_4;
  piVar2[5] = param_4;
  piVar2[0x20] = iVar6;
  piVar2[0x1f] = param_4;
  if (param_4 != 0) {
    do {
      iVar3 = iVar3 + 1;
      *(undefined4 *)(piVar2[0x10] + -4 + iVar3 * 4) = 0;
    } while (iVar3 != param_4);
  }
  iVar3 = 0;
  piVar2[0xb] = piVar2[0x20];
  piVar2[0x1d] = piVar2[0x21];
  piVar2[0x1a] = piVar2[0x21];
  local_c = 0;
  param_1 = (int *)0x0;
  if (param_7 != 0) {
    do {
      param_9 = (int *)0x0;
      *(undefined4 *)(piVar2[0x13] + (int)param_1 * 4) = 0;
      if ((*(uint *)(param_6 + (int)param_1 * 4) & 0xfffffffc) != 0) {
        iVar6 = iVar3 * 0x100 + param_2;
        do {
          iVar4 = *(int *)(iVar6 + 0xc);
          iVar8 = *(int *)(iVar6 + 0x2c);
          *(int *)(piVar2[0x19] + iVar3 * 8) = iVar4;
          *(int *)(piVar2[0x19] + 4 + iVar3 * 8) = iVar8;
          if (iVar4 != -1) {
            iVar9 = *(int *)(piVar2[0xd] + iVar4 * 4) + *(int *)(piVar2[0x10] + iVar4 * 4);
            FUN_0056cf90(iVar9 * 0x60 + *piVar2,iVar6,0);
            *(int *)(piVar2[0x1c] + iVar3 * 8) = iVar9 - local_c;
            *(int *)(piVar2[0x10] + iVar4 * 4) = *(int *)(piVar2[0x10] + iVar4 * 4) + 1;
            *(int *)(piVar2[10] + iVar9 * 4) = iVar3;
            *(int **)(piVar2[7] + iVar9 * 4) = param_9;
            *(int *)(piVar2[0x13] + (int)param_1 * 4) =
                 *(int *)(piVar2[0x13] + (int)param_1 * 4) + 1;
            piVar2[8] = piVar2[8] + 1;
          }
          if (iVar8 != -1) {
            iVar4 = *(int *)(piVar2[0xd] + iVar8 * 4) + *(int *)(piVar2[0x10] + iVar8 * 4);
            FUN_0056cf90(iVar4 * 0x60 + *piVar2,iVar6,1);
            *(int *)(piVar2[0x1c] + 4 + iVar3 * 8) = iVar4 - local_c;
            *(int *)(piVar2[0x10] + iVar8 * 4) = *(int *)(piVar2[0x10] + iVar8 * 4) + 1;
            *(int *)(piVar2[10] + iVar4 * 4) = iVar3;
            *(int **)(piVar2[7] + iVar4 * 4) = param_9;
            *(int *)(piVar2[0x13] + (int)param_1 * 4) =
                 *(int *)(piVar2[0x13] + (int)param_1 * 4) + 1;
            piVar2[8] = piVar2[8] + 1;
          }
          iVar3 = iVar3 + 1;
          iVar6 = iVar6 + 0x100;
          param_9 = (int *)((int)param_9 + 1);
        } while (param_9 != (int *)(*(uint *)(param_6 + (int)param_1 * 4) >> 2));
      }
      piVar2[0x14] = piVar2[0x14] + 1;
      local_c = local_c + *(int *)(piVar2[0x13] + (int)param_1 * 4);
      param_1 = (int *)((int)param_1 + 1);
    } while (param_1 != (int *)param_7);
  }
  return;
}

