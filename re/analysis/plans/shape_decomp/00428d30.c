
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-15] No declared parameters in decompiler signature. Renders the lobby/multiplayer
   setup screen. */

void FUN_00428d30(void)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  undefined1 local_20;
  undefined1 local_1f;
  undefined1 local_1e;
  undefined1 local_1d;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 local_8;
  undefined4 local_4;
  
  local_20 = 0;
  local_1f = 0;
  local_1e = 0;
  local_1d = 0xff;
  uVar1 = FUN_0042e590(0);
  local_1c = FUN_0042e590(1);
  local_8 = 0x3f4ccccd;
  local_4 = 0x3f4ccccd;
  uVar2 = FUN_004671a0(0,&local_8);
  FUN_004c1c80(uVar2);
  uVar2 = FUN_004671a0(0,&local_20,3);
  FUN_004c1bb0(uVar2);
  uVar2 = FUN_004671a0(0);
  iVar3 = FUN_004c1a00(uVar2);
  if (iVar3 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x20))(0x14,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
    (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
    FUN_00428760(uVar1,0,0x42800000,0x43b00000,0x43b00000,0);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
    (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
    uStack_c = 0x43f00000;
    uStack_10 = 0x43800000;
    uStack_14 = 0;
    uStack_18 = 0;
    FUN_00473220(0,0,0x43800000,0x43f00000,0x80000000,0xc0000000);
    uStack_10 = 0x42c00000;
    uStack_18 = 0x43800000;
    FUN_00473220(0x43800000,0,0x42c00000,0x43f00000,0xc0000000,0xff000000);
    uVar1 = FUN_0040d250();
    FUN_00401ee0(uVar1);
    if (DAT_00771964 != 0) {
      FUN_00428760(DAT_00771964,0x42800000,0x42700000,0x43480000,0x42c80000,0);
    }
    uStack_c = 0x44000000;
    uStack_10 = 0x44000000;
    uStack_14 = 0;
    uStack_18 = 0;
    FUN_004744a0(0,0,0x44000000,0x44000000,0,0,0x40840000,0,0x407e0000,0,0xff);
    uStack_c = 0x42800000;
    uStack_10 = 0x44200000;
    uStack_14 = 0;
    uStack_18 = 0;
    FUN_00472f40(0,0,0x44200000,0x42800000,0xa0000000);
    uStack_c = 0x42800000;
    uStack_14 = 0x43d00000;
    uStack_18 = 0;
    FUN_004730b0(0,0x43d00000,0x44200000,0x42800000,0xa0000000);
    uStack_c = 0x3f800000;
    uStack_10 = 0x44200000;
    uStack_18 = 0;
    FUN_00472c60(0,0x43d00000,0x44200000,0x3f800000,0xffffffff);
    uStack_14 = 0x42800000;
    FUN_00472c60(0,0x42800000,uStack_10,0x3f800000,0xffffffff);
    uStack_18 = 0x44100000;
    uStack_14 = 0;
    uStack_10 = 0x3f800000;
    uStack_c = 0x43f00000;
    FUN_00472c60(0x44100000,0,0x3f800000,0x43f00000,0xffffffff);
    uStack_c = 0x43800000;
    uStack_14 = 0x42800000;
    uStack_18 = 0x43a00000;
    FUN_00472c60(0x43a00000,0x42800000,0x3f800000,0x43800000,0xffffffff);
    uStack_c = 0x3f800000;
    uStack_10 = 0x43800000;
    uStack_14 = 0x43a00000;
    FUN_00472c60(0x43a00000,0x43a00000,0x43800000,0x3f800000,0xffffffff);
    uStack_10 = 0x44200000;
    uStack_14 = 0x43180000;
    uStack_18 = 0;
    FUN_00472c60(0,0x43180000,0x44200000,0x3f800000,0xffffffff);
    uStack_c = 0x43f00000;
    uStack_10 = 0x3f800000;
    uStack_14 = 0;
    uStack_18 = 0x43f00000;
    FUN_00472c60(0x43f00000,0,0x3f800000,0x43f00000,0xffffffff);
    FUN_00428450(0x10,0x100);
    FUN_00428760(local_1c,0x43a00000,0x42820000,0x43800000,0x437f0000,0);
    uStack_c = 0x43a00000;
    uStack_18 = 0x43a00000;
    uStack_14 = 0;
    uStack_10 = 0x3f800000;
    FUN_00472c60(0x43a00000,0,0x3f800000,0x43a00000,0xffffffff);
    uVar1 = FUN_0042f0b0(0x439b0000,0x43280000,0xffffffff,0x3f800000,1);
    FUN_00427e00(uVar1);
    _DAT_007f1010 = _DAT_007f1010 + _DAT_005cd694;
    FUN_00427e00(0x222,0x43e10000,0x43b90000,0xffffffff,0x3f800000,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(0x14,2);
    uVar1 = FUN_004671a0(0);
    FUN_004c19f0(uVar1);
  }
  uVar1 = FUN_004671a0(0,0,1);
  FUN_004c1be0(uVar1);
  return;
}

