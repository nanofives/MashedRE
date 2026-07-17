
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `int FUN_0057a250(int body_A, int body_B, float
   depth_cutoff, float… */

undefined4
FUN_0057a250(undefined4 param_1,undefined4 param_2,float param_3,float param_4,float *param_5,
            float *param_6,float *param_7,float param_8)

{
  float *pfVar1;
  float fVar2;
  float fVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  float10 fVar10;
  float local_40;
  int local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18 [4];
  float local_8;
  float local_4;

  iVar5 = (int)param_4;
  param_8 = param_8 * _DAT_005cd03c;
  fVar10 = (float10)_DAT_005ccabc;
  uVar9 = 1;
  local_38 = 0;
  local_34 = 0.0;
  *(undefined4 *)((int)param_4 + 0x1a4) = 0;
  param_4 = 0.0;
  local_30 = 0.5;
  local_2c = 0.6;
  local_28 = 0.6244998;
  fVar10 = (float10)param_8 * fVar10 + (float10)param_3 + (float10)_DAT_005cc320;
  do {
    local_40 = (float)fVar10;
    if (fVar10 <= (float10)param_8) break;
    uVar6 = 1;
    iVar8 = 0;
    *(undefined4 *)(iVar5 + 0x1ac) = 1;
    *(undefined4 *)(iVar5 + 0x1a8) = 0;
    if ((*(uint *)(iVar5 + 0x1a4) & 1) != 0) {
      do {
        uVar6 = uVar6 * 2;
        iVar8 = iVar8 + 1;
      } while ((*(uint *)(iVar5 + 0x1a4) & uVar6) != 0);
      *(int *)(iVar5 + 0x1a8) = iVar8;
      *(uint *)(iVar5 + 0x1ac) = uVar6;
    }
    pfVar1 = (float *)(iVar5 + *(int *)(iVar5 + 0x1a8) * 0x18);
    fVar10 = (float10)FUN_00579d50(pfVar1,&local_30,0xffffffff,param_1,param_2);
    fVar2 = (float)fVar10;
    if ((float10)param_3 < fVar10) {
      *param_5 = local_30;
      param_5[1] = local_2c;
      param_5[2] = local_28;
      FUN_00579c00(&local_30,*(undefined4 *)(iVar5 + 0x1a4),1,iVar5);
      *param_6 = fVar2 * _DAT_005cc32c +
                 param_5[2] * local_28 + local_30 * *param_5 + param_5[1] * local_2c;
      *param_7 = fVar2;
      return 2;
    }
    if (param_4 < fVar2) {
      param_4 = fVar2;
    }
    if ((local_40 < param_4 * _DAT_005ce1d4) ||
       (((DAT_005d757c < local_40 && (DAT_005d757c < fVar2)) &&
        ((local_38 != 0 || (0x28 < (int)uVar9)))))) {
      *param_5 = local_30;
      param_5[1] = local_2c;
      param_5[2] = local_28;
      FUN_00579c00(&local_30,*(undefined4 *)(iVar5 + 0x1a4),1,iVar5);
      *param_6 = fVar2 * _DAT_005cc32c +
                 param_5[2] * local_28 + local_30 * *param_5 + param_5[1] * local_2c;
      *param_7 = fVar2;
      return 1;
    }
    uVar9 = uVar9 + 1;
    if ((0x32 < (int)uVar9) ||
       ((3 < (int)uVar9 &&
        (((ABS(pfVar1[2] - local_18[2]) + ABS(*pfVar1 - local_18[0]) + ABS(pfVar1[1] - local_18[1])
           < _DAT_005ce54c ||
          (ABS(pfVar1[2] - local_4) + ABS(*pfVar1 - local_18[3]) + ABS(pfVar1[1] - local_8) <
           _DAT_005ce54c)) && (local_38 = local_38 + 1, 5 < local_38)))))) {
      return 0;
    }
    uVar6 = uVar9 & 1;
    fVar2 = pfVar1[1];
    local_18[uVar6 * 3] = *pfVar1;
    fVar3 = pfVar1[2];
    local_18[uVar6 * 3 + 1] = fVar2;
    local_18[uVar6 * 3 + 2] = fVar3;
    FUN_00579e50(iVar5);
    FUN_00579ee0(iVar5);
    uVar6 = 0;
    iVar8 = 0;
    do {
      uVar4 = *(uint *)((&PTR_DAT_00623fe8)[*(int *)(iVar5 + 0x1a4)] + iVar8 * 4);
      if (((iVar8 != 0) || (param_4 <= DAT_005d757c)) || ((*(uint *)(iVar5 + 0x1ac) | uVar4) != 0xf)
         ) {
        if (uVar4 == 0) {
          iVar7 = iVar5 + *(int *)(iVar5 + 0x1a8) * 0x18;
          local_24 = *(float *)(iVar5 + *(int *)(iVar5 + 0x1a8) * 0x18);
          local_20 = *(float *)(iVar7 + 4);
          local_1c = *(float *)(iVar7 + 8);
        }
        else {
          iVar7 = FUN_00579c00(&local_24,*(uint *)(iVar5 + 0x1ac) | uVar4,0,iVar5);
          if (iVar7 == 0) goto LAB_0057a521;
        }
        fVar2 = local_1c * local_1c + local_24 * local_24 + local_20 * local_20;
        if ((uVar6 == 0) || (fVar2 < local_34 != (fVar2 == local_34))) {
          local_30 = local_24;
          local_2c = local_20;
          local_28 = local_1c;
          uVar6 = uVar4;
          local_34 = fVar2;
        }
      }
LAB_0057a521:
      iVar8 = iVar8 + 1;
    } while (uVar4 != 0);
    fVar10 = (float10)FUN_005667c0(&local_30,&local_30);
    uVar6 = *(uint *)(iVar5 + 0x1ac) | uVar6;
    *(uint *)(iVar5 + 0x1a4) = uVar6;
  } while ((int)uVar6 < 0xf);
  *param_7 = -1.0;
  return 1;
}

