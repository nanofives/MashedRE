
/* [C1 2026-05-19] Signature: `void FUN_00567c60(int param_1, int param_2, undefined4 *param_3, int
   param_4, int param_5, uint param_6… */

void FUN_00567c60(int param_1,int param_2,undefined4 *param_3,int param_4,int param_5,uint param_6,
                 undefined4 param_7,int param_8,int param_9,uint param_10)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int *piVar5;
  undefined4 *puVar6;
  uint uVar7;
  uint *puVar8;
  uint *puVar9;
  int iVar10;
  uint uVar11;
  uint *puVar12;
  int iVar13;
  uint uVar14;
  uint *puVar15;
  int local_10;

  iVar4 = param_1;
  *(undefined4 *)(param_2 + 4) = 0;
  puVar6 = param_3;
  for (iVar10 = param_4; iVar10 != 0; iVar10 = iVar10 + -1) {
    *puVar6 = 0;
    puVar6 = puVar6 + 4;
  }
  FUN_00567f00(param_2,param_3,param_4,param_5,param_6,param_7,param_8,param_9,param_10);
  iVar10 = *(int *)(param_2 + 4);
  param_4 = 0;
  param_9 = 0;
  if (iVar10 == 0) {
    param_2 = 0;
  }
  else {
    param_2 = param_2 + 8;
  }
  iVar13 = 0;
  if (iVar10 != 0) {
    param_8 = 0;
    puVar12 = (uint *)(param_2 + 0x10);
    local_10 = iVar10;
    do {
      uVar2 = puVar12[3];
      uVar11 = puVar12[-3];
      uVar3 = *puVar12;
      piVar5 = (int *)(*(int *)(param_1 + 0x10) + param_8);
      param_10 = 0;
      iVar13 = *(int *)(param_1 + 8) + param_4 * 4;
      *piVar5 = iVar13;
      uVar14 = puVar12[-2];
      uVar7 = 0;
      if (uVar11 != 0) {
        do {
          *(undefined4 *)(iVar13 + uVar7 * 4) = *(undefined4 *)(uVar14 + 0xc);
          iVar13 = *piVar5;
          uVar14 = *(uint *)(uVar14 + 8);
          param_10 = param_10 + *(ushort *)(*(int *)(iVar13 + uVar7 * 4) + 0xc);
          uVar7 = uVar7 + 1;
        } while (uVar7 < uVar11);
      }
      piVar5[3] = uVar11;
      param_4 = param_4 + uVar11;
      piVar5[5] = param_10;
      uVar11 = 0;
      piVar5[2] = *(int *)(param_1 + 0xc) + param_9 * 4;
      puVar8 = (uint *)puVar12[1];
      if (uVar3 != 0) {
        do {
          *(uint **)(piVar5[2] + uVar11 * 4) = puVar8 + -2;
          puVar8 = (uint *)*puVar8;
          uVar11 = uVar11 + 1;
        } while (uVar11 < uVar3);
      }
      piVar5[4] = uVar3;
      param_9 = param_9 + uVar3;
      puVar15 = (uint *)0x0;
      uVar11 = 0;
      piVar5[1] = 0;
      puVar8 = (uint *)puVar12[4];
      if (uVar2 != 0) {
        do {
          puVar9 = puVar8;
          if (uVar11 == 0) {
            piVar5[1] = (int)(puVar9 + -0x35);
          }
          else {
            puVar15[0x37] = (uint)(puVar9 + -0x35);
          }
          puVar15 = puVar9 + -0x35;
          uVar11 = uVar11 + 1;
          puVar8 = (uint *)*puVar9;
        } while (uVar11 < uVar2);
        if (puVar15 != (uint *)0x0) {
          puVar9[2] = 0;
        }
      }
      piVar5[6] = 0;
      puVar12 = puVar12 + 10;
      param_8 = param_8 + 0x28;
      local_10 = local_10 + -1;
      iVar13 = iVar10;
    } while (local_10 != 0);
  }
  *(int *)(param_1 + 4) = iVar13;
  param_10 = 0;
  if (param_6 != 0) {
    param_1 = param_4 << 2;
    iVar13 = iVar10 * 0x28;
    param_2 = iVar10;
    do {
      iVar10 = *(int *)(param_5 + param_10 * 4);
      uVar1 = *(ushort *)(iVar10 + 0x20);
      if (param_3[(uint)uVar1 * 4] == 0) {
        if ((*(uint *)(*(int *)(*(int *)(iVar10 + 0x24) + 0x60) + (uint)(uVar1 >> 5) * 4) &
            1 << ((byte)uVar1 & 0x1f)) != 0) {
          puVar6 = (undefined4 *)(*(int *)(iVar4 + 0x10) + iVar13);
          param_2 = param_2 + 1;
          iVar13 = iVar13 + 0x28;
          piVar5 = (int *)(*(int *)(iVar4 + 8) + param_1);
          param_1 = param_1 + 4;
          *puVar6 = piVar5;
          *piVar5 = iVar10;
          puVar6[3] = 1;
          puVar6[4] = 0;
          puVar6[1] = 0;
          uVar1 = *(ushort *)(*(int *)*puVar6 + 0xc);
          puVar6[6] = 0;
          puVar6[5] = (uint)uVar1;
        }
      }
      else {
        param_3[(uint)uVar1 * 4] = 0;
      }
      param_10 = param_10 + 1;
    } while (param_10 < param_6);
    *(int *)(iVar4 + 4) = param_2;
    return;
  }
  *(int *)(param_1 + 4) = iVar10;
  return;
}

