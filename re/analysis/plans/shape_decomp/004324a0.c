
void FUN_004324a0(void)

{
  int iVar1;
  
  if (((DAT_0067eab0 == 0) || (DAT_0067eabc != -0xba0000)) ||
     ((DAT_0067eab4 != 0x233 &&
      (((DAT_0067eab4 != 0x231 && (DAT_0067eab4 != 0x1c1)) && (DAT_0067eab4 != 0x232)))))) {
    if (DAT_007f1a0c == 0x1000) {
      iVar1 = FUN_0042b930();
      if ((iVar1 != 0x21) && (iVar1 != 1)) {
        iVar1 = FUN_0042c1f0();
        if (iVar1 != 0) {
          if (DAT_0067eab0 == 0) {
            FUN_0042bf30(0x213,0xff220000,0,0,0,0);
            return;
          }
          if (DAT_0067eabc != -0xde0000) {
            FUN_00432450(0x213,0xff220000,0,0,0,0);
          }
        }
      }
    }
    else if (DAT_0067eab0 == 0) {
      FUN_0042c1d0();
      iVar1 = FUN_0042c220();
      if (iVar1 != 0) {
        FUN_0042c280();
        return;
      }
    }
    else if ((DAT_0067eabc != -0xdf0000) && (DAT_0067eabc != -0xde0000)) {
      FUN_0042c1d0();
      iVar1 = FUN_0042c220();
      if (iVar1 != 0) {
        FUN_00432450(0x213,0xff210000,0,0,0,0);
        return;
      }
    }
  }
  return;
}

