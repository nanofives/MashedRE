
/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] **Octree overlap query** with an iterative stack-based walk. Stack holds AABBs in
   local_2800..afStack_27ec (5-level… */

void FUN_005646c0(int param_1,float *param_2,ushort param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  ushort uVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  float *pfVar11;
  float *pfVar12;
  uint local_2ad8;
  uint local_2ad4;
  uint local_2ad0;
  int local_2ac8;
  float local_2aa4 [4];
  float local_2a94;
  float local_2a90;
  float local_2a8c;
  uint local_2a84;
  ushort local_2a80 [320];
  float local_2800 [5];
  float afStack_27ec [2554];
  undefined4 uStack_4;

  uStack_4 = 0x5646ca;
  pfVar11 = (float *)(param_1 + 0x7fe0);
  pfVar12 = local_2800;
  for (iVar10 = 8; iVar10 != 0; iVar10 = iVar10 + -1) {
    *pfVar12 = *pfVar11;
    pfVar11 = pfVar11 + 1;
    pfVar12 = pfVar12 + 1;
  }
  local_2a80[0] = 0;
  local_2ac8 = 1;
  do {
    local_2ac8 = local_2ac8 + -1;
    local_2a84 = (uint)local_2a80[local_2ac8];
    uVar7 = *(ushort *)(param_1 + (local_2a84 * 5 + 0x2607) * 4) & 0x3ff;
    pfVar11 = local_2800 + local_2ac8 * 8;
    pfVar12 = local_2aa4;
    for (iVar10 = 8; iVar10 != 0; iVar10 = iVar10 + -1) {
      *pfVar12 = *pfVar11;
      pfVar11 = pfVar11 + 1;
      pfVar12 = pfVar12 + 1;
    }
    for (; uVar7 != 0x3ff; uVar7 = *(ushort *)(param_1 + 0x8820 + (uint)uVar7 * 4)) {
      uVar8 = *(ushort *)(param_1 + 0x881e + (uint)uVar7 * 4) & 0x3ff;
      DAT_007dc8c8 = DAT_007dc8c8 + 1;
      pfVar11 = (float *)(uVar8 * 0x20 + param_1);
      if (((((pfVar11[4] < *param_2 != (pfVar11[4] == *param_2)) &&
            (param_2[4] < *pfVar11 != (param_2[4] == *pfVar11))) &&
           (pfVar11[5] < param_2[1] != (pfVar11[5] == param_2[1]))) &&
          ((param_2[5] < pfVar11[1] != (param_2[5] == pfVar11[1]) &&
           (pfVar11[6] < param_2[2] != (pfVar11[6] == param_2[2]))))) &&
         ((param_2[6] < pfVar11[2] != (param_2[6] == pfVar11[2]) &&
          ((iVar10 = *(int *)(param_1 + 0xc018), iVar10 == 0 ||
           (uVar9 = (uint)*(ushort *)(param_1 + 0xb810 + uVar8 * 2) +
                    (uint)*(ushort *)(param_1 + 0xb810 + (uint)param_3 * 2) * *(int *)(iVar10 + 4),
           (*(uint *)(iVar10 + 0xc + (uVar9 >> 5) * 4) & 1 << ((byte)uVar9 & 0x1f)) == 0)))))) {
        DAT_007dc8cc = DAT_007dc8cc + 1;
        FUN_00563e70(*(undefined4 *)(param_1 + 0xc010),param_3,uVar8);
      }
    }
    local_2ad0 = 0xff;
    fVar1 = local_2aa4[0] * _DAT_005e5418 + local_2a94 * _DAT_005cc318;
    fVar2 = local_2aa4[1] * _DAT_005e5418 + local_2a90 * _DAT_005cc318;
    fVar3 = local_2aa4[2] * _DAT_005e5418 + local_2a8c * _DAT_005cc318;
    fVar4 = local_2a94 * _DAT_005e5418 + local_2aa4[0] * _DAT_005cc318;
    fVar5 = local_2a90 * _DAT_005e5418 + local_2aa4[1] * _DAT_005cc318;
    fVar6 = local_2a8c * _DAT_005e5418 + local_2aa4[2] * _DAT_005cc318;
    if (fVar4 < param_2[4]) {
      local_2ad0 = 0xaa;
    }
    if (*param_2 < fVar1) {
      local_2ad0 = local_2ad0 & 0x55;
    }
    if (fVar5 < param_2[5]) {
      local_2ad0 = local_2ad0 & 0xcc;
    }
    if (param_2[1] < fVar2) {
      local_2ad0 = local_2ad0 & 0x33;
    }
    if (fVar6 < param_2[6]) {
      local_2ad0 = local_2ad0 & 0xf0;
    }
    if (param_2[2] < fVar3) {
      local_2ad0 = local_2ad0 & 0xf;
    }
    local_2ad4 = 0;
    local_2ad8 = 0xffffffff;
    pfVar11 = afStack_27ec + local_2ac8 * 8;
    do {
      if ((local_2ad0 & 1 << ((byte)local_2ad4 & 0x1f)) != 0) {
        uVar7 = *(ushort *)(param_1 + 0x9820 + (local_2a84 * 10 + local_2ad4) * 2);
        if ((uVar7 & 0x8000) == 0) {
          local_2a80[local_2ac8] = uVar7;
          uVar8 = (int)local_2ad4 >> 1 & 1;
          uVar9 = (int)local_2ad4 >> 2 & 1;
          pfVar11[-1] = (float)(local_2ad4 & 1) * fVar1 +
                        (float)(int)(1 - (local_2ad4 & 1)) * local_2a94;
          *pfVar11 = (float)uVar8 * fVar2 + (float)(int)(1 - uVar8) * local_2a90;
          uVar8 = (int)local_2ad8 >> 1 & 1;
          pfVar11[1] = (float)uVar9 * fVar3 + (float)(int)(1 - uVar9) * local_2a8c;
          uVar9 = (int)local_2ad8 >> 2 & 1;
          pfVar11[-5] = (float)(local_2ad8 & 1) * fVar4 +
                        (float)(int)(1 - (local_2ad8 & 1)) * local_2aa4[0];
          local_2ac8 = local_2ac8 + 1;
          pfVar11[-4] = (float)uVar8 * fVar5 + (float)(int)(1 - uVar8) * local_2aa4[1];
          pfVar11[-3] = (float)uVar9 * fVar6 + (float)(int)(1 - uVar9) * local_2aa4[2];
          pfVar11 = pfVar11 + 8;
        }
        else {
          for (uVar7 = *(ushort *)(param_1 + 0x9820 + ((local_2ad4 & 0xffff) + local_2a84 * 10) * 2)
                       & 0x3ff; uVar7 != 0x3ff;
              uVar7 = *(ushort *)(param_1 + 0x8820 + (uint)uVar7 * 4)) {
            uVar8 = *(ushort *)(param_1 + 0x881e + (uint)uVar7 * 4) & 0x3ff;
            DAT_007dc8c8 = DAT_007dc8c8 + 1;
            pfVar12 = (float *)(uVar8 * 0x20 + param_1);
            if ((((pfVar12[4] < *param_2 != (pfVar12[4] == *param_2)) &&
                 (param_2[4] < *pfVar12 != (param_2[4] == *pfVar12))) &&
                (pfVar12[5] < param_2[1] != (pfVar12[5] == param_2[1]))) &&
               (((param_2[5] < pfVar12[1] != (param_2[5] == pfVar12[1]) &&
                 (pfVar12[6] < param_2[2] != (pfVar12[6] == param_2[2]))) &&
                ((param_2[6] < pfVar12[2] != (param_2[6] == pfVar12[2]) &&
                 ((iVar10 = *(int *)(param_1 + 0xc018), iVar10 == 0 ||
                  (uVar9 = (uint)*(ushort *)(param_1 + 0xb810 + uVar8 * 2) +
                           (uint)*(ushort *)(param_1 + 0xb810 + (uint)param_3 * 2) *
                           *(int *)(iVar10 + 4),
                  (*(uint *)(iVar10 + 0xc + (uVar9 >> 5) * 4) & 1 << ((byte)uVar9 & 0x1f)) == 0)))))
                ))) {
              DAT_007dc8cc = DAT_007dc8cc + 1;
              FUN_00563e70(*(undefined4 *)(param_1 + 0xc010),param_3,uVar8);
            }
          }
        }
      }
      local_2ad4 = local_2ad4 + 1;
      local_2ad8 = local_2ad8 - 1;
    } while (-9 < (int)local_2ad8);
  } while (0 < local_2ac8);
  return;
}

