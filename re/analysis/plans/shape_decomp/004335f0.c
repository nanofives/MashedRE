
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Restarted to delay deadcode elimination for space: stack */
/* [C1 2026-05-07] Car-selection / race-position HUD renderer */

void FUN_004335f0(void)

{
  int iVar1;
  bool bVar2;
  float fVar3;
  byte bVar4;
  char cVar5;
  int iVar6;
  undefined4 uVar7;
  int *piVar8;
  undefined4 uVar9;
  float fVar10;
  char cVar11;
  float *pfVar12;
  float fVar13;
  float *pfVar14;
  float10 fVar15;
  undefined4 uStack_7c;
  char acStack_78 [4];
  undefined4 local_74;
  undefined4 local_70;
  int local_6c;
  undefined1 local_68;
  undefined1 local_67;
  undefined1 local_66;
  byte local_65;
  undefined1 local_64;
  undefined1 local_63;
  undefined1 local_62;
  byte local_61;
  undefined1 local_60;
  undefined1 local_5f;
  undefined1 local_5e;
  byte local_5d;
  undefined1 local_5c;
  undefined1 local_5b;
  undefined1 local_5a;
  byte local_59;
  undefined1 local_58;
  undefined1 local_57;
  undefined1 local_56;
  byte local_55;
  undefined1 local_54;
  undefined1 local_53;
  undefined1 local_52;
  byte local_51;
  undefined1 local_50;
  undefined1 local_4f;
  undefined1 local_4e;
  byte local_4d;
  float fStack_4c;
  int iStack_48;
  float fStack_44;
  float *pfStack_40;
  float fStack_3c;
  float fStack_38;
  float fStack_34;
  float fStack_30;
  float fStack_2c;
  float fStack_28;
  float fStack_24;
  float fStack_20;
  undefined1 *local_1c [7];
  
  bVar4 = DAT_0067e7ac;
  local_68 = 0xe0;
  local_67 = 0xe0;
  local_66 = 0xe0;
  local_5f = 0xa7;
  local_5e = 0xa7;
  local_1c[0] = &local_68;
  local_1c[3] = &local_5c;
  local_1c[6] = &local_64;
  local_1c[1] = &local_54;
  local_1c[2] = &local_58;
  local_74 = 0xffffffff;
  local_1c[4] = &local_50;
  local_1c[5] = &local_60;
  local_50 = 0xdb;
  local_4f = 0xc3;
  local_4e = 0x62;
  local_54 = 0x98;
  local_53 = 0x3a;
  local_52 = 0x3d;
  local_58 = 0x4e;
  local_57 = 0x89;
  local_56 = 0xae;
  local_5c = 0x61;
  local_5b = 0x76;
  local_5a = 0x56;
  local_64 = 0;
  local_63 = 0;
  local_62 = 0;
  local_60 = 0xeb;
  DAT_008990dc = 0;
  local_6c = CONCAT31(local_6c._1_3_,DAT_0067e7ac);
  local_65 = DAT_0067e7ac;
  local_61 = DAT_0067e7ac;
  local_5d = DAT_0067e7ac;
  local_70 = CONCAT13(DAT_0067e7ac,0xd7d7d7);
  local_59 = DAT_0067e7ac;
  local_55 = DAT_0067e7ac;
  local_51 = DAT_0067e7ac;
  local_4d = DAT_0067e7ac;
  (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
  fVar15 = (float10)fsin((float10)_DAT_0067ed14 * (float10)_DAT_005cc9b8);
  fStack_44 = (float)(fVar15 * (float10)_DAT_005ccd6c);
  iVar6 = FUN_00430760();
  if (iVar6 != 0) {
    local_74 = CONCAT13(bVar4,(undefined3)local_74);
    uVar9 = local_74;
    fStack_34 = 64.0;
    fVar13 = (DAT_0067f0c8 - _DAT_005cd914) - _DAT_005cd274;
    fStack_3c = 146.0;
    fStack_30 = 64.0;
    fStack_24 = 143.0;
    fStack_2c = 69.0;
    fStack_20 = DAT_0067f0c8 - _DAT_005cd910;
    fStack_28 = 20.0;
    iVar6 = 0;
    fStack_38 = fVar13;
    do {
      FUN_00472c60(fStack_24,fStack_20,fStack_2c,fStack_28,*(undefined4 *)local_1c[iVar6 + 1]);
      uVar7 = FUN_0042fab0(iVar6);
      FUN_004739f0(uVar7,fStack_3c,fVar13,0x42800000,fStack_30,uVar9,0,0x3f800000,0,0x3f800000,1,1);
      fStack_3c = fStack_3c + _DAT_005ccd0c;
      iVar6 = iVar6 + 1;
      fStack_24 = fStack_24 + _DAT_005ccd0c;
    } while (iVar6 < 6);
    bVar4 = (byte)local_6c;
    fVar13 = DAT_0067f0c8 - _DAT_005cd8d8;
    uStack_7c = CONCAT13((byte)local_6c >> 1,0x146ef0);
    fStack_30 = 26.0;
    fStack_34 = 534.0;
    fStack_2c = 534.0;
    fStack_3c = 48.0;
    fStack_28 = 26.0;
    fStack_38 = fVar13;
    FUN_00472c60(0x42400000,fVar13,0x44058000,0x41d00000,uStack_7c);
    uStack_7c = CONCAT13(bVar4,0x1050b4);
    fStack_34 = 2.0;
    fStack_3c = 46.0;
    FUN_00472c60(0x42380000,fVar13,0x40000000,0x41d00000,uStack_7c);
    fStack_34 = fStack_2c + _DAT_005cc35c;
    fStack_30 = 2.0;
    FUN_00472c60(0x42380000,fVar13,fStack_34,0x40000000,uStack_7c);
    fStack_4c = fStack_28 - _DAT_005cc574;
    fStack_38 = fStack_4c + fStack_38;
    FUN_00472c60(0x42380000,fStack_38,fStack_34,0x40000000,uStack_7c);
    fStack_3c = fStack_2c + _DAT_005cd784;
    fStack_34 = 2.0;
    fStack_38 = fStack_38 - fStack_4c;
    fStack_30 = fStack_28;
    FUN_00472c60(fStack_3c,fStack_38,0x40000000,fStack_28,uStack_7c);
    if (DAT_0067f0c0 != 6) {
      uStack_7c = *(uint *)local_1c[DAT_0067f0c0];
    }
    else {
      uStack_7c = local_70;
    }
    FUN_0042bcb0(DAT_007f1a14,DAT_0067f0c4,DAT_0067f0c8 - _DAT_005cc320,uStack_7c,DAT_0067f0c0 == 6)
    ;
    FUN_004282a0(0x36,0x3f4ccccd);
    iVar6 = FUN_004a2c48();
    fStack_4c = (float)(iVar6 / 2 + 4);
    acStack_78[0] = (char)DAT_007f1a14 + '1';
    uStack_7c = local_6c << 0x18;
    acStack_78[1] = 0;
    FUN_00427f00(acStack_78,(float)(int)fStack_4c + DAT_0067f0c4 + _DAT_005cc9b0,DAT_0067f0c8,
                 uStack_7c,0x3f19999a,0);
    (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
    return;
  }
  uStack_7c = (uint)bVar4 << 0x18;
  if (DAT_007f0fe8 != 0) {
    fStack_3c = 320.0;
    fStack_38 = 330.0;
    FUN_00427e00(0x245,0x43a00000,0x43a50000,uStack_7c,0x3f19999a,2);
    FUN_00427e00(0x244,0x43a00000,0x43af0000,uStack_7c,0x3f19999a,2);
    FUN_00427e00(0x277,0x43a00000,0x43b90000,uStack_7c,0x3f19999a,2);
    FUN_00427e00(0x278,0x43a00000,0x43c30000,uStack_7c,0x3f19999a,2);
  }
  piVar8 = &DAT_0067eafc;
  fVar13 = DAT_005d757c;
  do {
    if (piVar8[-3] != -1) {
      fVar13 = fVar13 + _DAT_005cc320;
    }
    if (*piVar8 != -1) {
      fVar13 = fVar13 + _DAT_005cc320;
    }
    if (piVar8[3] != -1) {
      fVar13 = fVar13 + _DAT_005cc320;
    }
    if (piVar8[6] != -1) {
      fVar13 = fVar13 + _DAT_005cc320;
    }
    if (piVar8[9] != -1) {
      fVar13 = fVar13 + _DAT_005cc320;
    }
    if (piVar8[0xc] != -1) {
      fVar13 = fVar13 + _DAT_005cc320;
    }
    piVar8 = piVar8 + 0x12;
  } while ((int)piVar8 < 0x67eb8c);
  if (fVar13 < _DAT_005cc574) {
    fStack_38 = 80.0;
    FUN_00427ad0(0x31,0x42a00000,0x42a00000,0x43f00000,0x42c80000,uStack_7c,0x3f333333);
  }
  iStack_48 = 0;
  fStack_4c = 0.0;
  pfStack_40 = (float *)&DAT_0067eaf8;
  do {
    fVar13 = fStack_4c;
    pfVar14 = pfStack_40;
    if (pfStack_40[-2] != -NAN) {
      if (iStack_48 == 0) {
        fStack_34 = 64.0;
        local_74 = CONCAT13((byte)local_6c,(undefined3)local_74);
        uVar9 = local_74;
        fVar13 = (*pfStack_40 - _DAT_005cd914) - _DAT_005cd274;
        fStack_3c = 146.0;
        fStack_30 = 64.0;
        fStack_24 = 143.0;
        fStack_20 = *pfStack_40 - _DAT_005cd910;
        fStack_2c = 69.0;
        fStack_28 = 20.0;
        iVar6 = 0;
        fStack_38 = fVar13;
        do {
          FUN_00472c60(fStack_24,fStack_20,fStack_2c,fStack_28,*(undefined4 *)local_1c[iVar6 + 1]);
          uVar7 = FUN_0042fab0(iVar6);
          FUN_004739f0(uVar7,fStack_3c,fVar13,0x42800000,fStack_30,uVar9,0,0x3f800000,0,0x3f800000,1
                       ,1);
          fStack_3c = fStack_3c + _DAT_005ccd0c;
          iVar6 = iVar6 + 1;
          fStack_24 = fStack_24 + _DAT_005ccd0c;
        } while (iVar6 < 6);
        iStack_48 = 1;
      }
      pfVar14 = pfStack_40;
      fVar13 = fStack_4c;
      uVar9 = FUN_004a2c48(local_6c);
      FUN_0042bde0(uVar9);
      fVar10 = pfVar14[-2];
      if (fVar10 == 0.0) {
        FUN_0042bcb0(fVar13,pfVar14[-1],*pfVar14 - _DAT_005cc320,*(undefined4 *)local_1c[0],0);
      }
      else {
        bVar2 = false;
        pfVar12 = (float *)&DAT_0067eafc;
        iVar6 = 2;
        do {
          if (((fVar10 == pfVar12[-3]) && (0 < (int)fVar10)) && ((float)(iVar6 + -2) != fVar13)) {
            bVar2 = true;
          }
          if (((fVar10 == *pfVar12) && (0 < (int)fVar10)) && ((float)(iVar6 + -1) != fVar13)) {
            bVar2 = true;
          }
          if (((fVar10 == pfVar12[3]) && (0 < (int)fVar10)) && ((float)iVar6 != fVar13)) {
            bVar2 = true;
          }
          if (((fVar10 == pfVar12[6]) && (0 < (int)fVar10)) && ((float)(iVar6 + 1) != fVar13)) {
            bVar2 = true;
          }
          if (((fVar10 == pfVar12[9]) && (0 < (int)fVar10)) && ((float)(iVar6 + 2) != fVar13)) {
            bVar2 = true;
          }
          if (((fVar10 == pfVar12[0xc]) && (0 < (int)fVar10)) && ((float)(iVar6 + 3) != fVar13)) {
            bVar2 = true;
          }
          iVar1 = iVar6 + 4;
          pfVar12 = pfVar12 + 0x12;
          iVar6 = iVar6 + 6;
        } while (iVar1 < 0xc);
        if (fVar10 == 8.40779e-45) {
          uStack_7c = local_70;
        }
        else {
          uStack_7c = *(uint *)local_1c[(int)fVar10];
        }
        if (bVar2) {
          cVar5 = FUN_004a2c48();
          if (fVar10 == 8.40779e-45) {
            cVar5 = cVar5 << 1;
          }
          cVar11 = (char)uStack_7c + cVar5;
          uStack_7c._1_1_ = uStack_7c._1_1_ + cVar5;
          uStack_7c._2_1_ = uStack_7c._2_1_ + cVar5;
          uStack_7c._0_2_ = CONCAT11(uStack_7c._1_1_,cVar11);
          uStack_7c = (uint)CONCAT12(uStack_7c._2_1_,(undefined2)uStack_7c);
        }
        if (fVar10 == 8.40779e-45) {
          FUN_0042bcb0(fVar13,pfVar14[-1],*pfVar14 - _DAT_005cc320,uStack_7c,1);
        }
        else {
          FUN_0042bcb0(fVar13,pfVar14[-1],*pfVar14 - _DAT_005cc320,uStack_7c,0);
        }
      }
      FUN_004282a0(0x36,0x3f4ccccd);
      iVar6 = FUN_004a2c48();
      fStack_4c = (float)(iVar6 / 2 + 4);
      uStack_7c = local_6c << 0x18;
      if (DAT_007f0fe8 == 0) {
        fVar10 = *pfVar14;
        fVar3 = (float)(int)fStack_4c + pfVar14[-1];
        acStack_78[0] = SUB41(fVar13,0);
      }
      else {
        fVar10 = (float)((int)fVar13 + -6);
        if ((int)pfVar14 < 0x67eb40) {
          fVar10 = fVar13;
        }
        if (5 < (int)fVar10) {
          fVar10 = (float)((int)fVar10 + -6);
        }
        fVar3 = (float)(int)fStack_4c + pfVar14[-1];
        acStack_78[0] = SUB41(fVar10,0);
        fVar10 = *pfVar14;
      }
      acStack_78[0] = acStack_78[0] + '1';
      acStack_78[1] = 0;
      FUN_00427f00(acStack_78,fVar3 + _DAT_005cc9b0,fVar10,uStack_7c,0x3f19999a,0);
    }
    pfStack_40 = pfVar14 + 3;
    fStack_4c = (float)((int)fVar13 + 1);
  } while ((int)pfStack_40 < 0x67eb88);
  (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
  return;
}

