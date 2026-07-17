
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_0056bb80(float *param_1, float *param_2, float param_3)`. */

void FUN_0056bb80(float *param_1,float *param_2,float param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float10 fVar9;
  float10 fVar10;

  fVar9 = (float10)FUN_004c3b30(param_2[2] * param_2[2] +
                                param_2[1] * param_2[1] + *param_2 * *param_2);
  if ((float10)_DAT_005cd03c <= ABS(fVar9)) {
    fVar6 = _DAT_005cc320 / (float)fVar9;
    fVar3 = param_1[1];
    fVar4 = *param_1;
    fVar5 = param_1[2];
    fVar7 = fVar6 * *param_2;
    fVar8 = fVar6 * param_2[1];
    fVar6 = fVar6 * param_2[2];
    fVar9 = (float10)(float)fVar9 * (float10)param_3 * (float10)_DAT_005cc32c;
    fVar10 = (float10)fsin(fVar9);
    fVar1 = (float)fVar10;
    fVar9 = (float10)fcos(fVar9);
    fVar2 = (float)fVar9;
    *param_1 = fVar4 * fVar2 + (fVar7 * param_1[3] + (fVar5 * fVar8 - fVar3 * fVar6)) * fVar1;
    param_1[1] = fVar3 * fVar2 + (fVar8 * param_1[3] + (fVar4 * fVar6 - fVar5 * fVar7)) * fVar1;
    param_1[2] = fVar5 * fVar2 + (fVar6 * param_1[3] + (fVar3 * fVar7 - fVar4 * fVar8)) * fVar1;
    param_1[3] = fVar2 * param_1[3] - (fVar5 * fVar6 + fVar4 * fVar7 + fVar3 * fVar8) * fVar1;
  }
  return;
}

