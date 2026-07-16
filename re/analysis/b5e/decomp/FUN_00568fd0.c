
/* [C1 2026-05-19] Signature: `int *FUN_00568fd0(int *param_1, int param_2)`. */

int * FUN_00568fd0(int *param_1,int param_2)

{
  ushort uVar1;
  ushort uVar2;
  float *pfVar3;
  float fVar4;
  int *piVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int local_8;
  uint local_4;

  piVar5 = param_1;
  local_4 = 0;
  if (*(int *)(param_2 + 4) == 0) {
    return param_1;
  }
  local_8 = 0;
  do {
    iVar6 = *(int *)(param_2 + 0x10) + local_8;
    if ((*(byte *)(iVar6 + 0x18) & 3) == 0) {
      for (pfVar3 = *(float **)(iVar6 + 4); pfVar3 != (float *)0x0; pfVar3 = (float *)pfVar3[0x37])
      {
        uVar1 = *(ushort *)(pfVar3 + 0x2e);
        param_1 = (int *)0xff7fffff;
        pfVar3[0x34] = 0.0;
        if (uVar1 == 0xffff) {
          uVar7 = 0xffff;
        }
        else {
          uVar7 = (uint)*(ushort *)((int)pfVar3[0x2d] + 0x20);
        }
        uVar2 = *(ushort *)(pfVar3 + 0x30);
        if (uVar2 == 0xffff) {
          uVar8 = 0xffff;
        }
        else {
          uVar8 = (uint)*(ushort *)((int)pfVar3[0x2f] + 0x20);
        }
        for (iVar6 = *(int *)(*piVar5 +
                             ((uVar7 * 0x20 + (uint)uVar1) * 0x3ffd + uVar8 * 0x20 + (uint)uVar2 &
                             piVar5[1] - 1U) * 4); iVar6 != 0; iVar6 = *(int *)(iVar6 + 0x24)) {
          if ((((*(ushort *)(iVar6 + 4) == uVar1) && (*(ushort *)(iVar6 + 0xc) == uVar2)) ||
              ((*(ushort *)(iVar6 + 4) == uVar2 && (*(ushort *)(iVar6 + 0xc) == uVar1)))) &&
             (fVar4 = *(float *)(iVar6 + 0x1c) * pfVar3[2] +
                      *(float *)(iVar6 + 0x14) * *pfVar3 + *(float *)(iVar6 + 0x18) * pfVar3[1],
             (float)param_1 < fVar4)) {
            pfVar3[0x34] = *(float *)(iVar6 + 0x20);
            param_1 = (int *)fVar4;
          }
        }
      }
    }
    local_4 = local_4 + 1;
    local_8 = local_8 + 0x28;
  } while (local_4 < *(uint *)(param_2 + 4));
  return piVar5;
}

