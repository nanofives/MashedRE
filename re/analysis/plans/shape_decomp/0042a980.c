
/* [C1 2026-05-20] Linked-list lookup. Calls FUN_0040ce80(param_1) -> iVar2 (key). Walks a
   sentinel-terminated list at… */

undefined4 FUN_0042a980(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = FUN_0040ce80(param_1);
  iVar3 = 0;
  iVar1 = DAT_005f6748;
  while( true ) {
    if (iVar1 == -1) {
      return 0;
    }
    if (iVar1 == iVar2) break;
    iVar1 = (&DAT_005f6754)[iVar3];
    iVar3 = iVar3 + 3;
  }
  return *(undefined4 *)(iVar3 * 4 + 0x5f674c);
}

