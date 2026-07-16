
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] **CCD (continuous collision detection) + impact resolution pass** for a solver
   context (param_1 = solver struct). */

float * FUN_00561390(float *param_1)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  ushort uVar8;
  undefined4 *puVar9;
  int iVar10;
  float fVar11;
  float fVar12;
  float *pfVar13;
  int iVar14;
  int *piVar15;
  float *pfVar16;
  uint uVar17;
  uint uVar18;
  float fVar19;
  int *piVar20;
  int iVar21;
  undefined4 *puVar22;
  int iVar23;
  undefined4 *puVar24;
  undefined4 *puVar25;
  float10 fVar26;
  float10 fVar27;
  float10 fVar28;
  int *local_a8;
  float local_a4;
  float *local_a0;
  uint local_9c;
  uint local_98;
  int *local_94;
  uint local_90;
  int local_8c;
  int local_84;
  float local_80;
  int local_78;
  float local_74;
  float local_70;
  float local_6c;
  float local_4c [3];
  float local_40;
  float fStack_38;
  undefined1 auStack_30 [12];
  undefined4 uStack_24;
  undefined4 auStack_20 [8];

  puVar9 = (undefined4 *)param_1[0x10];
  local_6c = 0.0;
  uVar18 = *(int *)(*(int *)param_1[0x1c] + 0x44) * 2;
  local_70 = 0.0;
  local_74 = 0.0;
  puVar24 = puVar9;
  for (uVar17 = uVar18 >> 2; uVar17 != 0; uVar17 = uVar17 - 1) {
    *puVar24 = 0;
    puVar24 = puVar24 + 1;
  }
  local_80 = 0.0;
  local_78 = 1;
  for (uVar18 = uVar18 & 3; uVar18 != 0; uVar18 = uVar18 - 1) {
    *(undefined1 *)puVar24 = 0;
    puVar24 = (undefined4 *)((int)puVar24 + 1);
  }
  local_94 = (int *)param_1[0x1d];
  iVar23 = 0;
  local_84 = 0;
  local_a4 = 0.0;
  if (param_1[0x32] != 0.0) {
    local_9c = 0;
    do {
      pfVar13 = (float *)((int)param_1[0x35] + local_9c);
      if (((uint)pfVar13[6] & 3) == 0) {
        local_94 = (int *)*pfVar13;
        local_a8 = (int *)0x0;
        if (pfVar13[3] != 0.0) {
          do {
            iVar23 = *local_94;
            if ((*(uint *)(iVar23 + 8) & 4) == 0) {
              if ((*(uint *)(iVar23 + 8) & 8) != 0) {
                iVar21 = 0;
                uVar8 = *(ushort *)(iVar23 + 0xc);
                local_98 = 0;
                if (uVar8 != 0) {
                  do {
                    iVar10 = *(int *)(iVar21 + *(int *)(iVar23 + 0x10));
                    piVar15 = *(int **)(iVar21 + 4 + *(int *)(iVar23 + 0x10));
                    iVar14 = FUN_0055bb70(iVar10,0,local_4c);
                    if (iVar14 != 0) {
                      fVar26 = (float10)FUN_004c3ac0(*(int *)(*(int *)(*piVar15 + 0x10) + 8) + 0x10
                                                     + piVar15[1] * 0x20);
                      fVar27 = (float10)FUN_004c3ac0(piVar15[1] * 0x20 +
                                                     *(int *)(*(int *)(*piVar15 + 0x10) + 8));
                      fVar28 = (float10)FUN_004c3ac0(piVar15 + 2);
                      fVar19 = (float)(((fVar28 + (float10)local_40) * (float10)(float)fVar26 +
                                       (float10)(float)fVar27) * (float10)*param_1);
                      fVar26 = (float10)(**(code **)(*(int *)(iVar10 + 0x5c) + 0x24))(iVar10);
                      if ((((float10)*(float *)(iVar10 + 0x4c) + (float10)*(float *)(iVar10 + 0x4c)
                           + fVar26) * (float10)_DAT_005cc32c < (float10)fVar19) &&
                         (local_40 * param_1[7] < fVar19)) goto LAB_00561536;
                    }
                    local_98 = local_98 + 1;
                    iVar21 = iVar21 + 0xc;
                  } while (local_98 < uVar8);
                }
              }
            }
            else {
LAB_00561536:
              uVar18 = (uint)*(ushort *)(*local_94 + 0xc);
              if ((uVar18 - 1) + local_78 <= (uint)param_1[8]) {
                *(short *)((int)puVar9 + (uint)*(ushort *)(*local_94 + 0x20) * 2) = (short)local_78;
                local_84 = local_84 + 1;
                local_78 = uVar18 + local_78;
              }
            }
            local_a8 = (int *)((int)local_a8 + 1);
            local_94 = local_94 + 1;
            iVar23 = local_84;
          } while (local_a8 < *(uint *)(local_9c + 0xc + (int)param_1[0x35]));
        }
      }
      local_a4 = (float)((int)local_a4 + 1);
      local_9c = local_9c + 0x28;
    } while ((uint)local_a4 < (uint)param_1[0x32]);
    if (iVar23 != 0) {
      local_6c = param_1[0xc];
      local_70 = param_1[0xd];
      local_94 = (int *)param_1[0xb];
      local_74 = param_1[0xe];
      local_80 = param_1[0xf];
      fVar19 = 0.0;
      piVar20 = local_94;
      piVar15 = local_94;
      if (param_1[0x1e] != 0.0) {
        do {
          iVar21 = *(int *)((int)param_1[0x1d] + (int)fVar19 * 4);
          if (*(short *)((int)puVar9 + (uint)*(ushort *)(iVar21 + 0x20) * 2) != 0) {
            *piVar20 = iVar21;
            piVar20 = piVar20 + 1;
          }
          fVar19 = (float)((int)fVar19 + 1);
        } while ((uint)fVar19 < (uint)param_1[0x1e]);
      }
      for (; iVar23 != 0; iVar23 = iVar23 + -1) {
        local_98 = (uint)*(ushort *)(*piVar15 + 0xc);
        uVar18 = (uint)*(ushort *)((int)puVar9 + (uint)*(ushort *)(*piVar15 + 0x20) * 2);
        if (local_98 != 0) {
          iVar21 = 0;
          local_a0 = (float *)((int)local_80 + uVar18 * 4);
          puVar24 = (undefined4 *)(uVar18 * 0x10 + (int)local_70);
          local_a8 = (undefined4 *)((int)local_6c + uVar18 * 0xc);
          do {
            iVar21 = iVar21 + 0xc;
            piVar20 = *(int **)(*(int *)(*piVar15 + 0x10) + -8 + iVar21);
            puVar22 = (undefined4 *)(**(int **)(*piVar20 + 0x10) + 0x30 + piVar20[1] * 0x40);
            *local_a8 = *puVar22;
            local_a8[1] = puVar22[1];
            local_a8[2] = puVar22[2];
            puVar22 = (undefined4 *)(*(int *)(*(int *)(*piVar20 + 0x10) + 0x10) + piVar20[1] * 0x10)
            ;
            *puVar24 = *puVar22;
            puVar24[1] = puVar22[1];
            puVar24[2] = puVar22[2];
            puVar24[3] = puVar22[3];
            *local_a0 = *param_1;
            local_a0 = local_a0 + 1;
            local_98 = local_98 - 1;
            puVar24 = puVar24 + 4;
            local_a8 = local_a8 + 3;
          } while (local_98 != 0);
        }
        piVar15 = piVar15 + 1;
      }
    }
  }
  puVar24 = *(undefined4 **)(*(int *)param_1[0x1c] + 0x10);
  FUN_0056c0a0(*puVar24,puVar24[1],puVar24[4],puVar24[5],puVar24[2],puVar24[3],puVar24[0x14],
               puVar24[0x15],puVar24[6],puVar24[7],*param_1);
  iVar23 = 0;
  local_a8 = (int *)0x0;
  if (param_1[0x32] != 0.0) {
    fVar19 = param_1[0x35];
    do {
      piVar15 = (int *)(iVar23 + (int)fVar19);
      if (((*(byte *)(iVar23 + 0x18 + (int)fVar19) & 1) == 0) && (uVar18 = 0, piVar15[3] != 0)) {
        do {
          piVar15 = *(int **)(*piVar15 + uVar18 * 4);
          (**(code **)(*piVar15 + 0x1c))(piVar15);
          FUN_0055e050(piVar15);
          fVar19 = param_1[0x35];
          uVar18 = uVar18 + 1;
          piVar15 = (int *)(iVar23 + (int)fVar19);
        } while (uVar18 < *(uint *)(iVar23 + 0xc + (int)fVar19));
      }
      local_a8 = (int *)((int)local_a8 + 1);
      iVar23 = iVar23 + 0x28;
    } while (local_a8 < (uint)param_1[0x32]);
  }
  if (local_84 != 0) {
    local_90 = local_84;
    local_a8 = local_94;
    do {
      uVar17 = (uint)*(ushort *)(*local_a8 + 0xc);
      uVar18 = (uint)*(ushort *)((int)puVar9 + (uint)*(ushort *)(*local_a8 + 0x20) * 2);
      local_a4 = 0.0;
      local_9c = 0;
      if (uVar17 != 0) {
        local_98 = 0;
        puVar24 = (undefined4 *)(uVar18 * 0x20 + (int)local_74);
        local_a0 = (float *)((int)local_6c + uVar18 * 0xc);
        pfVar13 = (float *)((int)local_70 + 4 + uVar18 * 0x10);
        do {
          iVar23 = FUN_00561280(*local_a8,local_9c,auStack_30);
          if (iVar23 != 0) {
            FUN_00565fa0(puVar24,local_a0,auStack_30,uStack_24);
            piVar15 = *(int **)(*(int *)(*local_a8 + 0x10) + 4 + local_98);
            fVar11 = _DAT_005cc574 /
                     (pfVar13[2] * pfVar13[2] +
                     pfVar13[1] * pfVar13[1] + *pfVar13 * *pfVar13 + pfVar13[-1] * pfVar13[-1]);
            fVar12 = pfVar13[-1] * fVar11;
            fStack_38 = *pfVar13 * fVar11;
            fVar11 = pfVar13[1] * fVar11;
            fVar19 = pfVar13[2];
            fVar1 = pfVar13[2];
            fVar2 = pfVar13[2];
            fVar3 = pfVar13[-1];
            fVar4 = *pfVar13;
            fVar5 = pfVar13[1];
            local_4c[0] = fVar11 * *pfVar13;
            fVar6 = pfVar13[1];
            fVar7 = pfVar13[-1];
            *(float *)(piVar15[1] * 0x40 + **(int **)(*piVar15 + 0x10)) =
                 _DAT_005cc320 - (fVar11 * fVar5 + fStack_38 * fVar4);
            *(float *)(**(int **)(*piVar15 + 0x10) + 4 + piVar15[1] * 0x40) =
                 fVar2 * fVar11 + fStack_38 * fVar7;
            *(float *)(**(int **)(*piVar15 + 0x10) + 8 + piVar15[1] * 0x40) =
                 fVar12 * fVar6 - fVar1 * fStack_38;
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x10 + piVar15[1] * 0x40) =
                 fStack_38 * fVar7 - fVar2 * fVar11;
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x14 + piVar15[1] * 0x40) =
                 _DAT_005cc320 - (fVar11 * fVar5 + fVar12 * fVar3);
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x18 + piVar15[1] * 0x40) =
                 local_4c[0] + fVar19 * fVar12;
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x20 + piVar15[1] * 0x40) =
                 fVar12 * fVar6 + fVar1 * fStack_38;
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x24 + piVar15[1] * 0x40) =
                 local_4c[0] - fVar19 * fVar12;
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x28 + piVar15[1] * 0x40) =
                 _DAT_005cc320 - (fStack_38 * fVar4 + fVar12 * fVar3);
            *(undefined4 *)(**(int **)(*piVar15 + 0x10) + 0x30 + piVar15[1] * 0x40) = 0;
            *(undefined4 *)(**(int **)(*piVar15 + 0x10) + 0x34 + piVar15[1] * 0x40) = 0;
            *(undefined4 *)(**(int **)(*piVar15 + 0x10) + 0x38 + piVar15[1] * 0x40) = 0;
            *(undefined4 *)(**(int **)(*piVar15 + 0x10) + 0xc + piVar15[1] * 0x40) = 3;
            puVar22 = (undefined4 *)(**(int **)(*piVar15 + 0x10) + 0x30 + piVar15[1] * 0x40);
            *puVar22 = *local_a0;
            puVar22[1] = local_a0[1];
            puVar22[2] = local_a0[2];
            pfVar16 = (float *)(*(int *)(*(int *)(*piVar15 + 0x10) + 0x10) + piVar15[1] * 0x10);
            *pfVar16 = pfVar13[-1];
            pfVar16[1] = *pfVar13;
            pfVar16[2] = pfVar13[1];
            pfVar16[3] = pfVar13[2];
            iVar23 = (int)local_a4 + 1;
            if (local_a4 == 0.0) {
              puVar22 = puVar24;
              puVar25 = auStack_20;
              for (iVar21 = 8; local_a4 = (float)iVar23, iVar21 != 0; iVar21 = iVar21 + -1) {
                *puVar25 = *puVar22;
                puVar22 = puVar22 + 1;
                puVar25 = puVar25 + 1;
              }
            }
            else {
              FUN_00565ef0(auStack_20,auStack_20,puVar24);
              local_a4 = (float)iVar23;
            }
          }
          local_9c = local_9c + 1;
          local_98 = local_98 + 0xc;
          pfVar13 = pfVar13 + 4;
          local_a0 = local_a0 + 3;
          puVar24 = puVar24 + 8;
        } while (local_9c < uVar17);
      }
      (**(code **)(*(int *)*local_a8 + 0x1c))((int *)*local_a8);
      FUN_0055ab30(*(undefined4 *)(*local_a8 + 0x24),*local_a8,auStack_20);
      local_a8 = local_a8 + 1;
      local_90 = local_90 + -1;
    } while (local_90 != 0);
    fVar19 = (float)FUN_0055a9a0(*(undefined4 *)param_1[0x1c],local_94,local_84,param_1[0x20],
                                 param_1[0x22],0xf);
    param_1[0x21] = fVar19;
    FUN_00573670(param_1,param_1[0x20],fVar19,puVar9,local_74,local_80,local_78);
    FUN_0056b9d0(param_1,local_94,local_84,puVar9,local_74,local_80,local_78);
    if (local_84 != 0) {
      local_8c = local_84;
      do {
        local_90 = (uint)*(ushort *)(*local_94 + 0xc);
        uVar8 = *(ushort *)((int)puVar9 + (uint)*(ushort *)(*local_94 + 0x20) * 2);
        if (local_90 != 0) {
          iVar23 = 0;
          do {
            FUN_0056be80(*(undefined4 *)(*(int *)param_1[0x1c] + 0x10),
                         *(undefined4 *)(*(int *)(*(int *)(*local_94 + 0x10) + 4 + iVar23) + 4),
                         *(undefined4 *)((int)local_80 + (uint)uVar8 * 4));
            iVar23 = iVar23 + 0xc;
            local_90 = local_90 - 1;
          } while (local_90 != 0);
        }
        (**(code **)(*(int *)*local_94 + 0x1c))((int *)*local_94);
        FUN_0055e050(*local_94);
        local_94 = local_94 + 1;
        local_8c = local_8c + -1;
      } while (local_8c != 0);
    }
  }
  return param_1;
}

