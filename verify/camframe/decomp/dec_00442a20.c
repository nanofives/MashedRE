
/* [C1 2026-06-01] `void FUN_00442a20(int param_1)` â€” sets a camera to a default entry +
   identity-like */

void FUN_00442a20(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  *(undefined4 *)(param_1 + 0x58) = DAT_00896580;
  FUN_00441700(param_1);
  puVar2 = &DAT_0089650c;
  puVar3 = (undefined4 *)(*(int *)(*(int *)(param_1 + 0x84) + 4) + 0x10);
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  return;
}

