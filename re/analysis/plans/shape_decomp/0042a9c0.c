
/* [C1 2026-05-20] Mode-based code lookup. Reads DAT_007f0fd0 (mode/state): */

undefined4 FUN_0042a9c0(void)

{
  undefined4 uVar1;
  
  if ((((DAT_007f0fd0 == 10) || (DAT_007f0fd0 == 9)) || (DAT_007f0fd0 == 8)) ||
     ((DAT_007f0fd0 == 7 || (uVar1 = 0x16, DAT_007f0fd0 == 5)))) {
    uVar1 = 0x2d;
  }
  return uVar1;
}

