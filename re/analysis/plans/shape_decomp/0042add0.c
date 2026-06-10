
/* [C1 2026-05-20] Variant of 0x0042ad90 — uses preselected table `(&DAT_0067ed38)[DAT_0067e9f8 *
   0x10]` (current-mode… */

int FUN_0042add0(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int unaff_EBX;
  int iVar4;
  int unaff_EDI;
  
  piVar1 = (int *)(&DAT_0067ed38)[DAT_0067e9f8 * 0x10];
  iVar2 = *piVar1;
  iVar4 = 0;
  iVar3 = 0;
  do {
    if (iVar2 == -0xf90000) {
      return -1;
    }
    if (iVar2 == unaff_EBX) {
      if (unaff_EDI == iVar4) {
        return piVar1[iVar3 + 1];
      }
      iVar4 = iVar4 + 1;
    }
    iVar2 = piVar1[iVar3 + 1];
    iVar3 = iVar3 + 1;
  } while( true );
}

