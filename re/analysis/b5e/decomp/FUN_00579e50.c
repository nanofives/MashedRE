
/* [C1 2026-05-20] Signature recovered: `void FUN_00579e50(int simplex)` — per-vertex incremental
   dot-product update for… */

void FUN_00579e50(int param_1)

{
  float *pfVar1;
  int iVar2;
  uint uVar3;
  float *pfVar4;
  uint uVar5;
  int iVar6;
  int iVar7;

  uVar5 = 1;
  uVar3 = *(uint *)(param_1 + 0x1a4) | *(uint *)(param_1 + 0x1ac);
  iVar6 = 0;
  if (0 < (int)uVar3) {
    iVar7 = 0x18;
    pfVar4 = (float *)(param_1 + 8);
    do {
      if ((uVar5 & uVar3) != 0) {
        iVar2 = *(int *)(param_1 + 0x1a8);
        pfVar1 = (float *)(param_1 + iVar2 * 0x18);
        *(float *)(param_1 + (iVar2 + iVar7) * 4) =
             pfVar1[2] * *pfVar4 +
             pfVar4[-2] * *pfVar1 + pfVar4[-1] * *(float *)(param_1 + 4 + iVar2 * 0x18);
        *(undefined4 *)(param_1 + (iVar6 + 0x18 + *(int *)(param_1 + 0x1a8) * 4) * 4) =
             *(undefined4 *)(param_1 + (iVar7 + *(int *)(param_1 + 0x1a8)) * 4);
      }
      uVar5 = uVar5 * 2;
      uVar3 = *(uint *)(param_1 + 0x1a4) | *(uint *)(param_1 + 0x1ac);
      iVar6 = iVar6 + 1;
      iVar7 = iVar7 + 4;
      pfVar4 = pfVar4 + 6;
    } while ((int)uVar5 <= (int)uVar3);
  }
  return;
}

