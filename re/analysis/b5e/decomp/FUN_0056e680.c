
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_0056e680(int param_1, ..., int *param_3, int *param_4, int
   param_5, ..., int param_7, int… */

void FUN_0056e680(int param_1,undefined4 param_2,int *param_3,int *param_4,int param_5,
                 undefined4 param_6,int param_7,int *param_8,int param_9,undefined4 param_10,
                 int param_11,undefined4 param_12,int param_13,undefined4 param_14,float param_15)

{
  float *pfVar1;
  undefined4 uVar2;
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
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float fVar20;
  float fVar21;
  float fVar22;
  float fVar23;
  float fVar24;
  float fVar25;
  float fVar26;
  float fVar27;
  float fVar28;
  float fVar29;
  float fVar30;
  float fVar31;
  float fVar32;
  float fVar33;
  float fVar34;
  float fVar35;
  float fVar36;
  float fVar37;
  float fVar38;
  float fVar39;
  float fVar40;
  float fVar41;
  float fVar42;
  float fVar43;
  float fVar44;
  float fVar45;
  float fVar46;
  float fVar47;
  float fVar48;
  int iVar49;
  int iVar50;
  int iVar51;
  float *pfVar52;
  int iVar53;
  int local_68;
  float local_44;
  float local_40;
  float local_30;
  float local_2c;
  float local_28;
  float local_20;
  float local_1c;
  float local_18;
  float local_10;
  float local_c;
  float local_8;

  iVar50 = param_11;
  iVar49 = param_1;
  local_68 = 0;
  if (param_8[1] != 0) {
    param_1 = 0;
    do {
      iVar53 = *(int *)(*param_8 + local_68 * 4);
      if ((*(byte *)(param_13 + iVar53 * 4) & 2) == 0) {
        uVar2 = *(undefined4 *)(iVar53 * 0x30 + 0x2c + param_7);
        *(undefined4 *)(param_1 + *param_3) = uVar2;
        *(undefined4 *)(param_1 + 4 + *param_3) = 0;
        *(undefined4 *)(param_1 + 8 + *param_3) = 0;
        *(undefined4 *)(param_1 + 0x10 + *param_3) = 0;
        *(undefined4 *)(param_1 + 0x14 + *param_3) = uVar2;
        *(undefined4 *)(param_1 + 0x18 + *param_3) = 0;
        *(undefined4 *)(param_1 + 0x20 + *param_3) = 0;
        *(undefined4 *)(param_1 + 0x24 + *param_3) = 0;
        *(undefined4 *)(param_1 + 0x28 + *param_3) = uVar2;
        *(undefined4 *)(param_1 + 0xc + *param_3) = *(undefined4 *)(iVar53 * 0x30 + 0xc + param_7);
      }
      else {
        iVar51 = iVar53 * 0x30 + param_7;
        FUN_0056ed60(*param_3 + param_1,iVar51,iVar53 * 0x40 + param_9);
        *(undefined4 *)(param_1 + 0xc + *param_3) = *(undefined4 *)(iVar51 + 0xc);
      }
      param_11 = iVar53 * 0x30;
      if ((*(byte *)(param_13 + iVar53 * 4) & 2) != 0) {
        iVar51 = iVar53 * 0x20;
        FUN_0056ed60(&local_30,param_11 + param_5,iVar53 * 0x40 + param_9);
        fVar6 = local_30 * *(float *)(iVar51 + 0x10 + iVar50) +
                local_20 * *(float *)(iVar51 + 0x14 + iVar50) +
                local_10 * *(float *)(iVar51 + 0x18 + iVar50);
        fVar7 = local_2c * *(float *)(iVar51 + 0x10 + iVar50) +
                local_1c * *(float *)(iVar51 + 0x14 + iVar50) +
                local_c * *(float *)(iVar51 + 0x18 + iVar50);
        fVar8 = local_28 * *(float *)(iVar51 + 0x10 + iVar50) +
                local_18 * *(float *)(iVar51 + 0x14 + iVar50) +
                local_8 * *(float *)(iVar51 + 0x18 + iVar50);
        fVar9 = *(float *)(iVar51 + 0x18 + iVar50) * fVar7 -
                *(float *)(iVar51 + 0x14 + iVar50) * fVar8;
        local_44 = fVar8 * *(float *)(iVar51 + 0x10 + iVar50) -
                   *(float *)(iVar51 + 0x18 + iVar50) * fVar6;
        local_40 = *(float *)(iVar51 + 0x14 + iVar50) * fVar6 -
                   fVar7 * *(float *)(iVar51 + 0x10 + iVar50);
        fVar10 = fVar9 * fVar9 + local_44 * local_44 + local_40 * local_40;
        if ((float)PTR_DAT_005ceabc < fVar10) {
          fVar3 = *(float *)(iVar51 + 0x18 + iVar50);
          fVar4 = *(float *)(iVar51 + 0x14 + iVar50);
          fVar5 = *(float *)(iVar51 + 0x10 + iVar50);
          fVar10 = (fVar3 * fVar3 + fVar4 * fVar4 + fVar5 * fVar5) * SQRT(fVar10) * param_15 *
                   param_15 * _DAT_005cc564;
          if ((float)PTR_DAT_005ceabc < fVar10) {
            fVar3 = (*(float *)(iVar51 + 0x18 + iVar50) * fVar8 +
                    fVar6 * *(float *)(iVar51 + 0x10 + iVar50) +
                    *(float *)(iVar51 + 0x14 + iVar50) * fVar7) * _DAT_005cc32c;
            fVar10 = SQRT((fVar10 + fVar3) / fVar3);
            fVar3 = (_DAT_005cc320 - fVar10) / param_15;
            fVar10 = _DAT_005cc320 / fVar10;
            fVar9 = (fVar6 * fVar3 + fVar9) * fVar10;
            local_44 = (fVar7 * fVar3 + local_44) * fVar10;
            local_40 = (fVar8 * fVar3 + local_40) * fVar10;
          }
          *(float *)(iVar51 + 0x10 + iVar49) = fVar9 + *(float *)(iVar51 + 0x10 + iVar49);
          *(float *)(iVar51 + 0x14 + iVar49) = local_44 + *(float *)(iVar51 + 0x14 + iVar49);
          *(float *)(iVar51 + 0x18 + iVar49) = local_40 + *(float *)(iVar51 + 0x18 + iVar49);
        }
      }
      local_68 = local_68 + 1;
      param_1 = param_1 + 0x30;
    } while (local_68 != param_8[1]);
  }
  param_15 = _DAT_005cc320 / param_15;
  iVar53 = 0;
  param_3[1] = param_8[1];
  local_68 = 0;
  if (param_8[1] == 0) {
    param_4[1] = param_8[1];
    return;
  }
  param_13 = 0;
  do {
    pfVar52 = (float *)(*param_3 + param_13);
    fVar48 = pfVar52[3] * pfVar52[3];
    fVar6 = *pfVar52;
    fVar7 = pfVar52[1];
    fVar8 = *pfVar52;
    fVar9 = pfVar52[2];
    fVar10 = *pfVar52;
    fVar3 = pfVar52[4];
    fVar4 = pfVar52[1];
    fVar5 = pfVar52[1];
    fVar11 = pfVar52[5];
    fVar12 = pfVar52[1];
    fVar13 = pfVar52[6];
    fVar14 = pfVar52[2];
    fVar15 = pfVar52[8];
    fVar16 = pfVar52[9];
    fVar17 = pfVar52[2];
    fVar18 = pfVar52[2];
    fVar19 = pfVar52[10];
    fVar20 = pfVar52[4];
    fVar21 = *pfVar52;
    fVar22 = pfVar52[4];
    fVar23 = pfVar52[2];
    fVar24 = pfVar52[4];
    fVar25 = pfVar52[5];
    fVar26 = pfVar52[5];
    fVar27 = pfVar52[6];
    fVar28 = pfVar52[5];
    fVar29 = pfVar52[8];
    fVar30 = pfVar52[6];
    fVar31 = pfVar52[9];
    fVar32 = pfVar52[6];
    fVar33 = pfVar52[6];
    fVar34 = pfVar52[10];
    iVar51 = *(int *)(*param_8 + local_68 * 4) * 0x20;
    fVar35 = pfVar52[8];
    fVar36 = *pfVar52;
    fVar37 = pfVar52[1];
    fVar38 = pfVar52[8];
    fVar39 = pfVar52[9];
    fVar40 = pfVar52[4];
    fVar41 = pfVar52[9];
    fVar42 = pfVar52[5];
    fVar43 = pfVar52[8];
    fVar44 = pfVar52[10];
    fVar45 = pfVar52[9];
    fVar46 = pfVar52[10];
    fVar47 = pfVar52[10];
    *(float *)(iVar53 + *param_4) =
         param_15 * *(float *)(iVar51 + iVar50) + fVar48 * *(float *)(iVar51 + iVar49);
    *(float *)(*param_4 + 4 + iVar53) =
         param_15 * *(float *)(iVar51 + 4 + iVar50) + fVar48 * *(float *)(iVar51 + 4 + iVar49);
    *(float *)(*param_4 + 8 + iVar53) =
         param_15 * *(float *)(iVar51 + 8 + iVar50) + fVar48 * *(float *)(iVar51 + 8 + iVar49);
    *(float *)(*param_4 + 0x10 + iVar53) = param_15 * *(float *)(iVar51 + 0x10 + iVar50);
    *(float *)(*param_4 + 0x14 + iVar53) = param_15 * *(float *)(iVar51 + 0x14 + iVar50);
    *(float *)(*param_4 + 0x18 + iVar53) = param_15 * *(float *)(iVar51 + 0x18 + iVar50);
    pfVar52 = (float *)(*param_4 + 0x10 + iVar53);
    *pfVar52 = (fVar6 * fVar6 + fVar3 * fVar4 + fVar14 * fVar15) *
               *(float *)(iVar51 + 0x10 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x14 + iVar53);
    param_13 = param_13 + 0x30;
    *pfVar52 = (fVar16 * fVar17 + fVar5 * fVar11 + fVar7 * fVar8) *
               *(float *)(iVar51 + 0x10 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x18 + iVar53);
    *pfVar52 = (fVar18 * fVar19 + fVar12 * fVar13 + fVar9 * fVar10) *
               *(float *)(iVar51 + 0x10 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x10 + iVar53);
    *pfVar52 = (fVar29 * fVar30 + fVar24 * fVar25 + fVar20 * fVar21) *
               *(float *)(iVar51 + 0x14 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x14 + iVar53);
    *pfVar52 = (fVar26 * fVar26 + fVar3 * fVar4 + fVar31 * fVar32) *
               *(float *)(iVar51 + 0x14 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x18 + iVar53);
    pfVar1 = (float *)(iVar51 + 0x18 + iVar49);
    *pfVar52 = (fVar33 * fVar34 + fVar27 * fVar28 + fVar22 * fVar23) *
               *(float *)(iVar51 + 0x14 + iVar49) + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x10 + iVar53);
    *pfVar52 = (fVar43 * fVar44 + fVar39 * fVar40 + fVar35 * fVar36) * *pfVar1 + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x14 + iVar53);
    *pfVar52 = (fVar45 * fVar46 + fVar41 * fVar42 + fVar37 * fVar38) * *pfVar1 + *pfVar52;
    pfVar52 = (float *)(*param_4 + 0x18 + iVar53);
    local_68 = local_68 + 1;
    iVar53 = iVar53 + 0x20;
    *pfVar52 = (fVar47 * fVar47 + fVar14 * fVar15 + fVar31 * fVar32) * *pfVar1 + *pfVar52;
  } while (local_68 != param_8[1]);
  param_4[1] = param_8[1];
  return;
}

