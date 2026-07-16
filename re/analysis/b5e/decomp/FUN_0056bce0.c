
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_0056bce0(float *param_1, float *param_2, float param_3)`. */

void FUN_0056bce0(float *param_1,float *param_2,float param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;

  fVar2 = ((-(*param_1 * *param_2) - param_2[1] * param_1[1]) - param_2[2] * param_1[2]) *
          _DAT_005cc32c * param_3 + param_1[3];
  fVar3 = ((param_1[3] * *param_2 + param_2[1] * param_1[2]) - param_2[2] * param_1[1]) *
          _DAT_005cc32c * param_3 + *param_1;
  fVar5 = (param_2[2] * *param_1 + (param_2[1] * param_1[3] - param_1[2] * *param_2)) *
          _DAT_005cc32c * param_3 + param_1[1];
  fVar1 = (param_2[2] * param_1[3] + (param_1[1] * *param_2 - param_2[1] * *param_1)) *
          _DAT_005cc32c * param_3 + param_1[2];
  fVar4 = _DAT_005cc320 / SQRT(fVar2 * fVar2 + fVar1 * fVar1 + fVar5 * fVar5 + fVar3 * fVar3);
  param_1[3] = fVar2 * fVar4;
  *param_1 = fVar3 * fVar4;
  param_1[1] = fVar5 * fVar4;
  param_1[2] = fVar1 * fVar4;
  return;
}

