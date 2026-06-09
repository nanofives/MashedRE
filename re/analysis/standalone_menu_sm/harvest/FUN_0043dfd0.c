
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-06-01] `void FUN_0043dfd0(void)` — large per-frame state-machine/tick (body
   0x0043dfd0.. */

void FUN_0043dfd0(void)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  bool bVar4;
  bool bVar5;
  bool bVar6;
  bool bVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  undefined4 *puVar12;
  int *piVar13;
  int iVar14;
  undefined4 *puVar15;
  int extraout_EDX;
  int iVar16;
  int *piVar17;
  int *piVar18;
  char cVar19;
  float10 fVar20;
  undefined4 uVar21;
  int local_2c;
  uint local_28;
  int local_24;
  int local_20;
  
  fVar20 = (float10)FUN_00431b30();
  if (fVar20 != (float10)_DAT_0067f1ac) {
    _DAT_0067f1ac = (float)fVar20;
    FUN_00495080((float)fVar20,0x3f800000);
  }
  _DAT_0067e840 = _DAT_0067e840 - _DAT_005cd18c;
  if (_DAT_0067e840 < DAT_005d757c) {
    _DAT_0067e840 = 0.0;
  }
  iVar8 = thunk_FUN_00493f70(0);
  if (iVar8 == 0) {
    thunk_FUN_00494a80(0,0,1);
  }
  else {
    thunk_FUN_00494480(0);
    FUN_00494f30();
  }
  FUN_0043c000();
  FUN_0040acd0(DAT_007f1004);
  if (DAT_0067f19c == 2) {
    DAT_0067f19c = 1;
    return;
  }
  DAT_0067f19c = 0;
  DAT_0067f1a0 = 0;
  DAT_0067f1a4 = 0;
  if (DAT_0067eca4 == 1) {
    iVar8 = FUN_004a2c48();
    if (DAT_0067eca8 + iVar8 < 0xff) {
      DAT_0067eca8 = DAT_0067eca8 + iVar8;
      return;
    }
    DAT_0067eca8 = 0xff;
    DAT_0067eca4 = 2;
    FUN_0043d2a0(DAT_0067ecac,0);
    return;
  }
  if (DAT_0067eca4 == 2) {
    FUN_004325c0();
    iVar8 = FUN_004a2c48();
    DAT_0067eca8 = DAT_0067eca8 + iVar8;
    if (-1 < DAT_0067eca8) {
      return;
    }
    DAT_0067eca8 = 0;
    DAT_0067eca4 = 3;
    return;
  }
  if (DAT_0067eca4 != 3) goto LAB_00440a0c;
  if ((undefined *)(&DAT_0067ed38)[DAT_0067e9f8 * 0x10] == &DAT_005f7000) {
    if (DAT_007f1046 != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + -2;
    }
    if (DAT_007f1047 != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + 2;
    }
    if (DAT_007f1044 != '\0') {
      DAT_007f0eec = DAT_007f0eec + -4;
    }
    if (DAT_007f1045 != '\0') {
      DAT_007f0eec = DAT_007f0eec + 4;
    }
    if (DAT_007f1092 != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + -2;
    }
    if (DAT_007f1093 != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + 2;
    }
    if (DAT_007f1090 != '\0') {
      DAT_007f0eec = DAT_007f0eec + -4;
    }
    if (DAT_007f1091 != '\0') {
      DAT_007f0eec = DAT_007f0eec + 4;
    }
    if (DAT_007f10de != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + -2;
    }
    if (DAT_007f10df != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + 2;
    }
    if (DAT_007f10dc != '\0') {
      DAT_007f0eec = DAT_007f0eec + -4;
    }
    if (DAT_007f10dd != '\0') {
      DAT_007f0eec = DAT_007f0eec + 4;
    }
    if (DAT_007f112a != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + -2;
    }
    if (DAT_007f112b != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + 2;
    }
    if (DAT_007f1128 != '\0') {
      DAT_007f0eec = DAT_007f0eec + -4;
    }
    if (DAT_007f1129 != '\0') {
      DAT_007f0eec = DAT_007f0eec + 4;
    }
    if (DAT_007f1176 != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + -2;
    }
    if (DAT_007f1177 != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + 2;
    }
    if (DAT_007f1174 != '\0') {
      DAT_007f0eec = DAT_007f0eec + -4;
    }
    if (DAT_007f1175 != '\0') {
      DAT_007f0eec = DAT_007f0eec + 4;
    }
    if (DAT_007f11c2 != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + -2;
    }
    if (DAT_007f11c3 != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + 2;
    }
    if (DAT_007f11c0 != '\0') {
      DAT_007f0eec = DAT_007f0eec + -4;
    }
    if (DAT_007f11c1 != '\0') {
      DAT_007f0eec = DAT_007f0eec + 4;
    }
    if (DAT_007f120e != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + -2;
    }
    if (DAT_007f120f != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + 2;
    }
    if (DAT_007f120c != '\0') {
      DAT_007f0eec = DAT_007f0eec + -4;
    }
    if (DAT_007f120d != '\0') {
      DAT_007f0eec = DAT_007f0eec + 4;
    }
    if (DAT_007f125a != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + -2;
    }
    if (DAT_007f125b != '\0') {
      DAT_007f0ef0 = DAT_007f0ef0 + 2;
    }
    if (DAT_007f1258 != '\0') {
      DAT_007f0eec = DAT_007f0eec + -4;
    }
    if (DAT_007f1259 != '\0') {
      DAT_007f0eec = DAT_007f0eec + 4;
    }
    if (DAT_007f0ef0 < 0) {
      DAT_007f0ef0 = 0;
    }
    else if (0x76 < DAT_007f0ef0) {
      DAT_007f0ef0 = 0x76;
    }
    if (DAT_007f0eec < 0x15e) {
      DAT_007f0eec = 0x15e;
    }
    else if (0x41a < DAT_007f0eec) {
      DAT_007f0eec = 0x41a;
    }
  }
  iVar8 = FUN_004325c0();
  FUN_004324a0();
  if (DAT_0067e914 != 0) {
    DAT_0067eca4 = 4;
  }
  if ((iVar8 == 0) || (DAT_0067eab0 != 0)) goto LAB_004409f2;
  piVar18 = (int *)(&DAT_0067ed38)[DAT_0067e9f8 * 0x10];
  iVar9 = 0;
  for (iVar8 = *piVar18; (iVar8 != -0xf90000 && (iVar8 != -0xf70000)); iVar8 = piVar18[iVar8]) {
    iVar8 = iVar9 + 1;
    iVar9 = iVar9 + 1;
  }
  if (piVar18[iVar9] != -0xf70000) goto LAB_004409f2;
  piVar18 = piVar18 + iVar9 + 1;
  iVar8 = *piVar18;
  bVar7 = false;
  bVar6 = false;
  bVar5 = false;
  local_20 = 0;
  bVar4 = false;
  bVar3 = false;
  bVar2 = false;
  while (iVar8 != -0xf60000) {
    if ((iVar8 != -0xf00000) && (iVar8 != -0xdd0000)) {
      if (iVar8 == -0xef0000) {
        bVar2 = true;
      }
      else if (iVar8 == -0xf50000) {
        bVar3 = true;
      }
      else if (iVar8 == -0xf40000) {
        bVar4 = true;
      }
      else if (iVar8 == -0xf30000) {
        bVar5 = true;
      }
      else if (iVar8 == -0xf20000) {
        bVar6 = true;
      }
      else if (iVar8 == -0xee0000) {
        local_20 = 1;
      }
      else if (iVar8 == -0xcd0000) {
        bVar7 = true;
      }
    }
    piVar13 = piVar18 + 1;
    piVar18 = piVar18 + 1;
    iVar8 = *piVar13;
  }
  FUN_0045b350();
  if ((extraout_EDX != 0) && (iVar8 = FUN_0042ae10(local_20), iVar8 != 0)) {
    DAT_0067f19c = 1;
    uVar10 = FUN_0042ac90();
    if (((DAT_0067e7f0 != 1) && (DAT_0067e7f0 != 2)) || ((&DAT_0067ed80)[DAT_0067e9f8 * 0x10] == 0))
    {
      _DAT_0067e840 = 1.0;
    }
    if (uVar10 != 0xffffffff) {
      if (uVar10 < 0xff400001) {
        if (uVar10 == 0xff400000) {
LAB_0043f468:
          DAT_007f0fe8 = 0;
          if (uVar10 == 0xff3d0000) {
            DAT_0067f184 = 1;
          }
          else if (uVar10 == 0xff400000) {
            DAT_0067f184 = 0;
          }
          FUN_0042f6b0();
          iVar8 = 0;
          do {
            FUN_0046dc00(iVar8,1);
            iVar8 = iVar8 + 1;
          } while (iVar8 < 4);
          DAT_0067ea64 = 0;
          DAT_0067ecdc = 0;
          DAT_0067f17c = 0;
          DAT_0067f180 = 0;
          FUN_0043d2a0(4,0);
          DAT_0067ea68 = 0;
          DAT_0067f0c0 = 1;
        }
        else if (uVar10 < 0xff2f0001) {
          if (uVar10 == 0xff2f0000) {
LAB_0043f21d:
            DAT_0067e9fc = 0;
            DAT_005f76c4 = 10;
            DAT_005f76c8 = 3;
            FUN_0042f400(0x61,10);
            FUN_0042f400(0x62,DAT_005f76c8);
            DAT_0067ea64 = (uint)(uVar10 == 0xff2f0000);
            DAT_0067ea8c = 0;
            FUN_0043d2a0(10,0);
          }
          else if (uVar10 < 0xff240001) {
            if (uVar10 == 0xff240000) {
              if ((DAT_0067e9fc == 6) || (DAT_0067e9fc == 2)) {
                if (DAT_0067ea6c == 1) {
                  DAT_0067ea6c = 2;
                }
                else if (DAT_0067ea6c == 4) goto LAB_0043e736;
              }
              else {
                DAT_0067ea6c = 4;
LAB_0043e736:
                if (DAT_0067ecdc == 0) {
                  if (DAT_0067ed6c == 0) {
                    DAT_0067f1a8 = 0;
                    FUN_0043d2a0(7,0);
                    if (DAT_0067ed6c == 2) {
                      DAT_0067e9fc = 3;
                    }
                    DAT_0067ea6c = 5;
                  }
                  else {
                    iVar8 = FUN_004307a0();
                    if (iVar8 == 0) {
                      FUN_0042bf30(0x136,0xff270000,2,0x2e,0x2f,0);
                      DAT_0067ed6c = 2;
                      DAT_0067ea6c = 5;
                    }
                    else {
                      FUN_0042bf30(0x135,0xff280000,1,0x2d,0,0);
                      DAT_0067ed6c = 1;
                      DAT_0067ea6c = 5;
                    }
                  }
                }
                else if (DAT_0067ed4c == 0) {
                  FUN_0042bf30(0x38,0xff240000,2,0x2e,0x2f,0);
                  DAT_0067ea6c = 5;
                }
                else {
                  FUN_0042bf30(0xa9,0xff320000,2,0x2e,0x2f,0);
                  DAT_0067ea6c = 5;
                }
              }
            }
            else if (uVar10 < 0xff1e0001) {
              if (uVar10 == 0xff1e0000) {
                FUN_0042bf30(0x32,0xff1e0000,2,0x2e,0x2f,0);
              }
              else if (uVar10 == 0xff150000) {
                FUN_0043d2a0(0,2);
                DAT_0067e914 = 1;
              }
              else {
                if (uVar10 != 0xff1d0000) goto LAB_0043fc37;
                iVar8 = FUN_00430760();
                if (iVar8 == 0) {
                  iVar8 = FUN_0042b9e0();
                  if (iVar8 == 0x1000) {
                    iVar8 = 0;
                    do {
                      *(undefined4 *)((int)&DAT_0067e850 + iVar8) = 1;
                      *(undefined4 *)((int)&DAT_0067e938 + iVar8) = 0;
                      iVar8 = iVar8 + 0xc;
                    } while (iVar8 < 0x90);
                    iVar8 = 0;
                    do {
                      FUN_0046dc00(iVar8,1);
                      iVar8 = iVar8 + 1;
                    } while (iVar8 < 4);
                    if (DAT_0067ea64 == 0) {
                      FUN_0043d2a0(0xf,0);
                    }
                    else {
                      FUN_0043d2a0(0x10,0);
                    }
                  }
                  else if (iVar8 == 0) {
                    FUN_0042bf30(0x24a,0,1,0x2d,0,0);
                  }
                  else if (iVar8 == 1) {
                    FUN_0042bf30(0x31,0,1,0x2d,0,0);
                  }
                  else if (iVar8 == 2) {
                    FUN_0042bf30(0x9d,0,1,0x2d,0,0);
                  }
                  else if (iVar8 == 3) {
                    FUN_0042bf30(0x9e,0,1,0x2d,0,0);
                  }
                }
                else {
                  if (DAT_0067f0c0 == 0) {
                    DAT_007f1a1c = 0;
                  }
                  else {
                    DAT_007f1a1c = DAT_0067f0c0 + -1;
                  }
                  FUN_0043d2a0(6,0);
                  FUN_00431d00();
                  DAT_0067ea78 = 0;
                }
              }
            }
            else if (uVar10 == 0xff1f0000) {
              FUN_0042bf30(0x33,0xff1f0000,2,0x2e,0x2f,0);
            }
            else {
              if (uVar10 != 0xff200000) goto LAB_0043fc37;
              FUN_0042bf30(0x34,0xff200000,2,0x2e,0x2f,0);
            }
          }
          else if (uVar10 < 0xff2d0001) {
            if (uVar10 == 0xff2d0000) goto LAB_0043f21d;
            if (uVar10 != 0xff260000) {
              if (uVar10 == 0xff2c0000) goto LAB_0043f198;
              goto LAB_0043fc37;
            }
            iVar8 = FUN_004309b0();
            if ((&DAT_007f0a40)[iVar8 + DAT_0067f17c * 0xc] == 0) {
              _DAT_0067e840 = 0.0;
              goto LAB_0043f6d4;
            }
            DAT_0067ed6c = 0;
            DAT_007f0fd0 = 0;
            FUN_0042f020();
            iVar8 = FUN_00430760();
            if (iVar8 != 0) {
              FUN_0040e480(0,1);
              FUN_0040e480(1,0);
              FUN_0040e480(2,0);
              FUN_0040e480(3,0);
            }
            switch(DAT_0067e9fc) {
            case 2:
              DAT_0067ea64 = 0;
              DAT_0067ecdc = 0;
              DAT_0067ea74 = 0;
              DAT_0067ea70 = 0;
              DAT_007f1a24 = 0xffffffff;
              DAT_007f1a34 = 0xffffffff;
              DAT_007f1a44 = 0xffffffff;
              DAT_007f1a18 = 0xffffffff;
              DAT_007f1a28 = 0xffffffff;
              DAT_007f1a38 = 0xffffffff;
              DAT_007f1a48 = 0xffffffff;
              FUN_00429aa0();
              FUN_004298c0();
              FUN_0046dc00(0,DAT_0067ea84);
              break;
            case 3:
            case 4:
            case 5:
              iVar8 = 0;
              if (DAT_0067e9fc == 4) {
                iVar8 = 1;
              }
              else if (DAT_0067e9fc == 5) {
                iVar8 = 2;
              }
              iVar8 = (iVar8 + DAT_0067f17c * 3) * 4;
              DAT_0067ea78 = *(uint *)(&DAT_0067f0e0 + iVar8);
              uVar10 = *(uint *)(&DAT_005f65c8 + iVar8) & 0xffff0000;
              if (uVar10 == 0x70000) {
                DAT_007f0fd0 = 5;
                DAT_0067ea74 = 1;
                DAT_0067ea64 = 0;
                DAT_0067ecdc = 0;
                DAT_0067ea70 = 0;
                DAT_007f1a24 = 0xffffffff;
                DAT_007f1a34 = 0xffffffff;
                DAT_007f1a44 = 0xffffffff;
                DAT_007f1a18 = 0xffffffff;
                DAT_007f1a28 = 0xffffffff;
                DAT_007f1a38 = 0xffffffff;
                DAT_007f1a48 = 0xffffffff;
                FUN_0040e480(0,1);
                FUN_0040e480(1,0);
                FUN_0040e480(2,0);
                FUN_0040e480(3,0);
              }
              else if (uVar10 == 0x40000) {
                DAT_007f0fd0 = 10;
                DAT_0067ea64 = 0;
                DAT_0067ecdc = 0;
                DAT_0067ea74 = 0;
                DAT_0067ea70 = 0;
                DAT_007f1a24 = 0xffffffff;
                DAT_007f1a34 = 0xffffffff;
                DAT_007f1a44 = 0xffffffff;
                DAT_007f1a18 = 0;
                DAT_007f1a28 = 0xffffffff;
                DAT_007f1a38 = 0xffffffff;
                DAT_007f1a48 = 0xffffffff;
                FUN_0040e480(0,1);
                FUN_0040e480(1,0);
                FUN_0040e480(2,0);
                FUN_0040e480(3,0);
              }
              else if (uVar10 == 0x80000) {
                DAT_007f0fd0 = 7;
                DAT_0067ea64 = 0;
                DAT_0067ecdc = 0;
                DAT_0067ea74 = 0;
                DAT_0067ea70 = 0;
                DAT_007f1a24 = 0xffffffff;
                DAT_007f1a34 = 0xffffffff;
                DAT_007f1a44 = 0xffffffff;
                DAT_007f1a18 = 0;
                DAT_007f1a28 = 0xffffffff;
                DAT_007f1a38 = 0xffffffff;
                DAT_007f1a48 = 0xffffffff;
                iVar8 = FUN_00413f90();
                piVar18 = (int *)(iVar8 + DAT_0067f17c * 0x30);
                if (DAT_0067e9fc == 4) {
                  piVar18 = piVar18 + 4;
                }
                else if (DAT_0067e9fc == 5) {
                  piVar18 = piVar18 + 8;
                }
                iVar8 = *piVar18;
                local_2c = 0;
                if (0 < iVar8) {
                  piVar17 = &DAT_007f1a2c;
                  iVar9 = 0;
                  piVar13 = piVar18 + 1;
                  do {
                    iVar16 = iVar9 + 1;
                    if (DAT_007f1a14 == iVar16) {
                      local_2c = local_2c + 1;
                    }
                    piVar17[-2] = local_2c + 1 + iVar9;
                    FUN_0040e480(iVar16,2);
                    iVar9 = *piVar13;
                    piVar13 = piVar13 + 1;
                    iVar14 = iVar9;
                    if (iVar9 == DAT_007f1a1c) {
                      iVar11 = 0;
                      do {
                        if (((iVar11 != DAT_007f1a1c) && (iVar11 != piVar18[1])) &&
                           ((iVar11 != piVar18[2] && (iVar14 = iVar11, iVar11 != piVar18[3]))))
                        break;
                        iVar11 = iVar11 + 1;
                        iVar14 = iVar9;
                      } while (iVar11 < 6);
                    }
                    *piVar17 = iVar14;
                    piVar17[-1] = 1;
                    piVar17 = piVar17 + 4;
                    iVar9 = iVar16;
                  } while (iVar16 < iVar8);
                }
                FUN_0046dc00(0,DAT_0067ea84);
                DAT_0067ea78 = 1;
              }
              else if (uVar10 == 0x50000) {
                DAT_007f0fd0 = 9;
                DAT_0067ea74 = 0;
                DAT_0067ea70 = 0;
                DAT_0067ea64 = 0;
                DAT_007f1a24 = 0xffffffff;
                DAT_007f1a34 = 0xffffffff;
                DAT_007f1a44 = 0xffffffff;
                DAT_007f1a18 = 0;
                DAT_007f1a28 = 0xffffffff;
                DAT_007f1a38 = 0xffffffff;
                DAT_007f1a48 = 0xffffffff;
                iVar8 = FUN_00413f90();
                piVar18 = (int *)(iVar8 + DAT_0067f17c * 0x30);
                if (DAT_0067e9fc == 4) {
                  piVar18 = piVar18 + 4;
                }
                else if (DAT_0067e9fc == 5) {
                  piVar18 = piVar18 + 8;
                }
                iVar8 = *piVar18;
                local_2c = 0;
                if (0 < iVar8) {
                  piVar17 = &DAT_007f1a2c;
                  iVar9 = 0;
                  piVar13 = piVar18 + 1;
                  do {
                    iVar16 = iVar9 + 1;
                    if (DAT_007f1a14 == iVar16) {
                      local_2c = local_2c + 1;
                    }
                    piVar17[-2] = local_2c + 1 + iVar9;
                    FUN_0040e480(iVar16,2);
                    iVar9 = *piVar13;
                    piVar13 = piVar13 + 1;
                    iVar14 = iVar9;
                    if (iVar9 == DAT_007f1a1c) {
                      iVar11 = 0;
                      do {
                        if ((((iVar11 != DAT_007f1a1c) && (iVar11 != piVar18[1])) &&
                            (iVar11 != piVar18[2])) && (iVar14 = iVar11, iVar11 != piVar18[3]))
                        break;
                        iVar11 = iVar11 + 1;
                        iVar14 = iVar9;
                      } while (iVar11 < 6);
                    }
                    *piVar17 = iVar14;
                    piVar17[-1] = 1;
                    piVar17 = piVar17 + 4;
                    iVar9 = iVar16;
                  } while (iVar16 < iVar8);
                }
                FUN_0046dc00(0,DAT_0067ea84);
              }
              else {
                if ((((uVar10 == 0x20000) || (uVar10 == 0x30000)) || (uVar10 == 0xb0000)) ||
                   (uVar10 == 0x60000)) {
                  DAT_007f0fd0 = 4;
                  if (uVar10 == 0x60000) {
                    DAT_007f0fd0 = 8;
                  }
                  DAT_0067ea74 = 0;
                  if (uVar10 == 0x30000) {
                    DAT_0067ea74 = 1;
                  }
                  else if (uVar10 == 0x60000) {
                    DAT_0067ea74 = 1;
                  }
                  else if (uVar10 == 0xb0000) {
                    DAT_0067ea74 = 2;
                  }
                  DAT_0067ea70 = 0;
                  DAT_0067ea64 = 0;
                  DAT_007f1a24 = 0xffffffff;
                  DAT_007f1a34 = 0xffffffff;
                  DAT_007f1a44 = 0xffffffff;
                  DAT_007f1a18 = 0;
                  DAT_007f1a28 = 0xffffffff;
                  DAT_007f1a38 = 0xffffffff;
                  DAT_007f1a48 = 0xffffffff;
                  iVar8 = FUN_00413f90();
                  piVar18 = (int *)(iVar8 + DAT_0067f17c * 0x30);
                  if (DAT_0067e9fc == 4) {
                    piVar18 = piVar18 + 4;
                  }
                  else if (DAT_0067e9fc == 5) {
                    piVar18 = piVar18 + 8;
                  }
                  iVar8 = *piVar18;
                  local_2c = 0;
                  if (0 < iVar8) {
                    piVar17 = &DAT_007f1a2c;
                    iVar9 = 0;
                    piVar13 = piVar18 + 1;
                    do {
                      iVar16 = iVar9 + 1;
                      if (DAT_007f1a14 == iVar16) {
                        local_2c = local_2c + 1;
                      }
                      piVar17[-2] = local_2c + 1 + iVar9;
                      FUN_0040e480(iVar16,2);
                      iVar9 = *piVar13;
                      piVar13 = piVar13 + 1;
                      iVar14 = iVar9;
                      if (iVar9 == DAT_007f1a1c) {
                        iVar11 = 0;
                        do {
                          if (((iVar11 != DAT_007f1a1c) && (iVar11 != piVar18[1])) &&
                             ((iVar11 != piVar18[2] && (iVar14 = iVar11, iVar11 != piVar18[3]))))
                          break;
                          iVar11 = iVar11 + 1;
                          iVar14 = iVar9;
                        } while (iVar11 < 6);
                      }
                      *piVar17 = iVar14;
                      piVar17[-1] = 1;
                      piVar17 = piVar17 + 4;
                      iVar9 = iVar16;
                    } while (iVar16 < iVar8);
                  }
                  FUN_0046dc00(0,DAT_0067ea84);
                }
                if (((uVar10 == 0) || (uVar10 == 0x10000)) || (uVar10 == 0xc0000)) {
                  DAT_0067ea64 = 1;
                  DAT_0067ecdc = 0;
                  DAT_0067ea74 = 0;
                  if (uVar10 == 0x10000) {
                    DAT_0067ea74 = 1;
                  }
                  else if (uVar10 == 0xc0000) {
                    DAT_0067ea74 = 2;
                  }
                  DAT_0067ea70 = 0;
                  DAT_007f1a24 = 0xffffffff;
                  DAT_007f1a34 = 0xffffffff;
                  DAT_007f1a44 = 0xffffffff;
                  DAT_007f1a18 = 0;
                  DAT_007f1a28 = 0xffffffff;
                  DAT_007f1a38 = 0xffffffff;
                  DAT_007f1a48 = 0xffffffff;
                  iVar8 = FUN_00413f90();
                  piVar18 = (int *)(iVar8 + DAT_0067f17c * 0x30);
                  if (DAT_0067e9fc == 4) {
                    piVar18 = piVar18 + 4;
                  }
                  else if (DAT_0067e9fc == 5) {
                    piVar18 = piVar18 + 8;
                  }
                  iVar8 = *piVar18;
                  local_2c = 0;
                  if (0 < iVar8) {
                    piVar17 = &DAT_007f1a2c;
                    iVar9 = 0;
                    piVar13 = piVar18 + 1;
                    do {
                      iVar16 = iVar9 + 1;
                      if (DAT_007f1a14 == iVar16) {
                        local_2c = local_2c + 1;
                      }
                      piVar17[-2] = local_2c + 1 + iVar9;
                      FUN_0040e480(iVar16,2);
                      iVar9 = *piVar13;
                      piVar13 = piVar13 + 1;
                      iVar14 = iVar9;
                      if (iVar9 == DAT_007f1a1c) {
                        iVar11 = 0;
                        do {
                          if ((((iVar11 != DAT_007f1a1c) && (iVar11 != piVar18[1])) &&
                              (iVar11 != piVar18[2])) && (iVar14 = iVar11, iVar11 != piVar18[3]))
                          break;
                          iVar11 = iVar11 + 1;
                          iVar14 = iVar9;
                        } while (iVar11 < 6);
                      }
                      *piVar17 = iVar14;
                      piVar17[-1] = 1;
                      piVar17 = piVar17 + 4;
                      iVar9 = iVar16;
                    } while (iVar16 < iVar8);
                  }
                  FUN_0046dc00(0,DAT_0067ea84);
                }
                if ((uVar10 == 0x90000) || (uVar10 == 0xa0000)) {
                  DAT_0067ea74 = (uint)(uVar10 != 0x90000);
                  DAT_0067ea64 = 0;
                  DAT_0067ecdc = 0;
                  DAT_0067ea70 = 0;
                  DAT_007f1a24 = 0xffffffff;
                  DAT_007f1a34 = 0xffffffff;
                  DAT_007f1a44 = 0xffffffff;
                  DAT_007f1a18 = 0;
                  DAT_007f1a28 = 0xffffffff;
                  DAT_007f1a38 = 0xffffffff;
                  DAT_007f1a48 = 0xffffffff;
                  iVar8 = FUN_00413f90();
                  piVar18 = (int *)(iVar8 + DAT_0067f17c * 0x30);
                  if (DAT_0067e9fc == 4) {
                    piVar18 = piVar18 + 4;
                  }
                  else if (DAT_0067e9fc == 5) {
                    piVar18 = piVar18 + 8;
                  }
                  iVar8 = *piVar18;
                  local_2c = 0;
                  if (0 < iVar8) {
                    piVar17 = &DAT_007f1a2c;
                    iVar9 = 0;
                    piVar13 = piVar18 + 1;
                    do {
                      iVar16 = iVar9 + 1;
                      if (DAT_007f1a14 == iVar16) {
                        local_2c = local_2c + 1;
                      }
                      piVar17[-2] = local_2c + 1 + iVar9;
                      FUN_0040e480(iVar16,2);
                      iVar9 = *piVar13;
                      piVar13 = piVar13 + 1;
                      iVar14 = iVar9;
                      if (iVar9 == DAT_007f1a1c) {
                        iVar11 = 0;
                        do {
                          if (((iVar11 != DAT_007f1a1c) && (iVar11 != piVar18[1])) &&
                             ((iVar11 != piVar18[2] && (iVar14 = iVar11, iVar11 != piVar18[3]))))
                          break;
                          iVar11 = iVar11 + 1;
                          iVar14 = iVar9;
                        } while (iVar11 < 6);
                      }
                      *piVar17 = iVar14;
                      piVar17[-1] = 0;
                      piVar17 = piVar17 + 4;
                      iVar9 = iVar16;
                    } while (iVar16 < iVar8);
                  }
                  FUN_0046dc00(0,DAT_0067ea84);
                }
              }
              break;
            case 6:
            case 7:
            case 8:
            case 9:
              iVar8 = FUN_00430b60();
              if (iVar8 == 2) goto LAB_0043f112;
              break;
            case 10:
              FUN_0040e170(DAT_0067f17c);
              FUN_0043d2a0(0x18,0);
              DAT_0067f1a8 = 1;
LAB_0043f112:
              DAT_0067ea78 = 0;
            }
            iVar8 = FUN_00430760();
            if (iVar8 == 0) {
              FUN_0040e170(DAT_0067f17c);
              FUN_0043d2a0(0x12,0);
              iVar8 = 0;
              puVar12 = &DAT_007f1a14;
              do {
                iVar9 = FUN_0040e470(iVar8);
                if (iVar9 == 2) {
                  *puVar12 = 0xffffffff;
                }
                puVar12 = puVar12 + 4;
                iVar8 = iVar8 + 1;
              } while ((int)puVar12 < 0x7f1a54);
              DAT_0067f1a8 = 1;
            }
            else if (DAT_0067e9fc != 10) {
              FUN_0040e170(DAT_0067f17c);
              goto LAB_0043f371;
            }
          }
          else {
            if (uVar10 == 0xff2e0000) {
LAB_0043f198:
              if (DAT_0067ea8c == 2) {
                DAT_0067ecdc = 1;
              }
              else {
                DAT_0067ecdc = (DAT_0067ea8c != 3) - 1 & 2;
              }
              DAT_0067f17c = 0;
              DAT_0067f180 = 0;
              DAT_0067f184 = 3;
              FUN_0042f6b0();
              DAT_0067ea64 = (uint)(uVar10 == 0xff2e0000);
              puVar12 = &DAT_0067eaf0;
              do {
                *puVar12 = 0xffffffff;
                puVar12 = puVar12 + 3;
              } while ((int)puVar12 < 0x67eb80);
              goto LAB_0043f1fd;
            }
LAB_0043fc37:
            if ((*(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40) == 1) &&
               (*(int *)(&DAT_0067ed40 + DAT_0067e9f8 * 0x40) == 0)) {
              FUN_0042b960();
            }
            FUN_0043d2a0(uVar10,0);
          }
        }
        else {
          if (uVar10 < 0xff390001) {
            if (uVar10 != 0xff390000) {
              if (uVar10 < 0xff360001) {
                if (uVar10 != 0xff360000) {
                  if (uVar10 == 0xff300000) {
                    DAT_0067ecdc = 1;
                  }
                  else {
                    if (uVar10 != 0xff310000) goto LAB_0043fc37;
                    DAT_0067ecdc = 2;
                  }
                  puVar12 = &DAT_0067eaf0;
                  do {
                    *puVar12 = 0xffffffff;
                    puVar12 = puVar12 + 3;
                  } while ((int)puVar12 < 0x67eb80);
                  goto LAB_0043f203;
                }
                FUN_0043d2a0(0xb,0);
              }
              else if (uVar10 != 0xff370000) {
                if (uVar10 != 0xff380000) goto LAB_0043fc37;
                FUN_0043d2a0(0xd,0);
              }
            }
            goto LAB_0043f6d4;
          }
          if (0xff3c0000 < uVar10) {
            if (uVar10 != 0xff3d0000) goto LAB_0043fc37;
            goto LAB_0043f468;
          }
          if (uVar10 == 0xff3c0000) {
            iVar8 = FUN_0042bb60();
            if (iVar8 == 0x1000) {
              puVar12 = &DAT_0067e850;
              do {
                *puVar12 = 1;
                puVar12 = puVar12 + 3;
              } while ((int)puVar12 < 0x67e8e0);
              iVar8 = 0;
              do {
                FUN_0046dc00(iVar8,0);
                iVar8 = iVar8 + 1;
              } while (iVar8 < 4);
              FUN_0043d2a0(0xf,0);
            }
            else if (iVar8 == 0) {
              FUN_0042bf30(0xd6,0,1,0x2d,0,0);
            }
            else if (iVar8 == 1) {
              FUN_0042bf30(0xd7,0,1,0x2d,0,0);
            }
            else if (iVar8 == 2) {
              FUN_0042bf30(0xd8,0,1,0x2d,0,0);
            }
            else if (iVar8 == 3) {
              FUN_0042bf30(0xd9,0,1,0x2d,0,0);
            }
          }
          else if (uVar10 == 0xff3a0000) {
            FUN_0040e170(0);
LAB_0043f371:
            FUN_0043d2a0(0,2);
            DAT_0067e914 = 1;
            DAT_0067ecb0 = 1;
            _DAT_0067ed34 = 0;
          }
          else {
            if (uVar10 != 0xff3b0000) goto LAB_0043fc37;
            FUN_0043d2a0(6,0);
            iVar8 = 0;
            piVar18 = &DAT_007f1a14;
            do {
              if (-1 < *piVar18) {
                FUN_0046dc00(iVar8,(&DAT_0067e850)[*piVar18 * 3]);
              }
              piVar18 = piVar18 + 4;
              iVar8 = iVar8 + 1;
            } while ((int)piVar18 < 0x7f1a54);
          }
        }
      }
      else if (uVar10 < 0xff4d0001) {
        if (uVar10 == 0xff4d0000) {
          DAT_007f0fe8 = 0;
          DAT_0067f184 = 0xb;
          FUN_0042f6b0();
          iVar8 = 0;
          do {
            FUN_0046dc00(iVar8,1);
            iVar8 = iVar8 + 1;
          } while (iVar8 < 4);
          DAT_0067ea64 = 1;
          DAT_0067ecdc = 0;
          DAT_0067f17c = 0;
          DAT_0067f180 = 0;
          FUN_0043d2a0(4,0);
          DAT_0067ea68 = 0;
          DAT_0067f0c0 = 1;
        }
        else if (uVar10 < 0xff480001) {
          if (uVar10 == 0xff480000) {
            FUN_0043d2a0(0x14,0);
          }
          else if (uVar10 < 0xff440001) {
            if (uVar10 == 0xff440000) {
              FUN_00409900();
            }
            else if (uVar10 == 0xff420000) {
              if (DAT_0067e9fc == 10) {
                cVar19 = DAT_0067ea98 != 0;
                if (DAT_0067ea9c != 0) {
                  cVar19 = cVar19 + '\x01';
                }
                if (DAT_0067eaa0 != 0) {
                  cVar19 = cVar19 + '\x01';
                }
                if (cVar19 == '\0') {
                  FUN_0042bf30(0x24f,0,1,0x2d,0,0);
                }
                else {
                  FUN_0043d2a0(0,2);
                  DAT_0067ea64 = DAT_0067eaac ^ 1;
                  DAT_0067e914 = 1;
                  DAT_0067ecb0 = 1;
                  _DAT_0067ed34 = 0;
                  DAT_0067ecdc = 0;
                  if (2 < (int)DAT_0067ea74) {
                    DAT_0067ea74 = 2;
                  }
                  DAT_0067ea70 = 0;
                  DAT_007f1a18 = 0;
                  DAT_007f1a28 = 0xffffffff;
                  DAT_007f1a38 = 0xffffffff;
                  DAT_007f1a48 = 0xffffffff;
                  DAT_007f1a24 = 0xffffffff;
                  DAT_007f1a34 = 0xffffffff;
                  DAT_007f1a44 = 0xffffffff;
                  local_28 = (uint)(DAT_0067ea98 != 0);
                  if (DAT_0067ea9c != 0) {
                    local_28 = local_28 + 1;
                  }
                  if (DAT_0067eaa0 != 0) {
                    local_28 = local_28 + 1;
                  }
                  iVar8 = 0;
                  iVar9 = 0;
                  local_24 = 6;
                  if (local_28 != 0) {
                    iVar16 = 1;
                    piVar18 = &DAT_007f1a2c;
                    do {
                      if ((&DAT_0067ea98)[iVar9] == 0) {
                        do {
                          piVar13 = &DAT_0067ea9c + iVar9;
                          iVar9 = iVar9 + 1;
                        } while (*piVar13 == 0);
                        if (*piVar13 != 0) goto LAB_0043f660;
                      }
                      else {
LAB_0043f660:
                        if (DAT_007f1a14 == iVar16) {
                          iVar9 = iVar9 + 1;
                        }
                        piVar18[-2] = local_24;
                        local_24 = local_24 + 1;
                        FUN_0040e480(iVar16,2);
                        iVar14 = (&DAT_0067ea98)[iVar8];
                        while (iVar14 == 0) {
                          piVar13 = &DAT_0067ea9c + iVar8;
                          iVar8 = iVar8 + 1;
                          iVar14 = *piVar13;
                        }
                        *piVar18 = (&DAT_0067ea98)[iVar8] + -1;
                        piVar18[-1] = 1;
                        iVar8 = iVar8 + 1;
                      }
                      iVar16 = iVar16 + 1;
                      piVar18 = piVar18 + 4;
                      local_28 = local_28 - 1;
                    } while (local_28 != 0);
                  }
LAB_0043f6c5:
                  FUN_0046dc00(0,DAT_0067ea84);
                }
              }
              else {
                DAT_0067ea64 = DAT_008990dc;
                if (DAT_0067ea88 == 0) {
                  DAT_007f0fd0 = 0;
                }
                else if (DAT_0067ea88 == 1) {
                  DAT_007f0fd0 = 1;
                }
                else if (DAT_0067ea88 == 2) {
                  DAT_007f0fd0 = DAT_0067ea88;
                  DAT_0067ea64 = 1;
                }
                FUN_0043d2a0(0,2);
                DAT_0067e914 = 1;
                DAT_0067ecb0 = 1;
                _DAT_0067ed34 = 0;
                iVar8 = 0;
                piVar18 = &DAT_007f1a14;
                do {
                  if (*piVar18 != -1) {
                    FUN_0040e480(iVar8,1);
                  }
                  iVar9 = FUN_0040e470(iVar8);
                  if (iVar9 == 2) {
                    *piVar18 = -1;
                    FUN_0040e480(iVar8,0);
                  }
                  piVar18 = piVar18 + 4;
                  iVar8 = iVar8 + 1;
                } while ((int)piVar18 < 0x7f1a54);
                if ((DAT_0067ea7c != 0) && (DAT_0067ea88 != 1)) {
                  iVar8 = 0;
                  piVar18 = &DAT_007f1a14;
                  do {
                    if (*piVar18 == -1) {
                      iVar9 = 0;
                      do {
                        bVar1 = false;
                        piVar13 = &DAT_007f1a14;
                        do {
                          if (0x7f1a53 < (int)piVar13) {
                            if (!bVar1) {
                              *piVar18 = iVar9;
                            }
                            break;
                          }
                          if (*piVar13 == iVar9) {
                            iVar9 = iVar9 + 1;
                            bVar1 = true;
                          }
                          piVar13 = piVar13 + 4;
                        } while (!bVar1);
                        FUN_0040e480(iVar8,2);
                      } while (bVar1);
                      iVar9 = 0;
                      do {
                        bVar1 = false;
                        piVar13 = &DAT_007f1a1c;
                        do {
                          if (0x7f1a5b < (int)piVar13) {
                            if (!bVar1) {
                              piVar18[2] = iVar9;
                              goto LAB_0043f8e5;
                            }
                            break;
                          }
                          if (*piVar13 == iVar9) {
                            iVar9 = iVar9 + 1;
                            bVar1 = true;
                          }
                          piVar13 = piVar13 + 4;
                        } while (!bVar1);
                      } while( true );
                    }
                    FUN_0040e480(iVar8,1);
LAB_0043f8e5:
                    piVar18 = piVar18 + 4;
                    iVar8 = iVar8 + 1;
                  } while ((int)piVar18 < 0x7f1a54);
                  goto LAB_0043f6c5;
                }
              }
            }
            else {
              if (uVar10 != 0xff430000) goto LAB_0043fc37;
              FUN_0043d2a0(0x13,0);
            }
          }
          else if (uVar10 == 0xff450000) {
            FUN_0043d2a0(0,1);
          }
          else {
            if (uVar10 != 0xff470000) goto LAB_0043fc37;
            FUN_00409930();
          }
        }
        else if (uVar10 < 0xff4b0001) {
          if (uVar10 == 0xff4b0000) {
            FUN_00472640(0xff);
            DAT_0067ecb8 = 1;
            DAT_0067ecb4 = 0;
            FUN_004098b0();
            FUN_0043d2a0(1,0);
            DAT_007f0fe8 = 0;
          }
          else if (uVar10 == 0xff490000) {
            DAT_0067f1a8 = 0;
            FUN_0043d2a0(7,0);
            if (DAT_0067ed6c == 2) {
              DAT_0067e9fc = 3;
            }
          }
          else {
            if (uVar10 != 0xff4a0000) goto LAB_0043fc37;
            FUN_0043d2a0(0,2);
            DAT_0067e914 = 1;
            FUN_0040e360(1);
            DAT_0067ecb4 = 4;
            DAT_0067ecb8 = 1;
            FUN_00429aa0();
            FUN_004298c0();
          }
        }
        else {
          if (uVar10 != 0xff4c0000) goto LAB_0043fc37;
          DAT_0067ecb4 = 0;
          if (*(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40) == 0x17) {
            FUN_0043d2a0(1,0);
            DAT_007f0fe8 = 0;
            FUN_004098b0();
            DAT_0067ed64 = 0;
            DAT_0067ed60 = 0;
          }
          else {
            FUN_0043d2a0(0,1);
            if (DAT_0067ed60 == 0) {
              if (DAT_0067ed64 != 0) {
                FUN_00409930();
              }
              DAT_0067ed64 = 0;
              DAT_0067ed60 = 0;
            }
            else {
              FUN_00409900();
              DAT_0067ed64 = 0;
              DAT_0067ed60 = 0;
            }
          }
        }
      }
      else if (uVar10 < 0xff730001) {
        if (uVar10 == 0xff730000) {
          FUN_0043d2a0(0x1e,0);
        }
        else if (uVar10 < 0xff500001) {
          if (uVar10 == 0xff500000) {
            FUN_0043d2a0(8,0);
          }
          else if (uVar10 == 0xff4e0000) {
            FUN_0042bf30(0x34,0xff4e0000,2,0x2e,0x2f,0);
          }
          else if (uVar10 != 0xff4f0000) goto LAB_0043fc37;
        }
        else if (uVar10 == 0xff710000) {
LAB_0043f1fd:
          DAT_007f0fe8 = 0;
LAB_0043f203:
          FUN_0043d2a0(4,0);
          DAT_0067ea68 = 1;
        }
        else {
          if (uVar10 != 0xff720000) goto LAB_0043fc37;
          DAT_007f0fe8 = 1;
          FUN_0043d2a0(4,0);
          DAT_0067ea68 = 1;
        }
      }
      else if (uVar10 < 0xff820001) {
        if (uVar10 == 0xff820000) {
          iVar8 = FUN_00402f40();
          if (iVar8 == 0) {
            FUN_0043d2a0(0x1f,0);
          }
          else {
            FUN_0043d2a0(0x21,0);
          }
        }
        else if (uVar10 == 0xff800000) {
          FUN_0042bf30(0x262,0xff800000,2,0x2e,0x2f,0);
        }
        else {
          if (uVar10 != 0xff810000) goto LAB_0043fc37;
          *(undefined4 *)(&DAT_0067ed40 + DAT_0067e9f8 * 0x40) = 0;
          _DAT_0067e840 = 0.0;
        }
      }
      else {
        if (uVar10 != 0xff830000) goto LAB_0043fc37;
        FUN_0043d2a0(0x20,0);
      }
    }
  }
LAB_0043f6d4:
  if (((!bVar2) || (iVar8 = FUN_0042aeb0(), iVar8 == 0)) || (DAT_0067f19c != 0)) goto LAB_0043fc9a;
  iVar8 = *(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40);
  _DAT_0067e840 = 1.0;
  DAT_0067f1a0 = 1;
  if (iVar8 == 0x1f) {
    DAT_007f0f30 = DAT_007f0f44;
    DAT_007f0f34 = DAT_007f0f48;
    DAT_007f0f38 = DAT_007f0f4c;
    DAT_007f0f3c = DAT_007f0f50;
  }
  if (iVar8 == 0x21) {
    DAT_007f0f30 = DAT_007f0f44;
    DAT_007f0f34 = DAT_007f0f48;
    DAT_007f0f38 = DAT_007f0f4c;
    DAT_007f0f3c = DAT_007f0f50;
  }
  switch((&DAT_0067ed7c)[(DAT_0067e9f8 + -2) * 0x10]) {
  case 0:
  case 5:
    if (iVar8 == 8) goto switchD_0043f78b_caseD_6;
    DAT_0067ed78 = PTR_DAT_005f763c;
    DAT_0067ed7c = 1;
    DAT_0067edb4 = 5;
    DAT_0067ed80 = 0;
LAB_0043fea7:
    DAT_007f0fe8 = 0;
    FUN_00414120();
    FUN_00422b30();
    FUN_0040b810();
    DAT_0067ea70 = 1;
    FUN_0042b950();
    DAT_0067ecb4 = 0;
    break;
  case 1:
    FUN_0042b950();
    DAT_0067ea70 = 1;
  default:
switchD_0043f78b_caseD_6:
    break;
  case 2:
    DAT_0067ea74 = 1;
    if (DAT_0067ecb0 != 2) goto switchD_0043f78b_caseD_6;
    DAT_0067ecb4 = 0;
    break;
  case 3:
  case 9:
    if (DAT_0067ecb0 == 9) {
      DAT_0067ecb4 = 0;
    }
    else if (DAT_0067ecb0 == 2) {
      DAT_0067ecb4 = 0;
    }
    else {
      if (DAT_0067ecb0 != 0x1c) goto switchD_0043f78b_caseD_6;
      DAT_0067ecb4 = 0;
    }
    break;
  case 4:
    iVar8 = FUN_00430760();
    if (iVar8 == 0) {
      FUN_0042b950();
    }
    goto switchD_0043f78b_caseD_6;
  case 8:
    if (iVar8 == 0x1a) {
      DAT_007f0eec = DAT_007f0ef4;
      DAT_007f0ef0 = DAT_007f0ef8;
    }
    if (iVar8 == 0x13) {
      DAT_007f0f00 = DAT_007f0f14;
      DAT_007f0f10 = DAT_007f0f24;
      DAT_007f0f0c = DAT_007f0f20;
      DAT_007f0f08 = DAT_007f0f1c;
      DAT_007f0f04 = DAT_007f0f18;
    }
    if (iVar8 == 0x20) {
      FUN_0040ad30(DAT_008990d8);
    }
    if (*(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40) == 0x1c) {
      puVar15 = &DAT_007f105c;
      puVar12 = &DAT_00899160;
      do {
        *puVar15 = *puVar12;
        puVar12 = puVar12 + 1;
        puVar15 = puVar15 + 0x13;
      } while ((int)puVar12 < 0x899190);
    }
    if (DAT_0067ecb0 == 0x20) {
      DAT_0067ecb4 = 0;
    }
    else {
      if (DAT_0067ecb0 != 0x22) goto switchD_0043f78b_caseD_6;
      DAT_0067ecb4 = 0;
    }
    break;
  case 0xf:
    DAT_0067f1a8 = 0;
    break;
  case 0x15:
    if (iVar8 != 8) {
      DAT_0067edb8 = PTR_DAT_005f763c;
      DAT_0067edbc = 1;
      _DAT_0067edf4 = 5;
      DAT_0067edc0 = 0;
      goto LAB_0043fea7;
    }
    goto switchD_0043f78b_caseD_6;
  }
  FUN_0043d2a0(0,1);
LAB_0043fc9a:
  if ((bVar3) && (iVar8 = FUN_0042aff0(), iVar8 != 0)) {
    DAT_0067f1a4 = 1;
    FUN_0042aa00(0xffffffff);
    if ((5 < *(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40)) &&
       ((*(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40) < 8 && (DAT_00898ab0 == 0)))) {
      FUN_004323c0();
    }
  }
  if (((bVar4) && (iVar8 = FUN_0042b180(), iVar8 != 0)) && (DAT_0067f1a4 = 1, DAT_00898ab0 == 0)) {
    FUN_004322c0();
  }
  if ((local_20 != 0) && (iVar8 = FUN_0042af50(), iVar8 != 0)) {
    DAT_0067f19c = 1;
    FUN_0043d2a0(0,2);
    DAT_0067e914 = 1;
  }
  if ((bVar5) && (iVar8 = FUN_0042b310(), iVar8 != 0)) {
    iVar8 = DAT_0067e9f8 * 0x40;
    DAT_0067f1a4 = 1;
    switch(*(undefined4 *)(&DAT_0067ed3c + iVar8)) {
    case 4:
      if (DAT_0067ed70 != 0) {
        DAT_0067f1a4 = 0;
      }
      break;
    case 6:
    case 7:
      if (DAT_0067e9fc == 2) {
        DAT_0067f1a4 = 0;
      }
      else if (DAT_0067e9fc == 10) {
        DAT_0067f1a4 = 0;
      }
      else if (DAT_0067ea68 == 0) {
        if (DAT_00898ab0 == 0) {
          if (DAT_0067f184 == 1) {
            DAT_0067f1a4 = 0;
          }
          else if (DAT_0067f184 == 2) {
            DAT_0067f184 = 1;
          }
          else {
            iVar8 = DAT_0067f184;
            if (DAT_0067f184 == 5) {
              DAT_0067f184 = 2;
            }
            else {
              do {
                DAT_0067f184 = iVar8 + -1;
                if (DAT_0067f184 < 1) {
                  DAT_0067f184 = 1;
                  DAT_0067f1a4 = 0;
                }
                iVar8 = DAT_0067f184;
                iVar9 = FUN_00430910();
              } while (iVar9 == 0);
            }
          }
          FUN_0042f6b0();
        }
      }
      else {
        DAT_0067f1a4 = 0;
      }
      break;
    case 10:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar8);
      if ((iVar8 == 0) && (DAT_0067ea8c = DAT_0067ea8c + -1, DAT_0067ea8c < 0)) {
        DAT_0067ea8c = 3;
      }
      if ((iVar8 == 1) && (1 < DAT_005f76c4)) {
        DAT_005f76c4 = DAT_005f76c4 + -1;
      }
      if ((iVar8 == 2) && (1 < DAT_005f76c8)) {
        DAT_005f76c8 = DAT_005f76c8 + -1;
      }
      if (DAT_005f76c4 == 1) {
        uVar21 = 0x69;
        iVar8 = 1;
      }
      else {
        uVar21 = 0x61;
        iVar8 = DAT_005f76c4;
      }
      FUN_0042f400(uVar21,iVar8);
      if (DAT_005f76c8 == 1) {
        FUN_0042f400(0x6a,1);
      }
      else {
        FUN_0042f400(0x62,DAT_005f76c8);
      }
      break;
    case 0x11:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar8);
      if ((iVar8 == 0) && (DAT_0067ea84 = DAT_0067ea84 + -1, DAT_0067ea84 < 0)) {
        DAT_0067ea84 = 3;
      }
      if (iVar8 == 1) {
        DAT_0067ea78 = DAT_0067ea78 ^ 1;
      }
      if ((iVar8 == 2) && (DAT_0067ea80 = DAT_0067ea80 + -1, DAT_0067ea80 < 0)) {
        DAT_0067ea80 = 2;
      }
      break;
    case 0x12:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar8);
      if ((iVar8 == 1) && (DAT_0067ea74 = DAT_0067ea74 - 1, (int)DAT_0067ea74 < 0)) {
        DAT_0067ea74 = 2;
      }
      if ((iVar8 == 2) && (DAT_0067ea80 = DAT_0067ea80 + -1, DAT_0067ea80 < 0)) {
        DAT_0067ea80 = 2;
      }
      if ((iVar8 == 3) && (DAT_0067ea7c = DAT_0067ea7c + -1, DAT_0067ea7c < 0)) {
        DAT_0067ea7c = 4;
      }
      iVar9 = DAT_0067ea94;
      if (iVar8 == 4) {
        do {
          iVar9 = iVar9 + -1;
          if (iVar9 < 0) {
            iVar9 = 0xc;
          }
          iVar16 = FUN_00430830(iVar9);
        } while (iVar16 == 0);
      }
      DAT_0067ea94 = iVar9;
      if (iVar8 == 5) {
        DAT_0067ea78 = DAT_0067ea78 ^ 1;
      }
      if (iVar8 == 6) {
        DAT_0067ea88 = DAT_0067ea88 + -1;
        if (DAT_0067ea88 < 0) {
          if (*(int *)(&DAT_007f0a5c + DAT_0067f17c * 0x30) != 0) {
            DAT_0067ea88 = 2;
LAB_004402b7:
            iVar8 = FUN_0042f500();
            if (iVar8 == 0) break;
          }
          DAT_0067ea88 = 1;
        }
        else if (DAT_0067ea88 == 2) goto LAB_004402b7;
      }
      break;
    case 0x13:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar8);
      if ((iVar8 == 0) && (DAT_007f0f04 = DAT_007f0f04 - _DAT_005cc328, DAT_007f0f04 < DAT_005d757c)
         ) {
        DAT_007f0f04 = 0.0;
      }
      if ((iVar8 == 1) && (DAT_007f0f00 = DAT_007f0f00 - _DAT_005cc328, DAT_007f0f00 < DAT_005d757c)
         ) {
        DAT_007f0f00 = 0.0;
      }
      if ((iVar8 == 2) && (DAT_007f0f08 = DAT_007f0f08 - _DAT_005cc328, DAT_007f0f08 < DAT_005d757c)
         ) {
        DAT_007f0f08 = 0.0;
      }
      if ((iVar8 == 3) && (DAT_007f0f10 = DAT_007f0f10 + -1, DAT_007f0f10 < 0)) {
        DAT_007f0f10 = 2;
      }
      break;
    case 0x18:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar8);
      if ((iVar8 == 1) && (DAT_0067ea74 = DAT_0067ea74 - 1, (int)DAT_0067ea74 < 0)) {
        DAT_0067ea74 = 2;
      }
      if ((iVar8 == 2) && (DAT_0067ea90 = DAT_0067ea90 + -1, DAT_0067ea90 < 1)) {
        DAT_0067ea90 = 4;
      }
      iVar9 = DAT_0067ea94;
      if (iVar8 == 3) {
        do {
          iVar9 = iVar9 + -1;
          if (iVar9 < 0) {
            iVar9 = 0xc;
          }
          iVar16 = FUN_00430830(iVar9);
        } while (iVar16 == 0);
      }
      DAT_0067ea94 = iVar9;
      if (iVar8 == 4) {
        DAT_0067ea98 = DAT_0067ea98 + -1;
        if (DAT_0067ea98 < 0) {
          DAT_0067ea98 = 6;
        }
        FUN_00431b80(1);
      }
      if (iVar8 == 5) {
        DAT_0067ea9c = DAT_0067ea9c + -1;
        if (DAT_0067ea9c < 0) {
          DAT_0067ea9c = 6;
        }
        FUN_00431b80(1);
      }
      if (iVar8 == 6) {
        DAT_0067eaa0 = DAT_0067eaa0 + -1;
        if (DAT_0067eaa0 < 0) {
          DAT_0067eaa0 = 6;
        }
        FUN_00431b80(1);
      }
      if (iVar8 == 7) {
        DAT_0067eaac = DAT_0067eaac ^ 1;
      }
      break;
    case 0x1c:
      iVar8 = 0;
      do {
        if (*(int *)(&DAT_0067ed40 + DAT_0067e9f8 * 0x40) == iVar8) {
          iVar9 = (&DAT_00898aa0)[iVar8];
          (&DAT_007f105c)[iVar9 * 0x13] = (&DAT_007f105c)[iVar9 * 0x13] ^ 1;
          FUN_00492340(iVar9);
        }
        iVar8 = iVar8 + 1;
      } while (iVar8 < 4);
      break;
    case 0x1e:
      if ((*(int *)(&DAT_0067ed40 + iVar8) == 0) &&
         (DAT_0067eaa8 = DAT_0067eaa8 - _DAT_005cc328, DAT_0067eaa8 < DAT_005d757c)) {
        DAT_0067eaa8 = 0.0;
      }
      break;
    case 0x1f:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar8);
      (&DAT_007f0f30)[iVar8] = (&DAT_007f0f30)[iVar8] ^ 1;
      iVar9 = 0;
      do {
        if ((iVar9 != iVar8) && ((&DAT_007f0f30)[iVar8] == 1)) {
          (&DAT_007f0f30)[iVar9] = 0;
        }
        iVar9 = iVar9 + 1;
      } while (iVar9 < 4);
      break;
    case 0x20:
      DAT_0067f1a4 = 0;
      uVar10 = FUN_0040ad20();
      FUN_0040ad30(uVar10 ^ 1);
      DAT_0067f1a4 = 1;
      break;
    case 0x21:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar8);
      (&DAT_007f0f34)[iVar8] = (&DAT_007f0f34)[iVar8] ^ 1;
      iVar9 = 0;
      do {
        if ((iVar9 != iVar8) && ((&DAT_007f0f34)[iVar8] == 1)) {
          (&DAT_007f0f34)[iVar9] = 0;
        }
        iVar9 = iVar9 + 1;
      } while (iVar9 < 2);
    }
  }
  if ((bVar6) && (iVar9 = FUN_0042b540(), iVar8 = DAT_0067f184, iVar9 != 0)) {
    iVar9 = DAT_0067e9f8 * 0x40;
    DAT_0067f1a4 = 1;
    switch(*(undefined4 *)(&DAT_0067ed3c + iVar9)) {
    case 4:
      if (DAT_0067ed70 != 0) {
        DAT_0067f1a4 = 0;
      }
      break;
    case 6:
    case 7:
      if (DAT_0067e9fc == 2) {
        DAT_0067f1a4 = 0;
      }
      else if (DAT_0067e9fc == 10) {
        DAT_0067f1a4 = 0;
      }
      else if (DAT_0067ea68 == 0) {
        if (DAT_00898ab0 == 0) {
          if (DAT_0067f184 == 5) {
            DAT_0067f1a4 = 0;
          }
          else if (DAT_0067f184 == 2) {
            DAT_0067f184 = 5;
          }
          else {
            iVar9 = DAT_0067f184;
            if (DAT_0067f184 == 1) {
              DAT_0067f184 = 2;
            }
            else {
              do {
                iVar16 = iVar9 + 1;
                if (iVar9 + 1 == 0xc) {
                  DAT_0067f1a4 = 0;
                  iVar16 = iVar8;
                }
                DAT_0067f184 = iVar16;
                iVar14 = FUN_00430910();
                iVar9 = iVar16;
              } while (iVar14 == 0);
            }
          }
          FUN_0042f6b0();
        }
      }
      else {
        DAT_0067f1a4 = 0;
      }
      break;
    case 10:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar9);
      if ((iVar8 == 0) && (DAT_0067ea8c = DAT_0067ea8c + 1, DAT_0067ea8c == 4)) {
        DAT_0067ea8c = 0;
      }
      if (iVar8 == 1) {
        DAT_005f76c4 = DAT_005f76c4 + 1;
      }
      if (iVar8 == 2) {
        DAT_005f76c8 = DAT_005f76c8 + 1;
      }
      if (DAT_005f76c4 == 1) {
        uVar21 = 0x69;
        iVar8 = 1;
      }
      else {
        uVar21 = 0x61;
        iVar8 = DAT_005f76c4;
      }
      FUN_0042f400(uVar21,iVar8);
      if (DAT_005f76c8 == 1) {
        FUN_0042f400(0x6a,1);
      }
      else {
        FUN_0042f400(0x62,DAT_005f76c8);
      }
      break;
    case 0x11:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar9);
      if ((iVar8 == 0) && (DAT_0067ea84 = DAT_0067ea84 + 1, DAT_0067ea84 == 4)) {
        DAT_0067ea84 = 0;
      }
      if (iVar8 == 1) {
        DAT_0067ea78 = DAT_0067ea78 ^ 1;
      }
      if ((iVar8 == 2) && (DAT_0067ea80 = DAT_0067ea80 + 1, DAT_0067ea80 == 3)) {
        DAT_0067ea80 = 0;
      }
      break;
    case 0x12:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar9);
      if ((iVar8 == 1) && (DAT_0067ea74 = DAT_0067ea74 + 1, 2 < (int)DAT_0067ea74)) {
        DAT_0067ea74 = 0;
      }
      if ((iVar8 == 2) && (DAT_0067ea80 = DAT_0067ea80 + 1, DAT_0067ea80 == 3)) {
        DAT_0067ea80 = 0;
      }
      if ((iVar8 == 3) && (DAT_0067ea7c = DAT_0067ea7c + 1, 4 < DAT_0067ea7c)) {
        DAT_0067ea7c = 0;
      }
      iVar9 = DAT_0067ea94;
      if (iVar8 == 4) {
        do {
          iVar9 = iVar9 + 1;
          if (0xc < iVar9) {
            iVar9 = 0;
          }
          iVar16 = FUN_00430830(iVar9);
        } while (iVar16 == 0);
      }
      DAT_0067ea94 = iVar9;
      if (iVar8 == 5) {
        DAT_0067ea78 = DAT_0067ea78 ^ 1;
      }
      if (iVar8 == 6) {
        DAT_0067ea88 = DAT_0067ea88 + 1;
        if (DAT_0067ea88 == (*(int *)(&DAT_007f0a5c + DAT_0067f17c * 0x30) != 0) + 2) {
          DAT_0067ea88 = 0;
        }
        else if ((DAT_0067ea88 == 2) && (iVar8 = FUN_0042f500(), iVar8 != 0)) {
          DAT_0067ea88 = 0;
        }
      }
      break;
    case 0x13:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar9);
      if ((iVar8 == 0) &&
         (DAT_007f0f04 = DAT_007f0f04 + _DAT_005cc328, _DAT_005cc320 < DAT_007f0f04)) {
        DAT_007f0f04 = 1.0;
      }
      if ((iVar8 == 1) &&
         (DAT_007f0f00 = DAT_007f0f00 + _DAT_005cc328, _DAT_005cc320 < DAT_007f0f00)) {
        DAT_007f0f00 = 1.0;
      }
      if ((iVar8 == 2) &&
         (DAT_007f0f08 = DAT_007f0f08 + _DAT_005cc328, _DAT_005cc320 < DAT_007f0f08)) {
        DAT_007f0f08 = 1.0;
      }
      if ((iVar8 == 3) && (DAT_007f0f10 = DAT_007f0f10 + 1, DAT_007f0f10 == 3)) {
        DAT_007f0f10 = 0;
      }
      break;
    case 0x18:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar9);
      if ((iVar8 == 1) && (DAT_0067ea74 = DAT_0067ea74 + 1, 2 < (int)DAT_0067ea74)) {
        DAT_0067ea74 = 0;
      }
      if ((iVar8 == 2) && (DAT_0067ea90 = DAT_0067ea90 + 1, 4 < DAT_0067ea90)) {
        DAT_0067ea90 = 1;
      }
      iVar9 = DAT_0067ea94;
      if (iVar8 == 3) {
        do {
          iVar9 = iVar9 + 1;
          if (0xc < iVar9) {
            iVar9 = 0;
          }
          iVar16 = FUN_00430830(iVar9);
        } while (iVar16 == 0);
      }
      DAT_0067ea94 = iVar9;
      if (iVar8 == 4) {
        DAT_0067ea98 = DAT_0067ea98 + 1;
        if (6 < DAT_0067ea98) {
          DAT_0067ea98 = 0;
        }
        FUN_00431b80(1);
      }
      if (iVar8 == 5) {
        DAT_0067ea9c = DAT_0067ea9c + 1;
        if (6 < DAT_0067ea9c) {
          DAT_0067ea9c = 0;
        }
        FUN_00431b80(1);
      }
      if (iVar8 == 6) {
        DAT_0067eaa0 = DAT_0067eaa0 + 1;
        if (6 < DAT_0067eaa0) {
          DAT_0067eaa0 = 0;
        }
        FUN_00431b80(1);
      }
      if (iVar8 == 7) {
        DAT_0067eaac = DAT_0067eaac ^ 1;
      }
      break;
    case 0x1c:
      iVar8 = 0;
      do {
        if (*(int *)(&DAT_0067ed40 + DAT_0067e9f8 * 0x40) == iVar8) {
          iVar9 = (&DAT_00898aa0)[iVar8];
          (&DAT_007f105c)[iVar9 * 0x13] = (&DAT_007f105c)[iVar9 * 0x13] ^ 1;
          FUN_00492340(iVar9);
        }
        iVar8 = iVar8 + 1;
      } while (iVar8 < 4);
      break;
    case 0x1e:
      if ((*(int *)(&DAT_0067ed40 + iVar9) == 0) &&
         (DAT_0067eaa8 = DAT_0067eaa8 + _DAT_005cc328, _DAT_005cc320 < DAT_0067eaa8)) {
        DAT_0067eaa8 = 1.0;
      }
      break;
    case 0x1f:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar9);
      (&DAT_007f0f30)[iVar8] = (&DAT_007f0f30)[iVar8] ^ 1;
      iVar9 = 0;
      do {
        if ((iVar9 != iVar8) && ((&DAT_007f0f30)[iVar8] == 1)) {
          (&DAT_007f0f30)[iVar9] = 0;
        }
        iVar9 = iVar9 + 1;
      } while (iVar9 < 4);
      break;
    case 0x20:
      DAT_0067f1a4 = 0;
      uVar10 = FUN_0040ad20();
      FUN_0040ad30(uVar10 ^ 1);
      DAT_0067f1a4 = 1;
      break;
    case 0x21:
      iVar8 = *(int *)(&DAT_0067ed40 + iVar9);
      (&DAT_007f0f34)[iVar8] = (&DAT_007f0f34)[iVar8] ^ 1;
      iVar9 = 0;
      do {
        if ((iVar9 != iVar8) && ((&DAT_007f0f34)[iVar8] == 1)) {
          (&DAT_007f0f34)[iVar9] = 0;
        }
        iVar9 = iVar9 + 1;
      } while (iVar9 < 2);
    }
  }
  if (((bVar7) && (iVar8 = FUN_0042b770(), iVar8 != 0)) &&
     (*(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40) == 5)) {
    DAT_0067ec28 = 1;
  }
LAB_004409f2:
  FUN_00432800(DAT_0067e9f8 + -1);
LAB_00440a0c:
  if (DAT_0067eca4 == 4) {
    iVar8 = FUN_004325c0();
    iVar9 = FUN_004a2c48();
    if (DAT_0067eca8 + iVar9 < 0xff) {
      DAT_0067eca8 = DAT_0067eca8 + iVar9;
      return;
    }
    DAT_0067eca8 = 0xff;
    if (iVar8 == 0) {
      DAT_0067eca8 = 0xff;
      return;
    }
    DAT_0067eca4 = 5;
  }
  else if (DAT_0067eca4 != 5) {
    return;
  }
  iVar8 = FUN_004a2c48();
  DAT_0067eca8 = DAT_0067eca8 + iVar8;
  if (DAT_0067eca8 < 1) {
    iVar8 = thunk_FUN_00493f70(0);
    if (iVar8 != 0) {
      thunk_FUN_00494460(0);
    }
    DAT_0067eca4 = 0;
    DAT_0067eca8 = 0;
  }
  return;
}

