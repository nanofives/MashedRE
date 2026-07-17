
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `uint FUN_00576880(float *vertsA, uint countA, float
   *vertsB, uint countB, float… */

uint FUN_00576880(float *param_1,uint param_2,float *param_3,uint param_4,float param_5,
                 float param_6,float *param_7,float *param_8,uint *param_9,uint *param_10,
                 float *param_11,float *param_12)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float *pfVar4;
  int iVar5;
  float *pfVar6;
  uint uVar7;
  uint uVar8;
  float *pfVar9;
  int iVar10;
  uint uVar11;
  uint local_104;
  float local_100;
  float local_fc;
  float local_f8;
  float local_f4;
  float local_f0;
  float local_ec;
  float local_e8;
  float local_e4;
  float local_e0;
  float local_dc;
  float local_d8;
  int local_d4;
  float local_d0;
  float local_cc;
  float local_c8;
  float local_c4;
  float local_c0;
  float local_bc;
  float local_b8;
  uint local_b4;
  uint local_b0;
  float local_ac;
  float local_a8;
  float local_a4;
  float local_a0;
  float local_9c;
  float local_98;
  float local_94;
  float local_90;
  float local_8c;
  uint local_88;
  float local_84;
  float local_80;
  uint local_7c;
  uint local_78;
  uint local_74;
  float local_70;
  uint local_6c;
  float local_68;
  uint local_64;
  uint local_60;
  float local_5c;
  float local_58;
  float local_54;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  uint local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  uint local_2c;
  uint local_28;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_4;

  local_b4 = param_2 + param_4;
  local_28 = 0;
  local_104 = 0;
  if (param_2 == 0) {
    if (param_4 != 0) {
      pfVar4 = param_3 + 1;
      pfVar6 = param_12;
      uVar11 = param_4;
      do {
        uVar11 = uVar11 - 1;
        fVar1 = param_6 - (param_7[2] * pfVar4[1] + pfVar4[-1] * *param_7 + *pfVar4 * param_7[1]);
        *pfVar6 = fVar1 * *param_7 + pfVar4[-1];
        *(float *)((int)param_12 + (-0x10 - (int)param_3) + (int)(pfVar4 + 4)) =
             fVar1 * param_7[1] + *pfVar4;
        pfVar6[2] = fVar1 * param_7[2] + pfVar4[1];
        pfVar4 = pfVar4 + 4;
        pfVar6 = pfVar6 + 4;
      } while (uVar11 != 0);
    }
    return param_4;
  }
  if (param_4 == 0) {
    if (param_2 != 0) {
      pfVar4 = param_1 + 1;
      pfVar6 = param_12;
      uVar11 = param_2;
      do {
        uVar11 = uVar11 - 1;
        fVar1 = param_6 - (param_7[2] * pfVar4[1] + pfVar4[-1] * *param_7 + *pfVar4 * param_7[1]);
        *pfVar6 = fVar1 * *param_7 + pfVar4[-1];
        *(float *)((int)param_12 + (-0x10 - (int)param_1) + (int)(pfVar4 + 4)) =
             fVar1 * param_7[1] + *pfVar4;
        pfVar6[2] = fVar1 * param_7[2] + pfVar4[1];
        pfVar4 = pfVar4 + 4;
        pfVar6 = pfVar6 + 4;
      } while (uVar11 != 0);
    }
    return param_2;
  }
  if (param_2 == 1) {
    *param_9 = 0;
    if (param_4 == 1) {
      local_ec = *param_3;
      local_e8 = param_3[1];
      local_e4 = param_3[2];
      *param_10 = 0;
    }
    else if (param_4 == 2) {
      local_f8 = param_3[4] - *param_3;
      local_f4 = param_3[5] - param_3[1];
      local_f0 = param_3[6] - param_3[2];
      uVar11 = FUN_00577be0(param_3,&local_f8,0,0x3f800000,param_1,&local_ec);
      *param_10 = uVar11;
    }
    else {
      uVar11 = FUN_00577cb0(param_3,param_4,param_7,param_1,&local_ec,&local_b0);
      *param_10 = uVar11;
      if (uVar11 == 2) {
        if (local_b4 <= local_104) {
          return local_104;
        }
        pfVar4 = &local_ec;
LAB_00576bbe:
        FUN_005784a0(&local_104,param_12,pfVar4,param_7,param_6);
        return local_104;
      }
      if (uVar11 == 1) {
        pfVar4 = param_3 + ((int)((local_b0 - param_4) + 1) >> 0x1f & local_b0 + 1) * 4;
        param_3 = param_3 + local_b0 * 4;
        local_f8 = *pfVar4 - *param_3;
        local_f4 = pfVar4[1] - param_3[1];
        local_f0 = pfVar4[2] - param_3[2];
      }
    }
    local_c0 = 0.0;
    local_e0 = *param_1 - local_ec;
    local_ac = *param_1;
    local_a8 = param_1[1];
    local_dc = param_1[1] - local_e8;
    local_d8 = param_1[2] - local_e4;
    local_a4 = param_1[2];
    local_c4 = local_d8 * local_d8 + local_e0 * local_e0 + local_dc * local_dc;
  }
  else if (param_4 == 1) {
    *param_10 = 0;
    if (param_2 == 2) {
      local_d0 = param_1[4] - *param_1;
      local_cc = param_1[5] - param_1[1];
      local_c8 = param_1[6] - param_1[2];
      uVar11 = FUN_00577be0(param_1,&local_d0,0,0x3f800000,param_3,&local_ac);
      *param_9 = uVar11;
    }
    else {
      uVar11 = FUN_00577cb0(param_1,param_2,param_7,param_3,&local_ac,&local_b0);
      *param_9 = uVar11;
      if (uVar11 == 2) {
        if (local_b4 <= local_104) {
          return local_104;
        }
        pfVar4 = &local_ac;
        goto LAB_00576bbe;
      }
      if (uVar11 == 1) {
        pfVar4 = param_1 + ((int)((local_b0 - param_2) + 1) >> 0x1f & local_b0 + 1) * 4;
        param_1 = param_1 + local_b0 * 4;
        local_d0 = *pfVar4 - *param_1;
        local_cc = pfVar4[1] - param_1[1];
        local_c8 = pfVar4[2] - param_1[2];
      }
    }
    local_e0 = local_ac - *param_3;
    local_ec = *param_3;
    local_e8 = param_3[1];
    local_e4 = param_3[2];
    local_c0 = 0.0;
    local_dc = local_a8 - param_3[1];
    local_d8 = local_a4 - param_3[2];
    local_c4 = local_d8 * local_d8 + local_e0 * local_e0 + local_dc * local_dc;
  }
  else if (local_b4 < 5) {
    local_d0 = param_1[4] - *param_1;
    local_cc = param_1[5] - param_1[1];
    local_c8 = param_1[6] - param_1[2];
    local_f8 = param_3[4] - *param_3;
    local_f4 = param_3[5] - param_3[1];
    local_f0 = param_3[6] - param_3[2];
    FUN_00577ec0(param_3,&local_f8,0,0x3f800000,param_1,&local_d0,0,0x3f800000,&local_100,&local_fc,
                 &local_c0);
    local_ac = local_fc * local_d0 + *param_1;
    local_a8 = local_fc * local_cc + param_1[1];
    local_a4 = local_fc * local_c8 + param_1[2];
    local_ec = local_100 * local_f8 + *param_3;
    local_e8 = local_100 * local_f4 + param_3[1];
    local_e4 = local_100 * local_f0 + param_3[2];
    local_e0 = local_ac - local_ec;
    local_dc = local_a8 - local_e8;
    local_d8 = local_a4 - local_e4;
    *param_9 = (uint)(local_fc * local_fc != local_fc);
    *param_10 = (uint)(local_100 * local_100 != local_100);
    local_c4 = local_d8 * local_d8 + local_e0 * local_e0 + local_dc * local_dc;
  }
  else {
    local_6c = local_b4 * 2 + 1;
    iVar10 = 0;
    local_40 = 0;
    local_70 = 0.0;
    local_68 = 0.0;
    local_7c = 0;
    local_88 = 0;
    local_78 = 1;
    local_60 = 1;
    local_d4 = 0;
    local_44 = 0.0;
    local_48 = 0.0;
    local_74 = 0;
    local_64 = 0;
    local_c4 = 3.4028235e+38;
    local_c0 = 0.0;
    local_b0 = 0;
    local_2c = 0;
    if (local_6c != 0) {
      do {
        local_6c = local_6c - 1;
        if (iVar10 != 2) {
          pfVar4 = param_1 + local_60 * 4;
          pfVar6 = param_1 + local_88 * 4;
          local_a0 = *pfVar4 - *pfVar6;
          local_9c = pfVar4[1] - pfVar6[1];
          local_98 = pfVar4[2] - pfVar6[2];
        }
        if (iVar10 != 1) {
          pfVar4 = param_3 + local_78 * 4;
          pfVar6 = param_3 + local_7c * 4;
          local_94 = *pfVar4 - *pfVar6;
          local_90 = pfVar4[1] - pfVar6[1];
          local_8c = pfVar4[2] - pfVar6[2];
        }
        pfVar4 = param_1 + local_88 * 4;
        pfVar6 = param_3 + local_7c * 4;
        FUN_00577ec0(pfVar6,&local_94,0,0x3f800000,pfVar4,&local_a0,0,0x3f800000,&local_fc,
                     &local_100,&local_4c);
        local_4 = local_100 * local_98 + pfVar4[2];
        local_20 = local_fc * local_90 + pfVar6[1];
        local_1c = local_fc * local_8c + pfVar6[2];
        local_58 = (local_100 * local_a0 + *pfVar4) - (local_fc * local_94 + *pfVar6);
        local_54 = (local_100 * local_9c + pfVar4[1]) - local_20;
        local_50 = local_4 - local_1c;
        fVar1 = local_50 * local_50 + local_58 * local_58 + local_54 * local_54;
        if ((fVar1 < local_c4 + _DAT_005ce54c) &&
           ((local_c0 == DAT_005d757c || (local_4c != DAT_005d757c)))) {
          local_2c = (uint)(local_100 * local_100 != local_100);
          local_b0 = (uint)(local_fc * local_fc != local_fc);
          local_40 = local_7c;
          local_70 = local_100;
          local_28 = local_88;
          local_c0 = local_4c;
          local_68 = local_fc;
          local_e0 = local_58;
          local_dc = local_54;
          local_d8 = local_50;
          local_c4 = fVar1;
        }
        pfVar9 = param_3 + local_78 * 4;
        local_3c = param_1[local_60 * 4] - *pfVar6;
        local_38 = param_1[local_60 * 4 + 1] - pfVar6[1];
        local_34 = param_1[local_60 * 4 + 2] - pfVar6[2];
        local_5c = (local_94 * param_7[1] - local_90 * *param_7) * local_98 +
                   (local_90 * param_7[2] - local_8c * param_7[1]) * local_a0 +
                   (local_8c * *param_7 - local_94 * param_7[2]) * local_9c;
        local_b8 = ((*pfVar9 - *pfVar4) * param_7[1] - (pfVar9[1] - pfVar4[1]) * *param_7) *
                   local_98 +
                   ((pfVar9[1] - pfVar4[1]) * param_7[2] - (pfVar9[2] - pfVar4[2]) * param_7[1]) *
                   local_a0 +
                   ((pfVar9[2] - pfVar4[2]) * *param_7 - (*pfVar9 - *pfVar4) * param_7[2]) *
                   local_9c;
        local_bc = (local_3c * param_7[1] - local_38 * *param_7) * local_8c +
                   (local_38 * param_7[2] - local_34 * param_7[1]) * local_94 +
                   (local_34 * *param_7 - local_3c * param_7[2]) * local_90;
        if ((iVar10 == 0) ||
           (((local_b8 * local_48 < DAT_005d757c == (local_b8 * local_48 == DAT_005d757c) &&
             (local_bc * local_44 < DAT_005d757c == (local_bc * local_44 == DAT_005d757c))) ||
            (ABS(local_5c) < _DAT_005ce54c)))) {
LAB_00577479:
          if (local_d4 == 1) {
LAB_00577480:
            if ((local_bc < DAT_005d757c) && (DAT_005d757c < local_b8)) goto LAB_005774a4;
            goto LAB_005774eb;
          }
          if (local_d4 == 2) goto LAB_005774b5;
          if (local_d4 != 0) goto LAB_005774eb;
        }
        else {
          fVar2 = (pfVar4[1] - pfVar6[1]) * param_7[2] - (pfVar4[2] - pfVar6[2]) * param_7[1];
          local_30 = (pfVar4[2] - pfVar6[2]) * *param_7 - (*pfVar4 - *pfVar6) * param_7[2];
          fVar1 = (*pfVar4 - *pfVar6) * param_7[1] - (pfVar4[1] - pfVar6[1]) * *param_7;
          local_80 = (local_30 * local_90 + fVar1 * local_8c + fVar2 * local_94) *
                     (_DAT_005cc320 / local_5c);
          local_84 = (fVar1 * local_98 + fVar2 * local_a0 + local_30 * local_9c) *
                     (_DAT_005cc320 / local_5c);
          if ((((local_80 <= _DAT_005e4568) || (_DAT_005e4564 <= local_80)) ||
              (local_84 <= _DAT_005e4568)) || (_DAT_005e4564 <= local_84)) goto LAB_00577479;
          local_18 = local_80 * local_a0 + *pfVar4;
          local_14 = local_80 * local_9c + pfVar4[1];
          local_10 = local_80 * local_98 + pfVar4[2];
          if (local_b4 <= local_104) {
            return local_104;
          }
          iVar10 = FUN_005784a0(&local_104,param_12,&local_18,param_7,param_6);
          if (iVar10 != 0) {
            return local_104;
          }
          if (local_5c < DAT_005d757c) {
            local_d4 = 1;
            goto LAB_00577480;
          }
LAB_005774a4:
          local_d4 = 2;
LAB_005774b5:
          if (local_b8 < DAT_005d757c) {
            if (DAT_005d757c < local_bc) {
              local_d4 = 1;
            }
LAB_005774eb:
            if ((local_b8 < DAT_005d757c) && (local_bc < DAT_005d757c)) {
              local_d4 = 0;
            }
          }
        }
        if (local_5c <= DAT_005d757c) {
          iVar10 = 2;
          if (local_bc <= DAT_005d757c) goto LAB_0057755d;
        }
        else if (DAT_005d757c < local_b8) {
LAB_0057755d:
          iVar10 = 1;
        }
        else {
          iVar10 = 2;
        }
        if ((local_b8 == DAT_005d757c) && (local_bc == DAT_005d757c)) {
          iVar10 = 2 - (uint)(local_d4 != 1);
        }
        if (iVar10 == 1) {
          local_88 = local_60;
          uVar7 = (int)((local_60 - param_2) + 1) >> 0x1f & local_60 + 1;
          uVar11 = local_78;
          if (local_d4 == 1) {
            pfVar4 = param_1;
            uVar8 = local_60;
            local_60 = uVar7;
            if (local_b4 <= local_104) {
              return local_104;
            }
LAB_00577645:
            iVar5 = FUN_005784a0(&local_104,param_12,pfVar4 + uVar8 * 4,param_7,param_6);
            uVar11 = local_78;
            uVar7 = local_60;
            if (iVar5 != 0) {
              return local_104;
            }
          }
        }
        else {
          local_7c = local_78;
          uVar11 = (int)((local_78 - param_4) + 1) >> 0x1f & local_78 + 1;
          uVar7 = local_60;
          if (local_d4 == iVar10) {
            pfVar4 = param_3;
            uVar8 = local_78;
            local_78 = uVar11;
            if (local_b4 <= local_104) {
              return local_104;
            }
            goto LAB_00577645;
          }
        }
        local_60 = uVar7;
        local_78 = uVar11;
        local_44 = local_bc;
        local_48 = local_b8;
        local_64 = local_64 | local_b8 < DAT_005d757c != (local_b8 == DAT_005d757c);
        local_74 = local_74 | local_bc < DAT_005d757c != (local_bc == DAT_005d757c);
        if ((local_104 == 0) && (local_6c < local_b4)) {
          if (local_74 == 0) {
            uVar11 = param_2;
            pfVar4 = param_1;
            if (local_64 != 0) goto LAB_005778a4;
LAB_0057788b:
            uVar11 = param_4;
            pfVar4 = param_3;
            if (local_74 != 0) {
LAB_005778a4:
              if (uVar11 == 0) {
                return 0;
              }
              pfVar6 = pfVar4 + 1;
              pfVar9 = param_12;
              uVar7 = uVar11;
              do {
                uVar7 = uVar7 - 1;
                fVar1 = param_6 - (param_7[2] * pfVar6[1] +
                                  pfVar6[-1] * *param_7 + *pfVar6 * param_7[1]);
                *pfVar9 = fVar1 * *param_7 + pfVar6[-1];
                *(float *)((int)param_12 + (-0x10 - (int)pfVar4) + (int)(pfVar6 + 4)) =
                     fVar1 * param_7[1] + *pfVar6;
                pfVar9[2] = fVar1 * param_7[2] + pfVar6[1];
                pfVar6 = pfVar6 + 4;
                pfVar9 = pfVar9 + 4;
              } while (uVar7 != 0);
              return uVar11;
            }
          }
          else if (local_64 == 0) goto LAB_0057788b;
          if (_DAT_005ceac0 <= local_c4) {
            return 0;
          }
          pfVar4 = param_1 + ((int)((local_28 - param_2) + 1) >> 0x1f & local_28 + 1) * 4;
          param_1 = param_1 + local_28 * 4;
          local_d0 = *pfVar4 - *param_1;
          local_cc = pfVar4[1] - param_1[1];
          local_c8 = pfVar4[2] - param_1[2];
          uVar11 = (int)((local_40 - param_4) + 1) >> 0x1f & local_40 + 1;
          local_f8 = param_3[uVar11 * 4] - param_3[local_40 * 4];
          pfVar4 = param_3 + local_40 * 4;
          local_f4 = param_3[uVar11 * 4 + 1] - pfVar4[1];
          local_f0 = param_3[uVar11 * 4 + 2] - pfVar4[2];
          local_ac = local_70 * local_d0 + *param_1;
          local_a8 = local_70 * local_cc + param_1[1];
          local_a4 = local_70 * local_c8 + param_1[2];
          local_ec = local_68 * local_f8 + *pfVar4;
          local_e8 = local_68 * local_f4 + pfVar4[1];
          local_e4 = local_68 * local_f0 + pfVar4[2];
          *param_9 = local_2c;
          *param_10 = local_b0;
          goto LAB_005776fc;
        }
      } while (local_6c != 0);
      if (local_104 != 0) {
        return local_104;
      }
    }
  }
LAB_005776fc:
  uVar11 = local_b4;
  if ((((DAT_005d757c <= *param_8) && ((float)PTR_DAT_005ceabc < local_c4)) &&
      (((uint)param_11[0x1f] & 0x20) == 0)) && (((uint)param_11[0x3f] & 0x20) == 0)) {
    fVar1 = _DAT_005cc320 / SQRT(local_c4);
    *param_8 = local_c4 * fVar1;
    *param_7 = local_e0 * fVar1;
    param_7[1] = fVar1 * local_dc;
    param_7[2] = fVar1 * local_d8;
    param_6 = (param_5 + *param_8) * _DAT_005cc32c +
              fVar1 * local_d8 * local_e4 +
              local_e0 * fVar1 * local_ec + fVar1 * local_dc * local_e8;
  }
  if (local_104 < local_b4) {
    FUN_005784a0(&local_104,param_12,&local_ec,param_7,param_6);
  }
  if ((local_c0 <= DAT_005d757c) || (uVar11 <= local_104)) {
    if (*param_9 == 0) {
      *param_11 = *param_7;
      param_11[1] = param_7[1];
      param_11[2] = param_7[2];
      goto LAB_00577ad6;
    }
  }
  else {
    local_ec = local_c0 * local_f8 + local_ec;
    local_e8 = local_c0 * local_f4 + local_e8;
    local_e4 = local_c0 * local_f0 + local_e4;
    FUN_005784a0(&local_104,param_12,&local_ec,param_7,param_6);
    *param_10 = 1;
    *param_9 = 1;
  }
  fVar1 = param_7[2] * local_cc - param_7[1] * local_c8;
  fVar3 = local_c8 * *param_7 - param_7[2] * local_d0;
  fVar2 = param_7[1] * local_d0 - local_cc * *param_7;
  *param_11 = fVar3 * local_c8 - fVar2 * local_cc;
  param_11[1] = fVar2 * local_d0 - fVar1 * local_c8;
  param_11[2] = fVar1 * local_cc - fVar3 * local_d0;
  FUN_005667c0(param_11,param_11);
LAB_00577ad6:
  fVar1 = *param_11 * _DAT_005cc33c;
  *param_11 = fVar1;
  fVar3 = param_11[1] * _DAT_005cc33c;
  param_11[1] = fVar3;
  fVar2 = param_11[2] * _DAT_005cc33c;
  param_11[2] = fVar2;
  param_11[3] = fVar2 * local_a4 + fVar1 * local_ac + fVar3 * local_a8;
  if (*param_10 == 0) {
    param_11[0x20] = *param_7;
    param_11[0x21] = param_7[1];
    param_11[0x22] = param_7[2];
  }
  else {
    pfVar4 = param_11 + 0x20;
    fVar1 = param_7[2] * local_f4 - param_7[1] * local_f0;
    fVar3 = local_f0 * *param_7 - param_7[2] * local_f8;
    fVar2 = param_7[1] * local_f8 - local_f4 * *param_7;
    *pfVar4 = fVar3 * local_f0 - fVar2 * local_f4;
    param_11[0x21] = fVar2 * local_f8 - fVar1 * local_f0;
    param_11[0x22] = fVar1 * local_f4 - fVar3 * local_f8;
    FUN_005667c0(pfVar4,pfVar4);
  }
  param_11[0x23] = param_11[0x22] * local_e4 + param_11[0x20] * local_ec + param_11[0x21] * local_e8
  ;
  return local_104;
}

