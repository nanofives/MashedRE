
/* [C1 2026-05-15] Local table `local_40[16]`: repeating pattern {0,1,2,3,4,5} × 3 (indices mod 6).
    */

void FUN_0042e8b0(int param_1)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  undefined4 local_64;
  int local_60;
  undefined4 local_40 [16];
  
  local_40[0] = 0;
  local_40[1] = 1;
  local_40[2] = 2;
  local_40[3] = 3;
  local_40[4] = 4;
  local_40[5] = 5;
  local_40[6] = 0;
  local_40[7] = 1;
  local_40[8] = 2;
  local_40[9] = 3;
  local_40[10] = 4;
  local_40[0xb] = 5;
  local_40[0xc] = 0;
  local_40[0xd] = 1;
  local_40[0xe] = 2;
  local_40[0xf] = 3;
  local_64 = 0xffffff;
  uVar1 = FUN_004a2c48();
  uVar1 = uVar1 & 0x1ff;
  if (uVar1 == 0x1e0) {
    DAT_0067ea58 = DAT_0067ea58 + 2;
  }
  else if (uVar1 == 0xe0) {
    DAT_0067f0bc = DAT_0067f0bc + 2;
  }
  if ((DAT_0067f0bc & 1) == 0) {
    DAT_0067f0bc = DAT_0067f0bc + 1;
  }
  if (uVar1 < 0xe0) {
    iVar6 = 0;
  }
  else if (uVar1 < 0x100) {
    iVar6 = uVar1 - 0xe0;
    if (0xf < iVar6) {
      iVar5 = (0x1f - iVar6) * 0x10;
      iVar6 = 0xff;
      goto LAB_0042e9ff;
    }
    iVar6 = iVar6 * 0x10;
  }
  else {
    if (uVar1 < 0x1e0) {
      iVar5 = 0;
      iVar6 = 0xff;
      goto LAB_0042e9ff;
    }
    iVar5 = uVar1 - 0x1e0;
    if (iVar5 < 0x10) {
      iVar5 = iVar5 * 0x10;
      iVar6 = 0xff;
      goto LAB_0042e9ff;
    }
    iVar6 = (0x1f - iVar5) * 0x10;
  }
  iVar5 = 0xff;
LAB_0042e9ff:
  uVar4 = local_40[DAT_0067f0bc & 0xf];
  if (iVar5 != 0) {
    iVar5 = iVar5 * param_1;
    uVar2 = uVar1 + 0x20;
    local_64 = CONCAT13(((char)(iVar5 / 0xff) + (char)(iVar5 >> 0x1f)) -
                        (char)((longlong)iVar5 * 0x80808081 >> 0x3f),0xffffff);
    if (0x1ff < uVar2) {
      uVar2 = uVar1 - 0x1e0;
    }
    uVar3 = FUN_0042e590(0,0x43a50000,0x42c00000,0x43870000,0x43340000,local_64,
                         (int)(uVar2 << 8) / 0x120,local_40[DAT_0067ea58 & 0xf],0);
    FUN_00474890(uVar3);
  }
  if (iVar6 != 0) {
    iVar6 = iVar6 * param_1;
    local_64 = CONCAT13(((char)(iVar6 / 0xff) + (char)(iVar6 >> 0x1f)) -
                        (char)((longlong)iVar6 * 0x80808081 >> 0x3f),(undefined3)local_64);
    uVar4 = FUN_0042e590(1,0x43a50000,0x42c00000,0x43870000,0x43340000,local_64,
                         (int)((uVar1 - 0xe0) * 0x100) / 0x120,uVar4,0);
    FUN_00474890(uVar4);
  }
  local_60 = param_1 << 0x18;
  FUN_00472c60(0x43a40000,0x42bc0000,0x40000000,0x43380000,local_60);
  FUN_00472c60(0x43a40000,0x42bc0000,0x43890000,0x40000000,local_60);
  FUN_00472c60(0x43a40000,0x438a0000,0x43890000,0x40000000,local_60);
  FUN_00472c60(0x44160000,0x42bc0000,0x40000000,0x43360000,local_60);
  DAT_008990e0 = DAT_008990e0 + 0x20;
  return;
}

