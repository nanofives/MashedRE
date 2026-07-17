
/* [C1 2026-05-19] Signature: `int *FUN_00568dd0(int *param_1, int param_2)`. */

int * FUN_00568dd0(int *param_1,int param_2)

{
  float *pfVar1;
  ushort uVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  float *pfVar6;
  float fVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  float *pfVar11;
  float *pfVar12;
  int local_10;
  int local_c;
  uint local_8;

  uVar8 = 0;
  local_10 = 0;
  if (param_1[1] != 0) {
    do {
      uVar8 = uVar8 + 1;
      *(undefined4 *)(*param_1 + -4 + uVar8 * 4) = 0;
    } while (uVar8 < (uint)param_1[1]);
  }
  local_8 = 0;
  if (*(int *)(param_2 + 4) != 0) {
    local_c = 0;
    do {
      iVar9 = *(int *)(param_2 + 0x10) + local_c;
      if (((*(byte *)(iVar9 + 0x18) & 3) == 0) &&
         (pfVar11 = *(float **)(iVar9 + 4), pfVar11 != (float *)0x0)) {
        iVar9 = local_10 * 0x28;
        do {
          uVar2 = *(ushort *)(pfVar11 + 0x2e);
          pfVar12 = (float *)(param_1[3] + iVar9);
          local_10 = local_10 + 1;
          iVar9 = iVar9 + 0x28;
          if (uVar2 == 0xffff) {
            uVar8 = 0xffff;
          }
          else {
            uVar8 = (uint)*(ushort *)((int)pfVar11[0x2d] + 0x20);
          }
          uVar3 = *(ushort *)(pfVar11 + 0x30);
          if (uVar3 == 0xffff) {
            uVar10 = 0xffff;
          }
          else {
            uVar10 = (uint)*(ushort *)((int)pfVar11[0x2f] + 0x20);
          }
          iVar4 = *param_1;
          iVar5 = param_1[1];
          *pfVar12 = pfVar11[0x2d];
          pfVar1 = (float *)(iVar4 + ((uVar8 * 0x20 + (uint)uVar2) * 0x3ffd + uVar10 * 0x20 +
                                      (uint)uVar3 & iVar5 - 1U) * 4);
          pfVar12[1] = pfVar11[0x2e];
          pfVar12[2] = pfVar11[0x2f];
          pfVar12[3] = pfVar11[0x30];
          pfVar12[5] = *pfVar11;
          pfVar12[6] = pfVar11[1];
          pfVar12[7] = pfVar11[2];
          if (*(short *)(pfVar11 + 0x2e) == -1) {
LAB_00568f4e:
            fVar7 = pfVar11[0x2c];
            fVar7 = -(*(float *)((int)fVar7 + 0x28) * pfVar11[2] +
                     *(float *)((int)fVar7 + 0x20) * *pfVar11 +
                     *(float *)((int)fVar7 + 0x24) * pfVar11[1]);
          }
          else {
            iVar4 = *(int *)((int)pfVar11[0x2d] + 0x24);
            if ((iVar4 == 0) ||
               (uVar2 = *(ushort *)((int)pfVar11[0x2d] + 0x20),
               (*(uint *)(*(int *)(iVar4 + 0x60) + (uint)(uVar2 >> 5) * 4) &
               1 << ((byte)uVar2 & 0x1f)) == 0)) goto LAB_00568f4e;
            pfVar6 = (float *)pfVar11[0x2c];
            fVar7 = pfVar6[2] * pfVar11[2] + *pfVar6 * *pfVar11 + pfVar6[1] * pfVar11[1];
          }
          pfVar12[8] = fVar7;
          if (pfVar12[8] < DAT_005d757c) {
            pfVar12[8] = 0.0;
          }
          pfVar12[9] = *pfVar1;
          *pfVar1 = (float)pfVar12;
          pfVar11 = (float *)pfVar11[0x37];
        } while (pfVar11 != (float *)0x0);
      }
      local_8 = local_8 + 1;
      local_c = local_c + 0x28;
    } while (local_8 < *(uint *)(param_2 + 4));
  }
  return param_1;
}

