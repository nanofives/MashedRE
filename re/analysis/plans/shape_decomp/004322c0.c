
void FUN_004322c0(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  FUN_0042aa00(1);
  if ((5 < *(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40)) &&
     (iVar4 = DAT_0067f184, iVar6 = DAT_0067e9fc, *(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40) < 8)
     ) {
    while( true ) {
      DAT_0067f17c = DAT_0067f17c + 1;
      if (0xc < DAT_0067f17c) {
        DAT_0067f17c = 0xc;
        DAT_0067f1a4 = 0;
      }
      iVar1 = DAT_0067f17c;
      iVar2 = FUN_00430830(DAT_0067f17c);
      if (iVar2 != 0) {
        return;
      }
      iVar2 = (uint)(DAT_0067ea68 != 0) * 2 + 1;
      if (iVar6 == 10) {
        iVar2 = 0xb;
      }
      else if (iVar6 == 2) {
        iVar2 = 0;
      }
      if (iVar4 == iVar2) break;
      do {
        iVar4 = iVar4 + -1;
        iVar5 = iVar2;
        DAT_0067f184 = iVar2;
        if (iVar4 < iVar2) break;
        DAT_0067f184 = iVar4;
        iVar3 = FUN_00430910();
        iVar5 = iVar4;
      } while (iVar3 == 0);
      if (((iVar6 == 3) || (iVar6 == 4)) || (iVar6 == 5)) {
        FUN_0042f6b0();
        iVar6 = DAT_0067e9fc;
      }
      DAT_0067f17c = iVar1 + -1;
      iVar4 = iVar5;
    }
    DAT_0067f17c = iVar1 + -1;
    DAT_0067f1a4 = 0;
  }
  return;
}

