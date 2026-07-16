
/* [C1 2026-05-19] **Insert primitive `param_2` with bound `param_3` into the octree.** */

int FUN_00564c80(int param_1,uint param_2,float *param_3)

{
  uint *puVar1;
  ushort *puVar2;
  byte *pbVar3;
  short sVar4;
  ushort uVar5;
  ushort uVar6;
  ushort uVar7;
  ushort uVar8;
  uint uVar9;
  float *pfVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  undefined4 uVar14;
  float *pfVar15;
  float *pfVar16;
  uint uVar17;
  float local_40 [4];
  float local_30;
  float local_2c;
  float local_28;
  float local_20 [8];

  uVar9 = param_2 & 0xffff;
  pfVar10 = (float *)(uVar9 * 0x20 + param_1);
  pfVar10[4] = param_3[4] - *(float *)(param_1 + 0xc014);
  pfVar10[5] = param_3[5] - *(float *)(param_1 + 0xc014);
  pfVar10[6] = param_3[6] - *(float *)(param_1 + 0xc014);
  *pfVar10 = *(float *)(param_1 + 0xc014) + *param_3;
  pfVar10[1] = param_3[1] + *(float *)(param_1 + 0xc014);
  pfVar10[2] = param_3[2] + *(float *)(param_1 + 0xc014);
  FUN_005646c0(param_1,pfVar10,param_2);
  pfVar15 = (float *)(param_1 + 0x7fe0);
  pfVar16 = local_40;
  for (iVar12 = 8; iVar12 != 0; iVar12 = iVar12 + -1) {
    *pfVar16 = *pfVar15;
    pfVar15 = pfVar15 + 1;
    pfVar16 = pfVar16 + 1;
  }
  uVar13 = 0;
  uVar6 = 0xffff;
  if ((((local_30 <= pfVar10[4]) && (*pfVar10 < local_40[0] != (*pfVar10 == local_40[0]))) &&
      (local_2c <= pfVar10[5])) &&
     (((pfVar10[1] < local_40[1] != (pfVar10[1] == local_40[1]) && (local_28 <= pfVar10[6])) &&
      ((pfVar10[2] < local_40[2] != (pfVar10[2] == local_40[2]) &&
       (uVar6 = FUN_00564310(local_20,pfVar10,local_40), uVar6 != 0xffff)))))) {
    do {
      pfVar15 = local_20;
      pfVar16 = local_40;
      for (iVar12 = 8; iVar12 != 0; iVar12 = iVar12 + -1) {
        *pfVar16 = *pfVar15;
        pfVar15 = pfVar15 + 1;
        pfVar16 = pfVar16 + 1;
      }
      iVar12 = (uint)uVar6 + (uVar13 & 0xffff) * 10;
      sVar4 = *(short *)(param_1 + 0x9820 + iVar12 * 2);
      uVar17 = CONCAT22((short)((uint)iVar12 >> 0x10),sVar4);
      if (sVar4 < 0) {
        if (uVar6 != 0xffff) {
          uVar5 = *(ushort *)(param_1 + 0x981a);
          uVar17 = (uint)uVar5;
          *(undefined2 *)(param_1 + 0x981a) = *(undefined2 *)(param_1 + 0x8820 + uVar17 * 4);
          *(ushort *)(param_1 + 0x881e + uVar17 * 4) = (ushort)param_2;
          puVar2 = (ushort *)(param_1 + 0x9820 + ((uint)uVar6 + (uVar13 & 0xffff) * 10) * 2);
          *(ushort *)(param_1 + 0x8820 + uVar17 * 4) = *puVar2 & 0x3ff;
          *puVar2 = *puVar2 & 0xfc00 | uVar5;
          *(ushort *)(param_1 + 0x8020 + uVar9 * 2) = uVar6 & 0x3f | (ushort)(uVar13 << 6);
          iVar12 = FUN_005641b0(pfVar10,local_40);
          if (iVar12 != 0) {
            pbVar3 = (byte *)(param_1 + 0x881f + uVar17 * 4);
            *pbVar3 = *pbVar3 | 0x80;
            if ((*puVar2 & 0x7c00) != 0x7c00) {
              *puVar2 = *puVar2 + 0x400;
            }
            if (0xc00 < (*puVar2 & 0x7c00)) {
              uVar5 = *(ushort *)(param_1 + 0xc00e);
              uVar14 = CONCAT22((short)((uVar13 << 6) >> 0x10),uVar5);
              if (uVar5 != 0x3ff) {
                uVar7 = *puVar2 & 0x3ff;
                iVar12 = (uint)uVar5 * 5 + 0x2607;
                *(undefined2 *)(param_1 + 0xc00e) = *(undefined2 *)(param_1 + iVar12 * 4);
                *puVar2 = uVar5;
                *(uint *)(param_1 + iVar12 * 4) =
                     ((uint)uVar6 << 10 | uVar13 & 0xffff) << 10 | 0x3ff;
                uVar9 = 0;
                iVar12 = (uint)uVar5 * 10;
                do {
                  uVar13 = uVar9 & 0xffff;
                  uVar9 = uVar9 + 1;
                  *(undefined2 *)(param_1 + 0x9820 + (uVar13 + iVar12) * 2) = 0x83ff;
                } while ((int)uVar9 < 8);
                if (uVar7 != 0x3ff) {
                  do {
                    uVar13 = (uint)uVar7;
                    uVar6 = *(ushort *)(param_1 + 0x8820 + uVar13 * 4);
                    uVar8 = *(ushort *)(param_1 + 0x881e + uVar13 * 4);
                    uVar9 = uVar8 & 0x3ff;
                    if ((short)uVar8 < 0) {
                      iVar11 = uVar9 * 0x20 + param_1;
                      uVar8 = FUN_00564310(local_20,iVar11,local_40);
                      uVar17 = (uint)uVar8;
                      *(ushort *)(param_1 + 0x8820 + uVar13 * 4) =
                           *(ushort *)(param_1 + 0x9820 + (uVar17 + iVar12) * 2) & 0x3ff;
                      FUN_00565120(param_1,uVar14,uVar17,uVar7);
                      *(ushort *)(param_1 + 0x8020 + uVar9 * 2) = uVar8 & 0x3f | uVar5 << 6;
                      iVar11 = FUN_005641b0(iVar11,local_20);
                      if (iVar11 == 0) {
                        pbVar3 = (byte *)(param_1 + 0x881f + uVar13 * 4);
                        *pbVar3 = *pbVar3 & 0x7f;
                      }
                      else {
                        FUN_005651b0(param_1,uVar14,uVar17);
                      }
                    }
                    else {
                      FUN_00565160(param_1,uVar14,uVar7);
                      *(ushort *)(param_1 + 0x8020 + uVar9 * 2) = uVar5 << 6 | 0x3f;
                    }
                    uVar7 = uVar6;
                  } while (uVar6 != 0x3ff);
                }
              }
            }
          }
          return param_1;
        }
        goto LAB_00564dfc;
      }
      uVar6 = FUN_00564310(local_20,pfVar10,local_40);
      uVar13 = uVar17;
    } while (uVar6 != 0xffff);
    uVar6 = 0xffff;
  }
LAB_00564dfc:
  uVar17 = (uint)*(ushort *)(param_1 + 0x981a);
  *(undefined2 *)(param_1 + 0x981a) = *(undefined2 *)(param_1 + 0x8820 + uVar17 * 4);
  *(ushort *)(param_1 + 0x881e + uVar17 * 4) =
       *(ushort *)(param_1 + 0x881e + uVar17 * 4) & 0xfc00 | (ushort)param_2;
  iVar12 = (uVar13 & 0xffff) + 0x79b;
  puVar1 = (uint *)(param_1 + iVar12 * 0x14);
  *(ushort *)(param_1 + 0x8820 + uVar17 * 4) = *(ushort *)(param_1 + iVar12 * 0x14) & 0x3ff;
  *puVar1 = *puVar1 & 0xfffffc00 | uVar17;
  *(ushort *)(param_1 + 0x8020 + uVar9 * 2) = uVar6 & 0x3f | (ushort)(uVar13 << 6);
  return param_1;
}

