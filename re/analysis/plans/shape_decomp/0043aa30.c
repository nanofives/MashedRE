
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-07] Team / co-op pairing screen renderer */

void FUN_0043aa30(void)

{
  float fVar1;
  byte bVar2;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  int *piVar6;
  char acStack_84 [4];
  float fStack_80;
  int iStack_7c;
  int local_78;
  undefined4 uStack_74;
  float fStack_70;
  undefined4 uStack_6c;
  int iStack_68;
  float fStack_64;
  float fStack_60;
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
  undefined1 local_4c;
  undefined1 local_4b;
  undefined1 local_4a;
  byte local_49;
  int iStack_48;
  int iStack_44;
  int iStack_40;
  int iStack_3c;
  float fStack_38;
  int *piStack_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  float fStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined1 *local_18 [4];
  undefined1 *local_8;
  int *local_4;
  
  bVar2 = DAT_0067e7e4;
  local_57 = 0xa7;
  local_56 = 0xa7;
  local_18[0] = &local_50;
  local_18[3] = &local_5c;
  local_18[1] = &local_4c;
  local_18[2] = &local_54;
  local_8 = &local_58;
  local_4 = &local_78;
  local_55 = DAT_0067e7e4;
  local_59 = DAT_0067e7e4;
  local_51 = DAT_0067e7e4;
  local_49 = DAT_0067e7e4;
  local_4d = DAT_0067e7e4;
  local_50 = 0x98;
  local_4f = 0x3a;
  local_4e = 0x3d;
  local_4c = 0x4e;
  local_4b = 0x89;
  local_4a = 0xae;
  local_54 = 0x61;
  local_53 = 0x76;
  local_52 = 0x56;
  local_5c = 0xdb;
  local_5b = 0xc3;
  local_5a = 0x62;
  local_58 = 0xeb;
  local_78 = (uint)DAT_0067e7e4 << 0x18;
  DAT_0067ea7c = 0;
  DAT_008990dc = 1;
  (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
  uStack_2c = 0x41e00000;
  uStack_30 = 0x430c0000;
  fStack_70 = 140.0;
  uStack_74 = 0x437a0000;
  FUN_0042f8d0(0x437a0000,0x430c0000,0x430c0000,0x41e00000);
  FUN_00427e00(0x9f,0x43a00000,0x431a0000,local_78,0x3f4ccccd,2);
  uStack_74 = 0x43d70000;
  FUN_0042f8d0(0x43d70000,0x430c0000,0x430c0000,0x41e00000);
  FUN_00427e00(0xa0,0x43fa0000,0x431a0000,local_78,0x3f4ccccd,2);
  uStack_74 = 0x428c0000;
  fStack_70 = 200.0;
  uStack_30 = 0x43ff0000;
  uStack_20 = 0x42400000;
  uStack_1c = 0x42400000;
  uStack_6c = CONCAT13(bVar2,0xffffff);
  iStack_68 = (uint)bVar2 << 0x18;
  (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
  iStack_7c = 0;
  iStack_3c = 0;
  iStack_40 = 0;
  piStack_34 = &DAT_0067e938;
  do {
    iStack_44 = iStack_3c * 0x22 + 0x1ae;
    iStack_48 = iStack_40 * 0x22 + 0xfa;
    piVar5 = &DAT_007f1a1c;
    piVar6 = piStack_34;
    do {
      if (piVar5[-2] == iStack_7c) {
        FUN_0042f8d0(uStack_74,fStack_70,uStack_30,uStack_2c);
        uVar3 = FUN_0042fab0(*piVar5);
        iVar4 = *piVar6;
        fStack_64 = (float)(iVar4 * 0xb4) + _DAT_005cd95c;
        fStack_80 = fStack_70 - _DAT_005cc574;
        fStack_60 = fStack_80 - _DAT_005cc9f4;
        fStack_38 = fStack_64;
        FUN_004739f0(uVar3,fStack_64,fStack_60,uStack_20,uStack_1c,uStack_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        fStack_64 = fStack_38;
        fStack_60 = fStack_80;
        if (iVar4 != 0) {
          if (iVar4 == 1) {
            iStack_40 = iStack_40 + 1;
            iVar4 = iStack_48;
            iStack_48 = iStack_48 + 0x22;
LAB_0043ad6f:
            fStack_28 = (float)iVar4;
            uStack_24 = 0x42d80000;
          }
          else if (iVar4 == 2) {
            iStack_3c = iStack_3c + 1;
            iVar4 = iStack_44;
            iStack_44 = iStack_44 + 0x22;
            goto LAB_0043ad6f;
          }
          FUN_004739f0(uVar3,fStack_28,uStack_24,uStack_20,uStack_1c,uStack_6c,0,0x3f800000,0,
                       0x3f800000,1,1);
        }
        fVar1 = fStack_60 + _DAT_005cc9b0;
        fStack_80 = fVar1;
        FUN_0042bcb0(piVar5[-2],fStack_64 + _DAT_005ccd0c,fVar1,*(undefined4 *)local_18[*piVar5],
                     *piVar5 == 5);
        FUN_004282a0(0x36,0x3f666666);
        iVar4 = FUN_004a2c48();
        fStack_38 = (float)(iVar4 / 2);
        acStack_84[0] = (char)iStack_7c;
        if ((DAT_007f0fe8 != 0) && (0x67e97f < (int)piStack_34)) {
          acStack_84[0] = (char)iStack_7c + -6;
        }
        acStack_84[0] = acStack_84[0] + '1';
        acStack_84[1] = 0;
        FUN_00427f00(acStack_84,(float)(int)fStack_38 + _DAT_005cc35c + fStack_64 + _DAT_005cda6c,
                     fVar1,iStack_68,0x3f19999a,0);
        fStack_70 = fStack_70 + _DAT_005cd274;
        piVar6 = piStack_34;
      }
      piVar5 = piVar5 + 4;
    } while ((int)piVar5 < 0x7f1a5c);
    piStack_34 = piVar6 + 3;
    iStack_7c = iStack_7c + 1;
    if (0x67e9c7 < (int)piStack_34) {
      (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
      (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
      return;
    }
  } while( true );
}

