
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-07] Time-trial mode info panel (tiny) */

void FUN_00430120(void)

{
  int iStack_4;
  
  if ((DAT_0067e818 != 2) && ((DAT_0067e818 != 0 || (0x5f < _DAT_0067e81c)))) {
    (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
    iStack_4 = _DAT_0067e81c << 0x18;
    if (*(int *)(&DAT_0067ed40 + DAT_0067e9f8 * 0x40) == 1) {
      FUN_00427e00(0x245,0x43a00000,0x43a50000,iStack_4,0x3f19999a,2);
      FUN_00427e00(0x244,0x43a00000,0x43af0000,iStack_4,0x3f19999a,2);
      FUN_00427e00(0x277,0x43a00000,0x43b90000,iStack_4,0x3f19999a,2);
      FUN_00427e00(0x278,0x43a00000,0x43c30000,iStack_4,0x3f19999a,2);
    }
    else {
      FUN_00427e00(0x247,0x43a00000,0x43a50000,iStack_4,0x3f19999a,2);
    }
    (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
    DAT_007f0fe8 = 0;
  }
  return;
}

