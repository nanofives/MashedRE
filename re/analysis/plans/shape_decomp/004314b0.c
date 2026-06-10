
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-07] Multi-option Y/N selector (2 or 4 items) */

void FUN_004314b0(void)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  short sVar5;
  undefined *puVar6;
  int iVar7;
  undefined4 uVar8;
  char cVar9;
  int *piVar10;
  int iStack_20;
  float fStack_1c;
  int iStack_18;
  
  (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
  iStack_20 = DAT_0067e834;
  if ((DAT_0067e830 == 0) && (DAT_0067e834 < 0x60)) {
    (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
    return;
  }
  iStack_18 = 4;
  if (DAT_0067e830 == 1) {
    puVar6 = (undefined *)(&DAT_0067ed38)[DAT_0067e9f8 * 0x10];
  }
  else {
    puVar6 = (undefined *)(&DAT_0067ed78)[DAT_0067e9f8 * 0x10];
  }
  if (puVar6 == &DAT_005f6f90) {
    iStack_18 = 2;
  }
  iVar7 = FUN_0042ac50();
  fStack_1c = (float)iVar7;
  iStack_20 = iStack_20 << 0x18;
  if (iStack_18 != 0) {
    piVar10 = &DAT_007f0f30 + (puVar6 == &DAT_005f6f90);
    do {
      sVar5 = FUN_0042b8b0();
      fVar1 = (float)(int)sVar5 * _DAT_005cd8e8;
      sVar5 = FUN_0042b8c0();
      fVar2 = (fStack_1c - _DAT_005cc9f4) * (float)(int)sVar5 * _DAT_005cc560;
      sVar5 = FUN_0042b8b0();
      fVar3 = (float)(int)sVar5 * _DAT_005cc9a4;
      sVar5 = FUN_0042b8c0();
      fVar4 = (float)(int)sVar5 * _DAT_005cc324;
      uVar8 = FUN_0040bb50("Arrow",fVar1,fVar2,fVar3,fVar4,iStack_20,0x3f7ff972,0,0,0x3f800000,0,0);
      FUN_004739f0(uVar8);
      cVar9 = (*piVar10 != 0) + 'Y';
      FUN_00427e00(cVar9,0x43bc0000,fStack_1c,iStack_20,0x3f333333,0);
      FUN_004282a0(cVar9,0x3f333333);
      iVar7 = FUN_004a2c48();
      sVar5 = FUN_0042b8b0();
      uVar8 = FUN_0040bb50("Arrow",((float)iVar7 + _DAT_005cd8e4) * (float)(int)sVar5 *
                                   _DAT_005cd5a8,fVar2,fVar3,fVar4,iStack_20,1);
      FUN_00473870(uVar8);
      fStack_1c = fStack_1c + _DAT_005cc72c;
      piVar10 = piVar10 + 1;
      iStack_18 = iStack_18 + -1;
    } while (iStack_18 != 0);
  }
  (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
  return;
}

