
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-07] Per-player progress bar renderer (set B, speed/boost) */

void FUN_00431710(void)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  short sVar5;
  short sVar6;
  int iVar7;
  int iVar8;
  undefined4 uVar9;
  int extraout_EDX;
  int iVar10;
  undefined4 local_38;
  int iStack_34;
  int iStack_30;
  float fStack_2c;
  
  local_38 = 0xff2080d8;
  (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
  iVar10 = DAT_0067e824;
  if ((DAT_0067e820 == 0) && (DAT_0067e824 < 0x60)) {
    (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
    return;
  }
  iVar7 = FUN_0042ac00();
  iVar8 = FUN_0042ac50();
  fStack_2c = (float)iVar8;
  iVar8 = 0;
  if (0 < iVar7) {
    do {
      local_38 = CONCAT13((byte)iVar10,(undefined3)local_38);
      iStack_30 = iVar10 << 0x18;
      iStack_34 = (uint)(byte)((byte)iVar10 >> 1) << 0x18;
      sVar5 = FUN_0042b8b0();
      sVar6 = FUN_0042b8c0();
      fVar1 = (fStack_2c - _DAT_005cc9f4) * (float)(int)sVar6 * _DAT_005cc560;
      sVar6 = FUN_0042b8b0();
      fVar2 = (float)(int)sVar6 * _DAT_005cc9a4;
      sVar6 = FUN_0042b8c0();
      fVar3 = (float)(int)sVar6 * _DAT_005cc324;
      uVar9 = FUN_0040bb50("Arrow",(float)((sVar5 * 0x164) / 0x280),fVar1,fVar2,fVar3,iStack_30,
                           0x3f7ff972,0,0,0x3f800000,0,0);
      FUN_004739f0(uVar9);
      if (iVar8 == 0) {
        fVar4 = fStack_2c - _DAT_005cc9b8;
        FUN_00472c60(0x43bb0000,fVar4,0x42d40000,0x41800000,iStack_34);
        FUN_00472c60(0x43bb0000,fVar4,0x42d40000,0x40400000,iStack_30);
        FUN_00472c60(0x43bb0000,fVar4 + _DAT_005cd8d8,0x42d40000,0x40400000,iStack_30);
        FUN_00472c60(0x43bb0000,fVar4,0x40400000,0x41800000,iStack_30);
        FUN_00472c60(0x43ee8000,fVar4,0x40400000,0x41800000,iStack_30);
        if (_DAT_005cc320 < DAT_0067eaa8 * _DAT_005cc568) {
          FUN_00472c60(0x43bc8000,fVar4 + _DAT_005cc31c,DAT_0067eaa8 * _DAT_005cc568,0x41200000,
                       local_38);
        }
      }
      else if ((iVar8 == 2) || (iVar8 == 3)) {
        FUN_004282a0(iVar7,0x3f333333);
        FUN_004a2c48();
      }
      sVar5 = FUN_0042b8b0();
      uVar9 = FUN_0040bb50("Arrow",(float)(((int)sVar5 * (extraout_EDX + 0x17e)) / 0x280),fVar1,
                           fVar2,fVar3,iStack_30,1);
      FUN_00473870(uVar9);
      fStack_2c = fStack_2c + _DAT_005cc72c;
      iVar8 = iVar8 + 1;
      iVar10 = DAT_0067e824;
    } while (iVar8 < iVar7);
  }
  (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
  return;
}

