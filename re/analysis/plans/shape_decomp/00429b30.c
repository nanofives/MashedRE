
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Race-overlay setup A. Gated on `GetRaceSubMode != 2 && DAT_008991b0 == 0`. */

void __fastcall FUN_00429b30(undefined4 param_1)

{
  short sVar1;
  int iVar2;
  
  iVar2 = FUN_0042f6a0(param_1);
  if ((iVar2 != 2) && (DAT_008991b0 == 0)) {
    sVar1 = FUN_0042b8c0();
    DAT_008991b0 = 0x3f;
    DAT_008991b4 = -(_DAT_005cd6c8 / (float)(int)sVar1);
  }
  return;
}

