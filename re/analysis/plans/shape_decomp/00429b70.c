
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Race-overlay setup B (sibling of 0x00429b30). Gated on `GetRaceSubMode in {3, 4,
   5}` (calls 3 times, 3… */

void __fastcall FUN_00429b70(undefined4 param_1)

{
  short sVar1;
  int iVar2;
  
  iVar2 = FUN_0042f6a0(param_1);
  if (((iVar2 != 3) && (iVar2 = FUN_0042f6a0(param_1), iVar2 != 4)) &&
     (iVar2 = FUN_0042f6a0(param_1), iVar2 != 5)) {
    return;
  }
  if (DAT_008991b0 == 0) {
    sVar1 = FUN_0042b8c0();
    DAT_008991b0 = 0xeb;
    DAT_008991b8 = 0;
    DAT_008991b4 = -(_DAT_005cd6c8 / (float)(int)sVar1);
  }
  return;
}

