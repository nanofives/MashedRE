
/* [C1 2026-05-19] Reads body matrix at `iVar9 = p1[1]*0x40 + **(int**)(*p1+0x10)` (cited
   0x0055b752). */

void FUN_0055b750(int *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float *pfVar8;
  int iVar9;

  iVar9 = param_1[1] * 0x40 + **(int **)(*param_1 + 0x10);
  fVar5 = *param_2 - *(float *)(iVar9 + 0x30);
  fVar7 = param_2[1] - *(float *)(iVar9 + 0x34);
  fVar6 = param_2[2] - *(float *)(iVar9 + 0x38);
  pfVar8 = (float *)(param_1[1] * 0x20 + (*(int **)(*param_1 + 0x10))[2]);
  fVar1 = pfVar8[6];
  fVar2 = pfVar8[4];
  fVar3 = pfVar8[4];
  fVar4 = pfVar8[5];
  *param_3 = *pfVar8 + (pfVar8[5] * fVar6 - pfVar8[6] * fVar7);
  param_3[1] = *(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 8) + 4 + param_1[1] * 0x20) +
               (fVar1 * fVar5 - fVar2 * fVar6);
  param_3[2] = *(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 8) + 8 + param_1[1] * 0x20) +
               (fVar3 * fVar7 - fVar4 * fVar5);
  return;
}

