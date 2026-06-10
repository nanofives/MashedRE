
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-07] Championship / cup progress screen renderer */

void FUN_00434720(void)

{
  uint uVar1;
  int iVar2;
  short sVar3;
  float fVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  short *psVar8;
  int iVar9;
  float fVar10;
  unkbyte10 Var11;
  float10 extraout_ST0;
  float10 fVar12;
  float10 fVar13;
  uint unaff_retaddr;
  undefined4 uVar14;
  undefined *puVar15;
  float fStack_20c;
  float fStack_208;
  float fStack_204;
  float fStack_200;
  float fStack_1fc;
  undefined4 uStack_1f8;
  float fStack_1f4;
  float fStack_1f0;
  float fStack_1ec;
  float fStack_1e8;
  undefined1 uStack_1e4;
  undefined1 uStack_1e3;
  undefined1 uStack_1e2;
  int iStack_1e1;
  short *psStack_1dc;
  float fStack_1d8;
  float fStack_1d4;
  undefined4 uStack_1d0;
  undefined4 local_1cc;
  float fStack_1c8;
  float fStack_1c4;
  float fStack_1c0;
  undefined1 auStack_1bc [4];
  undefined4 uStack_1b8;
  undefined1 local_1b4;
  undefined1 local_1b3;
  undefined1 local_1b2;
  byte local_1b1;
  int local_1b0;
  int iStack_1ac;
  float fStack_1a8;
  undefined4 uStack_1a4;
  undefined4 local_1a0;
  int iStack_19c;
  int iStack_198;
  undefined4 uStack_194;
  int iStack_190;
  int iStack_18c;
  undefined4 uStack_188;
  float fStack_184;
  float fStack_180;
  float fStack_17c;
  float fStack_178;
  float fStack_174;
  undefined4 uStack_170;
  float fStack_16c;
  undefined4 uStack_168;
  undefined4 uStack_164;
  undefined1 auStack_160 [32];
  undefined1 auStack_140 [32];
  undefined1 auStack_120 [68];
  int iStack_dc;
  double dStack_d8;
  undefined1 auStack_d0 [128];
  undefined1 auStack_50 [68];
  uint local_c;
  
  local_c = DAT_00616038 ^ unaff_retaddr;
  local_1b4 = 0xff;
  local_1b1 = 0xff;
  local_1a0 = 0xffffffff;
  local_1cc = local_1cc & 0xff000000;
  local_1b3 = 0;
  local_1b2 = 0x30;
  if (DAT_007f0fd0 == 2) {
    DAT_0067ea64 = 0;
  }
  FUN_0040e3a0(0,&local_1b4);
  local_1b0 = 1;
  if ((((DAT_0067e9fc == 3) || (DAT_0067e9fc == 4)) || (DAT_0067e9fc == 5)) &&
     (((DAT_007f0fd0 == 10 || (DAT_007f0fd0 == 9)) ||
      ((DAT_007f0fd0 == 8 || ((DAT_007f0fd0 == 7 || (DAT_007f0fd0 == 5)))))))) {
    local_1b0 = 0;
  }
  switch(DAT_0067ea6c) {
  case 0:
  case 1:
  case 2:
    FUN_0040b540(&DAT_0067ecbc);
    goto LAB_004347ee;
  case 3:
  case 4:
    if (DAT_0067ecdc == 2) {
      FUN_0040b620();
    }
    else {
      FUN_0040b460(&DAT_0067ecbc);
    }
  case 5:
    if (DAT_0067ea6c == 5) {
      local_1b1 = DAT_0067e7cc;
    }
    else {
LAB_004347ee:
      local_1b1 = FUN_004a2c48();
    }
  }
  (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
  fVar4 = (float)FUN_0040e340();
  uStack_188 = 3;
  uStack_1b8 = 2;
  uStack_170 = 1;
  if (fVar4 == 4.2039e-45) {
    uStack_188 = 2;
    uStack_1b8 = 1;
    uStack_170 = 0;
  }
  else if (fVar4 == 2.8026e-45) {
    uStack_188 = 1;
    uStack_1b8 = 0;
  }
  if (DAT_0067ea64 != 0) {
    uStack_188 = 1;
    uStack_1b8 = 0;
  }
  fStack_1ec = (float)DAT_0067ed68;
  if (DAT_0067ed68 < 0x3c) {
    fStack_1ec = 8.40779e-44;
  }
  fStack_20c = (float)(int)fStack_1ec;
  psStack_1dc = (short *)(600 - (int)fStack_1ec);
  fStack_1fc = (float)(int)psStack_1dc;
  uStack_1f8 = 0x41d00000;
  uStack_1e4 = 0xf0;
  uStack_1e3 = 0x6e;
  uStack_1e2 = 0x14;
  iStack_1e1 = CONCAT31(iStack_1e1._1_3_,DAT_0067e7cc);
  uStack_1d0 = CONCAT31(uStack_1d0._1_3_,DAT_0067e7cc);
  local_1cc = CONCAT13(DAT_0067e7cc,(undefined3)local_1cc);
  fStack_16c = fVar4;
  if (DAT_0067e9fc != 2) {
    FUN_0042f8d0(fStack_20c,0x42c80000,fStack_1fc,0x41d00000);
  }
  iVar2 = local_1cc;
  if (DAT_0067ea6c < 3) {
    if (DAT_0067ea64 == 0) {
      FUN_00427e00(0xa1,fStack_20c + _DAT_005cc55c,0x42e20000,local_1cc,0x3f333333,0);
    }
    else {
      FUN_00427e00(0x6b,fStack_20c + _DAT_005cc55c,0x42e20000,local_1cc,0x3f333333,0);
    }
    goto LAB_00434bde;
  }
  if ((DAT_0067e9fc != 6) && (DAT_0067e9fc != 2)) goto LAB_00434bde;
  if (DAT_0067ea64 == 0) {
    if (DAT_0067e9fc != 2) {
      uVar6 = 0xa1;
      goto LAB_004349f2;
    }
  }
  else {
    uVar6 = 0x6b;
LAB_004349f2:
    FUN_00427e00(uVar6,fStack_20c + _DAT_005cc55c,0x42e20000,local_1cc,0x3f333333,0);
    if (DAT_0067e9fc != 2) {
      fVar10 = fStack_20c + _DAT_005cd9e8;
      psStack_1dc = (short *)fVar10;
      FUN_00427e00(0x3d,fVar10,0x42d60000,iVar2,0x3f000000,2);
      FUN_00427e00(0x72,fVar10,0x42ee0000,iVar2,0x3f000000,2);
      FUN_00427e00(0x6d,fStack_20c + _DAT_005cd9e4,0x42e20000,iVar2,0x3f333333,2);
      FUN_00427e00(0x6e,fStack_20c + _DAT_005cd9e0,0x42e20000,iVar2,0x3f333333,2);
      if ((DAT_0067ea64 == 0) &&
         ((((int)fVar4 < 3 ||
           (FUN_00427e00(0x6f,fStack_20c + _DAT_005cd9dc,0x42e20000,iVar2,0x3f333333,2),
           DAT_0067ea64 == 0)) && (3 < (int)fVar4)))) {
        FUN_00427e00(0x70,fStack_20c + _DAT_005cd9d8,0x42e20000,iVar2,0x3f333333,2);
      }
      goto LAB_00434bde;
    }
  }
  fVar4 = fStack_1fc;
  fStack_178 = 26.0;
  fStack_1d4 = 100.0;
  fStack_17c = fStack_1fc;
  fStack_1d8 = fStack_20c;
  FUN_0042f8d0(fStack_20c,0x42c80000,fStack_1fc,0x41d00000);
  FUN_00427e00(0x14c,fStack_20c + _DAT_005cc55c,0x42e20000,iVar2,0x3f000000,0);
  fStack_1d4 = 132.0;
  FUN_0042f8d0(fStack_1d8,0x43040000,fVar4,0x41d00000);
  fStack_1d4 = 164.0;
  FUN_0042f8d0(fStack_1d8,0x43240000,fVar4,0x41d00000);
  fStack_1d4 = 196.0;
  FUN_0042f8d0(fStack_1d8,0x43440000,fVar4,0x41d00000);
  fStack_1d4 = 100.0;
LAB_00434bde:
  if ((DAT_0067ecdc == 0) || (DAT_0067ea6c < 3)) {
    if (DAT_0067e9fc == 2) {
      fVar4 = fStack_1d4 + _DAT_005cc9b8;
      psStack_1dc = (short *)fVar4;
      FUN_00427e00(0xbe,0x43870000,fVar4,iVar2,0x3f000000,2);
      fVar10 = fStack_1d4 + _DAT_005cd9d4;
      psStack_1dc = (short *)fVar10;
      FUN_00427e00(0x14d,0x43870000,fVar10,iVar2,0x3f000000,2);
      FUN_00427e00(0xa1,0x43c80000,fVar4,iVar2,0x3f000000,2);
      FUN_00427e00(0x14d,0x43c80000,fVar10,iVar2,0x3f000000,2);
      fVar4 = fStack_1d4 + _DAT_005cc354;
      uVar6 = 0x3f000000;
      uVar14 = 0x14e;
    }
    else {
      uVar6 = 0x3f333333;
      fVar4 = 113.0;
      if (DAT_0067e9fc == 6) {
        uVar14 = 0x6c;
      }
      else {
        uVar14 = 0x24e;
      }
    }
    FUN_00427e00(uVar14,0x44138000,fVar4,iVar2,uVar6,1);
  }
  else {
    if (DAT_0067ecdc == 1) {
      FUN_00427e00(0xa6,0x4414c000,0x42d60000,iVar2,0x3f000000,1);
      FUN_00427e00(0xa8,0x4414c000,0x42ee0000,iVar2,0x3f000000,1);
    }
    if (DAT_0067ecdc == 2) {
      FUN_00427e00(0xa7,0x4414c000,0x42d60000,iVar2,0x3f000000,1);
      FUN_00427e00(0xa8,0x4414c000,0x42ee0000,iVar2,0x3f000000,1);
    }
  }
  fVar4 = 1.4013e-45;
  iVar5 = 4;
  fStack_1a8 = -NAN;
  fStack_208 = 148.0;
  uStack_164 = 0x42000000;
  uStack_168 = 0x42000000;
  fStack_1ec = 1.4013e-45;
  psStack_1dc = (short *)&DAT_00000004;
  if (DAT_007f0fd0 == 8) {
    iVar5 = 1;
    psStack_1dc = (short *)0x1;
  }
  iStack_1ac = 0;
  fVar10 = fStack_1f4;
  if (iVar5 != 0) {
    do {
      if (((iStack_1ac == 0) || (DAT_0067ed6c == 0)) &&
         (iVar5 = *(int *)(&DAT_0067ecbc + iStack_1ac * 4), -1 < (int)(&DAT_007f1a14)[iVar5 * 4])) {
        if (DAT_0067ea64 != 0) {
          if ((float)(&DAT_007f1a18)[iVar5 * 4] == fStack_1a8) {
            fStack_208 = fStack_208 - _DAT_005cd120;
            uVar6 = FUN_0042fab0((&DAT_007f1a1c)[iVar5 * 4]);
            fStack_1ec = (float)((int)fVar4 * 0x24 + 8);
            fStack_20c = fStack_1ec + fStack_20c;
            FUN_004739f0(uVar6,fStack_20c,fStack_208,uStack_168,uStack_164,local_1a0,0,0x3f800000,0,
                         0x3f800000,1,1);
            fStack_20c = fStack_20c - fStack_1ec;
            fStack_1ec = (float)((int)fVar4 + 1);
            fStack_208 = fStack_208 + _DAT_005cd120;
            fVar4 = fStack_1ec;
            goto LAB_0043614d;
          }
          fStack_1ec = 1.4013e-45;
          fStack_1a8 = (float)(&DAT_007f1a18)[iVar5 * 4];
        }
        uStack_1f8 = 0x41d00000;
        uStack_1e4 = 0x2e;
        uStack_1e3 = 0x31;
        uStack_1e2 = 0x92;
        iStack_1e1 = CONCAT31(iStack_1e1._1_3_,DAT_0067e7cc);
        if (DAT_0067e9fc != 2) {
          FUN_0042f8d0(fStack_20c,fStack_208,fStack_1fc,0x41d00000);
        }
        fStack_208 = fStack_208 + _DAT_005cc574;
        uStack_1f8 = 0x40800000;
        if ((DAT_0067ea64 == 0) && ((DAT_0067e9fc == 0) == true)) {
          FUN_0042f8d0(fStack_20c,fStack_208,fStack_1fc,0x40800000);
        }
        iVar5 = iStack_1ac;
        uVar6 = FUN_0042fab0((&DAT_007f1a1c)[*(int *)(&DAT_0067ecbc + iStack_1ac * 4) * 4]);
        if (DAT_007f0fd0 == 8) {
          uVar6 = FUN_0042fab0(DAT_007f1a1c);
        }
        fStack_20c = fStack_20c + _DAT_005cc9f4;
        fStack_208 = fStack_208 - _DAT_005cc35c;
        if ((DAT_0067e9fc != 2) && (*(int *)(&DAT_0067ecbc + iVar5 * 4) != -1)) {
          FUN_004739f0(uVar6,fStack_20c,fStack_208,uStack_168,uStack_164,local_1a0,0,0x3f800000,0,
                       0x3f800000,1,1);
        }
        fStack_20c = fStack_20c - _DAT_005cc9f4;
        fStack_208 = fStack_208 + _DAT_005cc574;
        switch(DAT_0067ea6c) {
        case 0:
        case 1:
          if (iVar5 == 0) {
            fVar10 = 1.52742e-43;
            uVar6 = uStack_188;
LAB_00435128:
            fStack_1f4 = fVar10;
            FUN_004a2b60(auStack_1bc,&DAT_005cd794,uVar6);
          }
          else {
            if (iVar5 == 1) {
              fVar10 = 1.54143e-43;
              uVar6 = uStack_1b8;
              goto LAB_00435128;
            }
            if (iVar5 == 2) {
              iVar9 = FUN_0040b6d0(DAT_0067ecc0);
              iVar7 = FUN_0040b6d0(DAT_0067ecc4);
              if (iVar7 == iVar9) {
                fVar10 = 1.54143e-43;
                uVar6 = uStack_1b8;
              }
              else {
                fVar10 = 1.55544e-43;
                uVar6 = uStack_170;
              }
              goto LAB_00435128;
            }
            if (iVar5 == 3) {
              iVar9 = FUN_0040b6d0(DAT_0067ecc8);
              iVar7 = FUN_0040b6d0(DAT_0067ecc0);
              if (iVar9 == iVar7) {
                fVar10 = 1.54143e-43;
                uVar6 = uStack_1b8;
              }
              else {
                iVar9 = FUN_0040b6d0(DAT_0067ecc8);
                iVar7 = FUN_0040b6d0(DAT_0067ecc4);
                if (iVar9 == iVar7) {
                  fVar10 = 1.55544e-43;
                  uVar6 = uStack_170;
                }
                else {
                  fVar10 = 1.56945e-43;
                  uVar6 = 0;
                }
              }
              goto LAB_00435128;
            }
          }
          if ((DAT_007f0fd0 == 4) || (DAT_007f0fd0 == 7)) {
            fVar10 = (float)(iVar5 + 0x6d);
            fStack_1f4 = fVar10;
          }
          if ((DAT_0067ea64 != 0) && (iVar5 != 0)) {
            FUN_004a2b60(auStack_120,&PTR_DAT_005cd9d0);
            fVar10 = 1.54143e-43;
            fStack_1f4 = 1.54143e-43;
            FUN_004a2b60(auStack_1bc,&DAT_005cd794,uStack_1b8);
          }
          if (DAT_0067e9fc == 6) {
            fVar4 = fStack_208 + _DAT_005cc9b0;
            fStack_1e8 = fVar4;
            FUN_00427e00(fVar10,fStack_20c + _DAT_005cd9cc,fVar4,iVar2,0x3f19999a,1);
            FUN_00427f00(auStack_1bc,fStack_20c + _DAT_005cd9c8,fVar4,iVar2,0x3f19999a,2);
          }
          else {
            if (((DAT_007f0fd0 == 10) || (DAT_007f0fd0 == 8)) || (DAT_007f0fd0 == 5)) {
              fVar10 = (float)((-(uint)(DAT_007f0fcc != 0) & 0xfffffe4c) + 0x221);
              fStack_1f4 = fVar10;
            }
            FUN_00427e00(fVar10,fStack_20c + _DAT_005cd9c8,fStack_208 + _DAT_005cc9b0,iVar2,
                         0x3f19999a,1);
          }
          break;
        case 3:
        case 4:
        case 5:
          if (DAT_0067e9fc == 2) {
            DAT_0067ed2c = 0;
            iVar9 = FUN_00429870();
            if ((iVar9 != 0) && (DAT_0067ed6c == 0)) {
              FUN_00429a30();
              DAT_0067ed2c = 1;
            }
            DAT_0067ecd4 = FUN_00429a80(0);
            DAT_0067eccc = FUN_00429a90(0);
            FUN_00429a70(0);
            DAT_0067f0cc = FUN_004a2c48();
            DAT_0067ecd8 = FUN_00429a80(1);
            DAT_0067ecd0 = FUN_00429a90(1);
            FUN_00429a70(1);
            DAT_0067f0d0 = FUN_004a2c48();
            if (DAT_0067e9fc != 2) goto LAB_00435bb9;
            FUN_004a2b60(auStack_120,&DAT_005cd794,1);
            fStack_1e8 = fStack_1d8 + _DAT_005cc55c;
            FUN_00427f00(auStack_120,fStack_1e8,fStack_1d4 + _DAT_005ccacc,iVar2,0x3f333333,0);
            FUN_00430b30(&fStack_184,&iStack_19c,&iStack_190);
            FUN_0042d290(iStack_19c,fStack_184 * _DAT_005cc568,auStack_160,auStack_140);
            FUN_004a2b60(auStack_120,"%d%s%s%s%s",iStack_190,&DAT_005cd9c4,auStack_160,&DAT_005cd9c4
                         ,auStack_140);
            fVar4 = fStack_1d4 + _DAT_005cd9b4;
            fStack_200 = fVar4;
            FUN_00427f00(auStack_120,0x43870000,fVar4,iVar2,0x3f19999a,2);
            FUN_00430b30(&fStack_180,&iStack_198,&iStack_18c);
            fStack_200 = fStack_180 * _DAT_005cc568;
            FUN_0042d290(iStack_198,fStack_200,auStack_160,auStack_140);
            iVar5 = iStack_18c;
            FUN_004a2b60(auStack_120,"%d%s%s%s%s",iStack_18c,&DAT_005cd9c4,auStack_160,&DAT_005cd9c4
                         ,auStack_140);
            FUN_00427f00(auStack_120,0x43c80000,fVar4,iVar2,0x3f19999a,2);
            fStack_1f0 = (float)((iVar5 * 0x3c + iStack_198) * 100);
            uVar6 = FUN_004a2c48(&local_1cc,&fStack_204,&uStack_1a4,&uStack_194);
            fStack_1f0 = (float)((iStack_190 * 0x3c + iStack_19c) * 100);
            uVar6 = FUN_004a2c48(uVar6);
            FUN_0042d300(uVar6);
            FUN_0042d290(uStack_1a4,uStack_194,auStack_160,auStack_140);
            if (local_1cc == 2) {
              FUN_004a2b60(auStack_120,"%d%s%s%s%s",fStack_204,&DAT_005cd9c4,auStack_160,
                           &DAT_005cd9c4,auStack_140);
            }
            else {
              if (local_1cc == 1) {
                puVar15 = &DAT_005cd9b0;
              }
              else {
                puVar15 = &DAT_005cd99c;
              }
              FUN_004a2b60(auStack_120,"%s%d%s%s%s%s",puVar15,fStack_204,&DAT_005cd9c4,auStack_160,
                           &DAT_005cd9c4,auStack_140);
            }
            FUN_00427f00(auStack_120,0x44138000,fVar4,iVar2,0x3f19999a,1);
            FUN_004a2b60(auStack_120,&DAT_005cd794,2);
            FUN_00427f00(auStack_120,fStack_1e8,fStack_1d4 + _DAT_005cd998,iVar2,0x3f333333,0);
            FUN_00430b30(&fStack_184,&iStack_19c,&iStack_190);
            FUN_0042d290(iStack_19c,fStack_184 * _DAT_005cc568,auStack_160,auStack_140);
            FUN_004a2b60(auStack_120,"%d%s%s%s%s",iStack_190,&DAT_005cd9c4,auStack_160,&DAT_005cd9c4
                         ,auStack_140);
            fVar4 = fStack_1d4 + _DAT_005cd994;
            fStack_1f0 = fVar4;
            FUN_00427f00(auStack_120,0x43870000,fVar4,iVar2,0x3f19999a,2);
            FUN_00430b30(&fStack_180,&iStack_198,&iStack_18c);
            fStack_200 = fStack_180 * _DAT_005cc568;
            FUN_0042d290(iStack_198,fStack_200,auStack_160,auStack_140);
            iVar5 = iStack_18c;
            FUN_004a2b60(auStack_120,"%d%s%s%s%s",iStack_18c,&DAT_005cd9c4,auStack_160,&DAT_005cd9c4
                         ,auStack_140);
            FUN_00427f00(auStack_120,0x43c80000,fVar4,iVar2,0x3f19999a,2);
            fStack_1f0 = (float)((iVar5 * 0x3c + iStack_198) * 100);
            uVar6 = FUN_004a2c48(&local_1cc,&fStack_204,&uStack_1a4,&uStack_194);
            fStack_1f0 = (float)((iStack_190 * 0x3c + iStack_19c) * 100);
            uVar6 = FUN_004a2c48(uVar6);
            FUN_0042d300(uVar6);
            FUN_0042d290(uStack_1a4,uStack_194,auStack_160,auStack_140);
            if (local_1cc == 2) {
              FUN_004a2b60(auStack_120,"%d%s%s%s%s",fStack_204,&DAT_005cd9c4,auStack_160,
                           &DAT_005cd9c4,auStack_140);
            }
            else {
              if (local_1cc == 1) {
                puVar15 = &DAT_005cd9b0;
              }
              else {
                puVar15 = &DAT_005cd99c;
              }
              FUN_004a2b60(auStack_120,"%s%d%s%s%s%s",puVar15,fStack_204,&DAT_005cd9c4,auStack_160,
                           &DAT_005cd9c4,auStack_140);
            }
            FUN_00427f00(auStack_120,0x44138000,fVar4,iVar2,0x3f19999a,1);
            FUN_00427e00(0x14f,fStack_1e8,fStack_1d4 + _DAT_005cd990,iVar2,0x3f333333,0);
            FUN_0042d290(DAT_0067ecd0,(float)DAT_0067f0d0,auStack_160,auStack_140);
            FUN_004a2b60(auStack_120,"%d%s%s%s%s",DAT_0067ecd8,&DAT_005cd9c4,auStack_160,
                         &DAT_005cd9c4,auStack_140);
            fVar4 = fStack_1d4 + _DAT_005cd98c;
            fStack_1f0 = fVar4;
            FUN_00427f00(auStack_120,0x43870000,fVar4,iVar2,0x3f19999a,2);
            FUN_0042d290(DAT_0067eccc,(float)DAT_0067f0cc,auStack_160,auStack_140);
            FUN_004a2b60(auStack_120,"%d%s%s%s%s",DAT_0067ecd4,&DAT_005cd9c4,auStack_160,
                         &DAT_005cd9c4,auStack_140);
            FUN_00427f00(auStack_120,0x43c80000,fVar4,iVar2,0x3f19999a,2);
            FUN_0042d300((DAT_0067ecd8 * 0x3c + DAT_0067ecd0) * 100 + DAT_0067f0d0,
                         (DAT_0067ecd4 * 0x3c + DAT_0067eccc) * 100 + DAT_0067f0cc,&local_1cc,
                         &fStack_204,&uStack_1a4,&uStack_194);
            FUN_0042d290(uStack_1a4,uStack_194,auStack_160,auStack_140);
            if (local_1cc == 2) {
              FUN_004a2b60(auStack_120,"%d%s%s%s%s",fStack_204,&DAT_005cd9c4,auStack_160,
                           &DAT_005cd9c4,auStack_140);
            }
            else {
              if (local_1cc == 1) {
                puVar15 = &DAT_005cd9b0;
              }
              else {
                puVar15 = &DAT_005cd99c;
              }
              FUN_004a2b60(auStack_120,"%s%d%s%s%s%s",puVar15,fStack_204,&DAT_005cd9c4,auStack_160,
                           &DAT_005cd9c4,auStack_140);
            }
            FUN_00427f00(auStack_120,0x44138000,fVar4,iVar2,0x3f19999a,1);
            if (DAT_0067ed2c == 0) break;
            fVar12 = (float10)FUN_00428320(auStack_120,0x3f19999a);
            fVar4 = 1.4013e-45;
            fStack_200 = 1.4013e-45;
            fStack_1c8 = (float)(fVar12 * (float10)_DAT_005cc32c + (float10)_DAT_005cc9f4);
LAB_00435d57:
            _DAT_0067ea04 = DAT_007f1004 + _DAT_0067ea04;
            if (_DAT_005cd988 < _DAT_0067ea04) {
              if (_DAT_0067ed74 < _DAT_005cd0e4) {
                _DAT_0067ed74 = _DAT_0067ed74 + _DAT_005ccacc;
              }
              else {
                _DAT_0067ed74 = 359.0;
                if (_DAT_0067ea04 <= _DAT_005cc950) goto LAB_00435dce;
                _DAT_0067ed74 = 0.0;
              }
              _DAT_0067ea04 = 0.0;
            }
LAB_00435dce:
            iStack_dc = FUN_004a2c48();
            uStack_1e4 = 0xf7;
            uStack_1e3 = 0x94;
            uStack_1e2 = 0x1d;
            fStack_174 = 38.0;
            if (fVar4 == 0.0) {
              fStack_174 = 26.0;
            }
            fStack_1e8 = 0.0;
            if (-1 < iStack_dc) {
              dStack_d8 = (double)fStack_174;
              do {
                fVar4 = fStack_1e8;
                if (DAT_0067e9fc == 2) {
                  fStack_1c4 = 392.0;
                  fStack_1c0 = ((fStack_1d4 + _DAT_005cd8d8) - _DAT_005cc9b8) + _DAT_005cd980;
                }
                else {
                  fStack_1c4 = (((fStack_1fc + fStack_20c) - _DAT_005cc358) - fStack_1c8) -
                               _DAT_005cc9b8;
                  fStack_1c0 = (fStack_208 + _DAT_005cd8d8) - _DAT_005cc9b8;
                }
                fVar10 = (float)(int)fStack_1e8 * _DAT_005ccacc;
                fStack_1f0 = fVar10;
                Var11 = FUN_00474e60(fVar10);
                fVar12 = (float10)fsin(Var11);
                fStack_1c4 = (float)(fVar12 * (float10)dStack_d8 * (float10)_DAT_005cd978 +
                                    (float10)fStack_1c4);
                dStack_d8 = (double)fStack_174;
                Var11 = FUN_00474e60(fVar10);
                fcos(Var11);
                sVar3 = FUN_0042b8b0();
                fStack_1f0 = (float)(int)sVar3;
                fStack_1c4 = (float)(int)fStack_1f0 * fStack_1c4 * _DAT_005cd5a8;
                sVar3 = FUN_0042b8c0();
                fStack_1f0 = (float)(int)sVar3;
                fStack_1c0 = (float)((float10)(int)fStack_1f0 * extraout_ST0 *
                                    (float10)_DAT_005cc560);
                sVar3 = FUN_0042b8b0();
                fStack_1f0 = (float)(int)sVar3;
                fStack_17c = (float)(int)fStack_1f0 * _DAT_005cc9a4;
                sVar3 = FUN_0042b8c0();
                fStack_1f0 = (float)(int)sVar3;
                uStack_1e2 = 0;
                uStack_1e3 = 0;
                uStack_1e4 = 0;
                fVar10 = (float)(int)fStack_1f0 * _DAT_005cc324;
                fStack_178 = fVar10;
                uVar6 = FUN_0040bb50("Star",fStack_1c4,fStack_1c0,fStack_17c,fVar10,
                                     iStack_1e1 << 0x18,1);
                FUN_00473870(uVar6);
                fStack_1c4 = fStack_1c4 - _DAT_005cc574;
                uStack_1e4 = 0xf7;
                uStack_1e3 = 0x94;
                uStack_1e2 = 0x1d;
                fStack_1c0 = fStack_1c0 - _DAT_005cc574;
                uVar6 = FUN_0040bb50("Star",fStack_1c4,fStack_1c0,fStack_17c,fVar10,
                                     CONCAT13((undefined1)iStack_1e1,0x1d94f7),1);
                FUN_00473870(uVar6);
                fStack_1e8 = (float)((int)fVar4 + 1);
                fVar4 = fStack_200;
              } while ((int)fStack_1e8 <= iStack_dc);
            }
            if (fVar4 == 0.0) {
LAB_0043604f:
              uVar6 = FUN_0040b7a0();
              FUN_004a2b60(auStack_120,&DAT_005cd794,uVar6);
              fVar4 = fStack_208 + _DAT_005cc9b0;
              fStack_1e8 = fVar4;
              FUN_00427f00(auStack_120,fStack_20c + _DAT_005cd9e8,fVar4,iVar2,0x3f19999a,2);
              if (DAT_0067ea64 == 0) {
                fStack_200 = fStack_16c;
              }
              else {
                fStack_200 = 2.8026e-45;
              }
              iVar5 = 0;
              fStack_1e8 = 0.0;
              do {
                if (iVar5 < (int)fStack_200) {
                  uVar6 = FUN_0040b7b0(*(undefined4 *)(&DAT_0067ecbc + iStack_1ac * 4),iVar5);
                  FUN_004a2b60(auStack_120,&DAT_005cd794,uVar6);
                  FUN_00427f00(auStack_120,(float)(int)fStack_1e8 + fStack_20c + _DAT_005cd9e4,fVar4
                               ,iVar2,0x3f19999a,2);
                }
                fStack_1e8 = (float)((int)fStack_1e8 + 0x46);
                iVar5 = iVar5 + 1;
              } while ((int)fStack_1e8 < 0x118);
            }
          }
          else {
LAB_00435bb9:
            if ((DAT_0067e9fc == 6) || (DAT_0067e9fc == 2)) {
              if (DAT_0067ecdc == 2) {
                uVar6 = FUN_0040b6b0(*(undefined4 *)(&DAT_0067ecbc + iVar5 * 4));
              }
              else {
                uVar6 = FUN_0040b6c0(*(undefined4 *)(&DAT_0067ecbc + iVar5 * 4));
              }
              FUN_004a2b60(auStack_120,&DAT_005cd794,uVar6);
              if (DAT_0067ecdc == 0) {
                FUN_00427f00(auStack_120,(fStack_1fc + fStack_20c) - _DAT_005cd96c,
                             fStack_208 + _DAT_005cc9b0,iVar2,0x3f19999a,1);
              }
              else {
                uVar14 = DAT_005f76c4;
                if ((DAT_0067ecdc != 1) && (uVar14 = uVar6, DAT_0067ecdc == 2)) {
                  uVar14 = DAT_005f76c8;
                }
                FUN_004a2b60(auStack_50,&DAT_005cd794,uVar14);
                fStack_1e8 = (fStack_1fc + fStack_20c) - _DAT_005cc358;
                FUN_00427f00(auStack_50,fStack_1e8,fStack_208 + _DAT_005cd9d4,iVar2,0x3f000000,1);
                fVar12 = (float10)FUN_00428320(auStack_50,0x3f000000);
                fStack_200 = (float)fVar12;
                fStack_1c8 = fStack_200;
                FUN_00427f00(&DAT_005cc4d8,fStack_1e8 - fStack_200,fStack_208 + _DAT_005cd8d8,iVar2,
                             0x3f000000,1);
                fVar12 = (float10)FUN_00428320(&DAT_005cc4d8,0x3f000000);
                fStack_200 = (float)(fVar12 + (float10)fStack_200);
                fVar12 = (float10)FUN_00428320(&DAT_005cc4d8,0x3f000000);
                fStack_1c8 = (float)(fVar12 * (float10)_DAT_005cc32c + (float10)fStack_1c8);
                FUN_00427f00(auStack_120,(fStack_1e8 - fStack_200) - _DAT_005cc574,
                             fStack_208 + _DAT_005cc354,iVar2,0x3f000000,1);
                fVar4 = 0.0;
                fStack_200 = 0.0;
                if (DAT_0067ed4c == *(int *)(&DAT_0067ecbc + iVar5 * 4) + 1) goto LAB_00435d57;
              }
              goto LAB_0043604f;
            }
          }
        }
        fStack_208 = fStack_208 + _DAT_005cd784;
        fVar10 = fStack_1f4;
        fVar4 = fStack_1ec;
      }
LAB_0043614d:
      iStack_1ac = iStack_1ac + 1;
    } while (iStack_1ac < (int)psStack_1dc);
  }
  if (local_1b0 == 0) {
    uVar6 = CONCAT13((undefined1)iStack_1e1,CONCAT12(uStack_1e2,CONCAT11(uStack_1e3,uStack_1e4)));
  }
  else {
    uStack_1f8 = 0x42700000;
    fStack_1fc = 520.0;
    iStack_1e1 = CONCAT31(iStack_1e1._1_3_,DAT_0067e7cc);
    uStack_1e4 = 0xf0;
    uStack_1e3 = 0x6e;
    uStack_1e2 = 0x14;
    FUN_0042f8d0(0x42700000,0x43aa0000,0x44020000,0x42700000);
    fStack_204 = ((float)DAT_0067ebcc - _DAT_005cd618) - _DAT_005cc9fc;
    if (fStack_204 < _DAT_005cc568) {
      if (fStack_204 <= _DAT_005cd968) {
        fStack_204 = fStack_204 + _DAT_005cd964;
      }
      else {
        fStack_204 = 100.0;
      }
    }
    fStack_200 = 0.0;
    if (0 < DAT_0067ebc8) {
      do {
        fVar4 = fStack_200;
        uVar1 = (&DAT_0067ebd0)[(int)fStack_200];
        fStack_1ec = (float)uVar1;
        fVar12 = (float10)FUN_004282a0(uVar1,0x3f19999a);
        fStack_1f4 = (float)(fVar12 + (float10)fStack_204);
        if ((float10)_DAT_005cc728 <= fVar12 + (float10)fStack_204) {
          if (fStack_204 < _DAT_005cc728) {
            psVar8 = (short *)FUN_00427780(fStack_1ec);
            fStack_1a8 = _DAT_005cc728 - fStack_204;
            auStack_d0[0] = (undefined1)psVar8[1];
            auStack_d0[1] = 0;
            fStack_1c8 = 0.0;
            iVar5 = 1;
            if (DAT_005d757c < fStack_1a8) {
              psStack_1dc = psVar8 + 2;
              do {
                fVar12 = (float10)FUN_00428320(auStack_d0,0x3f19999a);
                auStack_d0[0] = (undefined1)*psStack_1dc;
                fStack_1c8 = (float)(fVar12 + (float10)fStack_1c8);
                psStack_1dc = psStack_1dc + 1;
                iVar5 = iVar5 + 1;
              } while (fStack_1c8 < fStack_1a8);
            }
            iVar9 = 0;
            if (iVar5 < *psVar8 + 1) {
              do {
                auStack_d0[iVar9] = (char)psVar8[iVar5];
                iVar9 = iVar9 + 1;
                iVar5 = iVar5 + 1;
              } while (iVar5 < *psVar8 + 1);
            }
            auStack_d0[iVar9] = 0;
            FUN_00427f00(auStack_d0,(fStack_204 - _DAT_005cc728) + fStack_1c8 + _DAT_005cc728,
                         0x43bc0000,iVar2,0x3f19999a,0);
LAB_00436463:
            fVar12 = (float10)FUN_004282a0(fStack_1ec,0x3f19999a);
            goto LAB_00436479;
          }
          if (fStack_204 <= _DAT_005cd960) {
            if (fStack_1f4 <= _DAT_005cd960) {
              FUN_00427e00(uVar1,fStack_204,0x43bc0000,iVar2,0x3f19999a,0);
            }
            else {
              psVar8 = (short *)FUN_00427780(fStack_1ec);
              sVar3 = *psVar8;
              auStack_d0[1] = 0;
              if (_DAT_005cd960 < fStack_1f4) {
                do {
                  if (sVar3 == 0) break;
                  auStack_d0[0] = (undefined1)psVar8[sVar3];
                  sVar3 = sVar3 + -1;
                  fVar13 = (float10)FUN_00428320(auStack_d0,0x3f19999a);
                  fVar12 = (float10)fStack_1f4;
                  fStack_1f4 = (float)(fVar12 - fVar13);
                } while ((float10)_DAT_005cd960 < fVar12 - fVar13);
              }
              iVar5 = 0;
              if (0 < sVar3) {
                do {
                  psVar8 = psVar8 + 1;
                  auStack_d0[iVar5] = (char)*psVar8;
                  iVar5 = iVar5 + 1;
                } while (iVar5 < sVar3);
              }
              auStack_d0[iVar5] = 0;
              FUN_00427f00(auStack_d0,fStack_204,0x43bc0000,iVar2,0x3f19999a,0);
            }
            goto LAB_00436463;
          }
        }
        else {
          fVar12 = (float10)FUN_004282a0(uVar1,0x3f19999a);
          fStack_200 = fVar4;
LAB_00436479:
          fStack_204 = (float)(fVar12 + (float10)fStack_204 + (float10)_DAT_005cc9f4);
          fVar4 = fStack_200;
        }
        fStack_200 = (float)((int)fVar4 + 1);
      } while ((int)fStack_200 < DAT_0067ebc8);
    }
    uVar6 = CONCAT13((undefined1)iStack_1e1,CONCAT12(uStack_1e2,CONCAT11(uStack_1e3,uStack_1e4)));
    fStack_1fc = 10.0;
    FUN_00472c60(0x42700000,0x43aa0000,0x41200000,uStack_1f8,uVar6);
    iVar5 = iStack_1e1;
    fStack_1fc = 30.0;
    FUN_004736c0(0x428c0000,0x43aa0000,0x41f00000,uStack_1f8,uVar6,iStack_1e1,0,iStack_1e1,0);
    FUN_004736c0(0x44070000,0x43aa0000,fStack_1fc,uStack_1f8,uVar6,0,iVar5,0,iVar5);
    fStack_1fc = 10.0;
    FUN_00472c60(0x440e8000,0x43aa0000,0x41200000,uStack_1f8,uVar6);
    fStack_1fc = 20.0;
    FUN_004736c0(0x42200000,0x43aa0000,0x41a00000,uStack_1f8,uVar6,0,iVar5,0,iVar5);
    FUN_004736c0(0x44110000,0x43aa0000,fStack_1fc,uStack_1f8,uVar6,iVar5,0,iVar5,0);
    uStack_1f8 = 0x40000000;
    fStack_1fc = 640.0;
    FUN_00472c60(0,0x43ac0000,0x44200000,0x40000000,iVar2);
    fVar12 = (float10)FUN_004282a0(0x71,0x3f19999a);
    fStack_1f4 = (float)fVar12;
    fVar12 = (float10)FUN_004282a0(DAT_0067ea08 + 0xaa,0x3f19999a);
    if (DAT_0067e9fc == 2) {
      fStack_1f4 = 0.0;
      fVar12 = (float10)FUN_004282a0(0xc1,0x3f19999a);
    }
    uStack_1f8 = 0x41800000;
    fStack_1fc = (float)(fVar12 + (float10)fStack_1f4 + (float10)_DAT_005cc35c);
    FUN_0042f8d0(0x42a00000,0x43ac0000,fStack_1fc,0x41800000);
    if (DAT_0067e9fc == 2) {
      FUN_00427e00(0xc1,0x42a40000,0x43af8000,iVar2,0x3f19999a,0);
    }
    else {
      FUN_00427e00(0x71,0x42a40000,0x43af8000,iVar2,0x3f19999a,0);
      FUN_00427e00(DAT_0067ea08 + 0xaa,fStack_1f4 + _DAT_005cd95c,0x43af8000,iVar2,0x3f19999a,0);
    }
  }
  (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
  (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
  if (DAT_0067ea6c < 4) {
    fStack_16c = (float)(DAT_0067ed68 + -0x127);
    fStack_1fc = 256.0;
    uStack_1f8 = 0x435c0000;
    fStack_1ec = (float)((uint)fStack_1ec & 0xff000000);
    uStack_1d0._0_1_ = DAT_0067e7cc;
    if (0xa0 < DAT_0067e7cc) {
      uStack_1d0._0_1_ = 0xa0;
    }
    fVar4 = (float)(int)fStack_16c + _DAT_005cc9f4;
    FUN_004736c0(fVar4,0x42d80000,0x43800000,0x435c0000,fStack_1ec,uStack_1d0,uStack_1d0,uStack_1d0,
                 uStack_1d0);
    FUN_00472c60(fVar4 - _DAT_005cc9f4,0x42c80000,0x43800000,0x435c0000,uVar6);
  }
  return;
}

