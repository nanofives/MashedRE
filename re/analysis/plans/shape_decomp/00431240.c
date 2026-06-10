
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-07] Yes/No option selector (single, text-width-laid-out) */

void FUN_00431240(void)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  short sVar5;
  int iVar6;
  int iVar7;
  undefined4 uVar8;
  int iVar9;
  char cVar10;
  float10 fVar11;
  int iStack_20;
  float fStack_1c;
  
  (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
  iStack_20 = DAT_0067e83c;
  iVar9 = DAT_0067e838;
  if ((DAT_0067e838 == 0) && (DAT_0067e83c < 0x60)) {
    (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
    return;
  }
  iVar6 = FUN_0042ac00();
  iStack_20 = iStack_20 << 0x18;
  iVar7 = FUN_0042ac50();
  if (iVar9 == 1) {
    uVar8 = *(undefined4 *)(&DAT_00898ab4 + iVar6 * 0x34);
  }
  else {
    uVar8 = (&DAT_00898ab8)[iVar6 * 0xd];
  }
  fVar11 = (float10)FUN_004282a0(uVar8,0x3f4ccccd);
  if ((float10)_DAT_005cd8e0 <= fVar11 + (float10)(float)(&DAT_00898aa4)[iVar6 * 0xd]) {
    fStack_1c = (float)(fVar11 + (float10)(float)(&DAT_00898aa4)[iVar6 * 0xd] +
                       (float10)_DAT_005ccd6c);
  }
  else {
    fStack_1c = 356.0;
  }
  sVar5 = FUN_0042b8b0();
  fVar1 = (float)(int)sVar5 * fStack_1c * _DAT_005cd5a8;
  sVar5 = FUN_0042b8c0();
  fVar2 = ((float)iVar7 - _DAT_005cc9f4) * (float)(int)sVar5 * _DAT_005cc560;
  sVar5 = FUN_0042b8b0();
  fVar3 = (float)(int)sVar5 * _DAT_005cc9a4;
  sVar5 = FUN_0042b8c0();
  fVar4 = (float)(int)sVar5 * _DAT_005cc324;
  uVar8 = FUN_0040bb50("Arrow",fVar1,fVar2,fVar3,fVar4,iStack_20,0x3f7ff972,0,0,0x3f800000,0,0);
  FUN_004739f0(uVar8);
  iVar9 = FUN_0040ad20();
  cVar10 = (iVar9 != 0) + 'Y';
  FUN_00427e00(cVar10,fStack_1c + _DAT_005ccd6c,(float)iVar7,iStack_20,0x3f333333,0);
  FUN_004282a0(cVar10,0x3f333333);
  iVar9 = FUN_004a2c48();
  sVar5 = FUN_0042b8b0();
  uVar8 = FUN_0040bb50("Arrow",((float)iVar9 + fStack_1c + _DAT_005cd8dc) * (float)(int)sVar5 *
                               _DAT_005cd5a8,fVar2,fVar3,fVar4,iStack_20,1);
  FUN_00473870(uVar8);
  (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
  return;
}

