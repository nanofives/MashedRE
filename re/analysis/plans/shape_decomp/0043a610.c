
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-07] Race-result scoreboard renderer */

void FUN_0043a610(void)

{
  float fVar1;
  byte bVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  char acStack_74 [4];
  int iStack_70;
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
  undefined4 local_50;
  undefined4 uStack_4c;
  float fStack_48;
  int *piStack_44;
  float fStack_40;
  float fStack_3c;
  undefined4 local_38;
  float local_34;
  float fStack_30;
  float fStack_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined1 *local_18 [4];
  undefined1 *local_8;
  undefined1 *local_4;
  
  bVar2 = DAT_0067e7dc;
  local_63 = 0xa7;
  local_62 = 0xa7;
  local_18[1] = &local_58;
  local_24 = 0x41e00000;
  uVar4 = 0x41e00000;
  local_18[0] = &local_54;
  local_28 = 0x42f00000;
  local_18[3] = &local_60;
  local_8 = &local_64;
  local_18[2] = &local_5c;
  local_34 = 140.0;
  local_38 = 0x42820000;
  local_4 = &local_68;
  local_54 = 0x98;
  local_53 = 0x3a;
  local_52 = 0x3d;
  local_58 = 0x4e;
  local_57 = 0x89;
  local_56 = 0xae;
  local_5c = 0x61;
  local_5b = 0x76;
  local_5a = 0x56;
  local_60 = 0xdb;
  local_5f = 0xc3;
  local_5e = 0x62;
  local_64 = 0xeb;
  local_68 = 0xff;
  local_67 = 0xff;
  local_66 = 0xff;
  local_65 = DAT_0067e7dc;
  local_61 = DAT_0067e7dc;
  local_5d = DAT_0067e7dc;
  local_59 = DAT_0067e7dc;
  local_55 = DAT_0067e7dc;
  local_51 = DAT_0067e7dc;
  uStack_4c = CONCAT13(DAT_0067e7dc,(undefined3)uStack_4c);
  FUN_0042f8d0(0x42820000,0x430c0000,0x42f00000,0x41e00000);
  local_38 = 0x43430000;
  FUN_0042f8d0(0x43430000,0x430c0000,0x42f00000,0x41e00000);
  local_38 = 0x43a28000;
  FUN_0042f8d0(0x43a28000,0x430c0000,0x42f00000,0x41e00000);
  local_38 = 0x43e38000;
  FUN_0042f8d0(0x43e38000,0x430c0000,0x42f00000,0x41e00000);
  local_38 = 0x42700000;
  local_34 = 200.0;
  local_28 = 0x4402c000;
  local_20 = 0x42400000;
  local_1c = 0x42400000;
  local_50 = CONCAT13(bVar2,0xffffff);
  local_6c = (uint)bVar2 << 0x18;
  (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
  iStack_70 = 0;
  piStack_44 = &DAT_0067e850;
  do {
    piVar6 = &DAT_007f1a1c;
    piVar7 = piStack_44;
    do {
      if (piVar6[-2] == iStack_70) {
        FUN_0042f8d0(local_38,local_34,local_28,uVar4);
        uVar4 = FUN_0042fab0(*piVar6);
        fStack_40 = (float)(*piVar7 * 0x82) + _DAT_005cd6d4;
        fStack_3c = local_34 - _DAT_005cc574;
        fVar1 = fStack_3c + _DAT_005cc9b0;
        fStack_48 = fVar1;
        fStack_30 = fStack_40;
        FUN_0042bcb0(piVar6[-2],fStack_40 + _DAT_005cda6c,fVar1,*(undefined4 *)local_18[*piVar6],
                     *piVar6 == 5);
        FUN_004282a0(0x36,0x3f666666);
        iVar5 = FUN_004a2c48();
        fStack_48 = (float)(iVar5 / 2);
        acStack_74[0] = (char)iStack_70;
        if ((DAT_007f0fe8 != 0) && (0x67e897 < (int)piStack_44)) {
          acStack_74[0] = (char)iStack_70 + -6;
        }
        acStack_74[0] = acStack_74[0] + '1';
        acStack_74[1] = 0;
        FUN_00427f00(acStack_74,(float)(int)fStack_48 + fStack_30 + _DAT_005cda6c,fVar1,local_6c,
                     0x3f19999a,0);
        fStack_2c = fStack_3c - _DAT_005cc9f4;
        fStack_30 = fStack_40;
        FUN_004739f0(uVar4,fStack_40,fStack_2c,local_20,local_1c,local_50,0,0x3f800000,0,0x3f800000,
                     1,1);
        local_34 = local_34 + _DAT_005cd274;
        uVar4 = local_24;
        piVar7 = piStack_44;
      }
      piVar6 = piVar6 + 4;
    } while ((int)piVar6 < 0x7f1a5c);
    piStack_44 = piVar7 + 3;
    iStack_70 = iStack_70 + 1;
  } while ((int)piStack_44 < 0x67e8e0);
  uStack_4c = uStack_4c & 0xff000000;
  uVar3 = uStack_4c;
  FUN_00427e00(0xe3,0x42fa0000,0x431c0000,uStack_4c,0x3f400000,2);
  FUN_00427e00(0xd2,0x437f0000,0x431c0000,uVar3,0x3f400000,2);
  FUN_00427e00(0xd3,0x43c08000,0x431c0000,uVar3,0x3f400000,2);
  FUN_00427e00(0xd4,0x4400c000,0x431c0000,uVar3,0x3f400000,2);
  (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
  return;
}

