
/* [C1 2026-05-19] Signature: `void FUN_0056ed60(float *param_1, float *param_2, float *param_3)`.
    */

void FUN_0056ed60(float *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;

  fVar1 = param_3[8] * param_2[8] + *param_2 * *param_3 + param_2[4] * param_3[4];
  fVar3 = param_2[1] * *param_3 + param_2[9] * param_3[8] + param_2[5] * param_3[4];
  fVar2 = param_2[6] * param_3[4] + param_2[2] * *param_3 + param_2[10] * param_3[8];
  fVar4 = param_2[8] * param_3[9] + param_3[1] * *param_2 + param_2[4] * param_3[5];
  fVar5 = param_2[1] * param_3[1] + param_2[5] * param_3[5] + param_2[9] * param_3[9];
  fVar6 = param_2[2] * param_3[1] + param_2[6] * param_3[5] + param_2[10] * param_3[9];
  fVar7 = *param_2 * param_3[2] + param_2[4] * param_3[6] + param_3[10] * param_2[8];
  fVar8 = param_2[1] * param_3[2] + param_3[6] * param_2[5] + param_3[10] * param_2[9];
  fVar9 = param_3[6] * param_2[6] + param_3[10] * param_2[10] + param_2[2] * param_3[2];
  *param_1 = fVar3 * param_3[4] + fVar1 * *param_3 + fVar2 * param_3[8];
  param_1[1] = fVar3 * param_3[5] + fVar2 * param_3[9] + fVar1 * param_3[1];
  param_1[2] = fVar1 * param_3[2] + fVar3 * param_3[6] + fVar2 * param_3[10];
  param_1[4] = fVar5 * param_3[4] + fVar4 * *param_3 + fVar6 * param_3[8];
  param_1[5] = fVar5 * param_3[5] + fVar6 * param_3[9] + fVar4 * param_3[1];
  param_1[6] = fVar4 * param_3[2] + fVar5 * param_3[6] + fVar6 * param_3[10];
  param_1[8] = fVar8 * param_3[4] + fVar7 * *param_3 + fVar9 * param_3[8];
  param_1[9] = fVar8 * param_3[5] + fVar9 * param_3[9] + fVar7 * param_3[1];
  param_1[10] = fVar7 * param_3[2] + fVar8 * param_3[6] + fVar9 * param_3[10];
  return;
}

