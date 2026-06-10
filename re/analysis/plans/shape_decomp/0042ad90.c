
/* [C1 2026-05-20] Sparse key/value table lookup. Takes (param_1 unused, param_2 = int* table). */

int __fastcall FUN_0042ad90(undefined4 param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  int unaff_EBX;
  int iVar3;
  int unaff_EDI;
  
  if (param_2 == (int *)0x0) {
    return -1;
  }
  iVar1 = *param_2;
  iVar3 = 0;
  iVar2 = 0;
  do {
    if (iVar1 == -0xf90000) {
      return -1;
    }
    if (iVar1 == unaff_EBX) {
      if (unaff_EDI == iVar3) {
        return param_2[iVar2 + 1];
      }
      iVar3 = iVar3 + 1;
    }
    iVar1 = param_2[iVar2 + 1];
    iVar2 = iVar2 + 1;
  } while( true );
}

