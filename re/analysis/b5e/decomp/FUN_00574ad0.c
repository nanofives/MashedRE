
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `int FUN_00574ad0(float *plane_pair, float *poly_verts, uint
   vert_count, float… */

undefined4
FUN_00574ad0(float *param_1,float *param_2,uint param_3,float param_4,float param_5,
            undefined4 param_6)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  uint uVar16;
  float fVar17;
  int iVar18;
  undefined4 uVar19;
  uint uVar20;
  float *pfVar21;
  float *pfVar22;
  uint uVar23;
  float *pfVar24;
  int iVar25;
  int iVar26;
  int iVar27;
  uint uVar28;
  uint uVar29;
  float10 fVar30;
  uint local_1e8;
  uint local_1e0;
  float *local_1dc;
  uint local_1d8;
  float *local_1d4;
  uint local_1d0;
  float afStack_1c0 [16];
  float afStack_180 [24];
  float local_120 [72];

  iVar25 = 0;
  if (param_3 != 0) {
    if (param_3 == 1) {
      uVar19 = FUN_00575120(param_2,(((param_1[0x22] * param_2[2] +
                                      param_1[0x20] * *param_2 + param_1[0x21] * param_2[1]) -
                                     param_1[0x23]) /
                                     (param_1[0x42] * param_1[0x22] +
                                     param_1[0x40] * param_1[0x20] + param_1[0x21] * param_1[0x41])
                                    - ((param_1[2] * param_2[2] +
                                       *param_1 * *param_2 + param_2[1] * param_1[1]) - param_1[3])
                                      / (param_1[2] * param_1[0x42] +
                                        param_1[0x40] * *param_1 + param_1[0x41] * param_1[1])) -
                                    param_5,param_6,0);
      return uVar19;
    }
    uVar28 = 0;
    local_1e0 = 0;
    fVar17 = DAT_005d757c;
    if (param_3 != 0) {
      pfVar24 = param_2 + 1;
      fVar3 = param_1[0x21];
      fVar4 = param_1[0x41];
      fVar5 = param_1[0x40];
      fVar6 = param_1[0x20];
      fVar7 = param_1[0x42];
      fVar8 = param_1[0x22];
      fVar9 = param_1[1];
      fVar10 = param_1[0x41];
      fVar11 = param_1[0x40];
      fVar12 = *param_1;
      fVar13 = param_1[2];
      fVar14 = param_1[0x42];
      do {
        fVar15 = ((((pfVar24[1] * param_1[0x22] +
                    *pfVar24 * param_1[0x21] + pfVar24[-1] * param_1[0x20]) - param_1[0x23]) /
                   (fVar7 * fVar8 + fVar5 * fVar6 + fVar3 * fVar4) -
                  ((param_1[2] * pfVar24[1] + pfVar24[-1] * *param_1 + *pfVar24 * param_1[1]) -
                  param_1[3]) / (fVar13 * fVar14 + fVar11 * fVar12 + fVar9 * fVar10)) - param_4) -
                 param_5;
        afStack_1c0[uVar28] = fVar15;
        if (fVar15 < *(float *)((int)afStack_1c0 + iVar25)) {
          iVar25 = uVar28 * 4;
          local_1e0 = uVar28;
        }
        if (uVar28 != 0) {
          fVar17 = ABS(*param_2 - pfVar24[-1]) +
                   ABS(param_2[2] - pfVar24[1]) + ABS(param_2[1] - *pfVar24) + fVar17;
        }
        uVar28 = uVar28 + 1;
        pfVar24 = pfVar24 + 4;
      } while (uVar28 < param_3);
    }
    if (DAT_005d757c < afStack_1c0[local_1e0]) {
      return 1;
    }
    uVar28 = param_3 - 1;
    uVar29 = 0;
    local_1d8 = 0x10;
    local_1d0 = 0;
    fVar17 = (_DAT_005cc328 / (float)uVar28) * fVar17;
    if (param_3 != 0) {
      local_1d4 = local_120 + 1;
      local_1dc = local_120 + 2;
      iVar25 = uVar28 * 0x10;
      pfVar22 = local_120;
      pfVar24 = param_2 + 2;
      uVar20 = local_1d0;
      do {
        local_1d0 = uVar20;
        pfVar1 = afStack_1c0 + local_1d0;
        fVar3 = afStack_1c0[uVar28];
        pfVar2 = afStack_1c0 + uVar28;
        pfVar21 = pfVar22;
        if (afStack_1c0[local_1d0] < DAT_005d757c == (afStack_1c0[local_1d0] == DAT_005d757c)) {
          if (fVar3 < DAT_005d757c != (fVar3 == DAT_005d757c)) {
            afStack_180[uVar29] = 0.0;
            fVar3 = *pfVar2 / (*pfVar2 - *pfVar1);
            *pfVar22 = (pfVar24[-2] - *(float *)(iVar25 + (int)param_2)) * fVar3 +
                       *(float *)(iVar25 + (int)param_2);
            pfVar22 = (float *)(iVar25 + 8 + (int)param_2);
            *local_1d4 = (pfVar24[-1] - *(float *)(iVar25 + 4 + (int)param_2)) * fVar3 +
                         *(float *)(iVar25 + 4 + (int)param_2);
            *local_1dc = (*pfVar24 - *pfVar22) * fVar3 + *pfVar22;
            goto LAB_00574ea7;
          }
        }
        else {
          if (DAT_005d757c < fVar3) {
            afStack_180[uVar29] = 0.0;
            uVar29 = uVar29 + 1;
            pfVar21 = pfVar22 + 3;
            fVar3 = *pfVar2 / (*pfVar2 - *pfVar1);
            *pfVar22 = (pfVar24[-2] - *(float *)(iVar25 + (int)param_2)) * fVar3 +
                       *(float *)(iVar25 + (int)param_2);
            pfVar22 = (float *)(iVar25 + 8 + (int)param_2);
            *local_1d4 = (pfVar24[-1] - *(float *)(iVar25 + 4 + (int)param_2)) * fVar3 +
                         *(float *)(iVar25 + 4 + (int)param_2);
            *local_1dc = (*pfVar24 - *pfVar22) * fVar3 + *pfVar22;
            local_1dc = local_1dc + 3;
            local_1d4 = local_1d4 + 3;
          }
          *pfVar21 = pfVar24[-2];
          pfVar21[1] = pfVar24[-1];
          pfVar21[2] = *pfVar24;
          afStack_180[uVar29] = *pfVar1;
          if (local_1e0 == local_1d0) {
            local_1d8 = uVar29;
          }
LAB_00574ea7:
          local_1dc = local_1dc + 3;
          local_1d4 = local_1d4 + 3;
          uVar29 = uVar29 + 1;
          pfVar22 = pfVar21 + 3;
        }
        iVar25 = (-8 - (int)param_2) + (int)pfVar24;
        pfVar24 = pfVar24 + 4;
        uVar20 = local_1d0 + 1;
        uVar28 = local_1d0;
      } while (local_1d0 + 1 < param_3);
      if (4 < uVar29) {
        local_1e8 = 0;
        if (uVar29 != 0) {
          iVar25 = (uVar29 * 3 + -6) * 4;
          uVar28 = uVar29 - 1;
          iVar18 = 0;
          iVar27 = (uVar29 - 1) * 0xc;
          do {
            iVar26 = iVar18;
            if (uVar28 == local_1d8) {
              afStack_1c0[uVar28] = 3.4028235e+38;
            }
            else {
              fVar30 = (float10)FUN_005751f0((int)local_120 + iVar25,(int)local_120 + iVar27,
                                             (int)local_120 + iVar26);
              afStack_1c0[uVar28] = (float)fVar30;
            }
            uVar20 = local_1e8 + 1;
            iVar25 = iVar27;
            uVar28 = local_1e8;
            iVar18 = iVar26 + 0xc;
            iVar27 = iVar26;
            local_1e8 = uVar20;
          } while (uVar20 < uVar29);
        }
        if (4 < uVar29) {
          local_1d8 = uVar29 - 4;
          do {
            uVar20 = 0;
            uVar28 = uVar29;
            fVar3 = _DAT_005ceac0;
            if (uVar29 != 0) {
              do {
                fVar4 = afStack_1c0[uVar20];
                if ((fVar4 != _DAT_005cc33c) && (fVar4 < fVar3)) {
                  uVar28 = uVar20;
                  fVar3 = fVar4;
                }
                uVar20 = uVar20 + 1;
              } while (uVar20 < uVar29);
              if (uVar28 != uVar29) {
                uVar23 = 0;
                afStack_1c0[uVar28] = -1.0;
                uVar20 = 0;
                local_1e0 = uVar28;
                do {
                  local_1e0 = local_1e0 + 1;
                  if (uVar29 <= local_1e0) {
                    local_1e0 = 0;
                  }
                  local_1e8 = local_1e0;
                } while ((afStack_1c0[local_1e0] == -1.0) && (uVar20 = uVar20 + 1, uVar20 < uVar29))
                ;
                do {
                  local_1e8 = local_1e8 + 1;
                  if (uVar29 <= local_1e8) {
                    local_1e8 = 0;
                  }
                } while ((afStack_1c0[local_1e8] == -1.0) && (uVar23 = uVar23 + 1, uVar23 < uVar29))
                ;
                uVar20 = 0;
                do {
                  uVar23 = uVar29;
                  if (uVar28 != 0) {
                    uVar23 = uVar28;
                  }
                  uVar28 = uVar23 - 1;
                } while ((afStack_1c0[uVar28] == -1.0) && (uVar20 = uVar20 + 1, uVar20 < uVar29));
                uVar23 = 0;
                uVar20 = uVar28;
                do {
                  uVar16 = uVar29;
                  if (uVar20 != 0) {
                    uVar16 = uVar20;
                  }
                  uVar20 = uVar16 - 1;
                } while ((afStack_1c0[uVar20] == -1.0) && (uVar23 = uVar23 + 1, uVar23 < uVar29));
                fVar30 = (float10)FUN_005751f0(local_120 + uVar20 * 3,local_120 + uVar28 * 3,
                                               local_120 + local_1e0 * 3);
                afStack_1c0[uVar28] = (float)fVar30;
                fVar30 = (float10)FUN_005751f0(local_120 + uVar28 * 3,local_120 + local_1e0 * 3,
                                               local_120 + local_1e8 * 3);
                afStack_1c0[local_1e0] = (float)fVar30;
              }
            }
            local_1d8 = local_1d8 + -1;
          } while (local_1d8 != 0);
        }
        uVar28 = 0;
        if (uVar29 == 0) {
          return 1;
        }
        pfVar24 = local_120;
        do {
          if (afStack_1c0[uVar28] != -1.0) {
            FUN_00575120(pfVar24,param_4 + afStack_180[uVar28],param_6,fVar17);
          }
          uVar28 = uVar28 + 1;
          pfVar24 = pfVar24 + 3;
        } while (uVar28 < uVar29);
        return 1;
      }
    }
    uVar28 = 0;
    if (uVar29 != 0) {
      pfVar24 = local_120;
      do {
        FUN_00575120(pfVar24,param_4 + afStack_180[uVar28],param_6,fVar17);
        uVar28 = uVar28 + 1;
        pfVar24 = pfVar24 + 3;
      } while (uVar28 < uVar29);
      return 1;
    }
  }
  return 1;
}

