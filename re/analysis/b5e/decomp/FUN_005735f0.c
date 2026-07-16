
/* [C1 2026-05-19] Signature: `void FUN_005735f0(float *param_1, float *param_2, float *param_3,
   float *param_4)`. */

void FUN_005735f0(float *param_1,float *param_2,float *param_3,float *param_4)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;

  if (param_1 == (float *)0x0) {
    param_4[2] = 0.0;
    param_4[1] = 0.0;
    *param_4 = 0.0;
    return;
  }
  fVar1 = *param_3;
  fVar2 = *param_2;
  fVar3 = param_3[1];
  fVar4 = param_2[1];
  fVar5 = param_3[2];
  fVar6 = param_2[2];
  *param_4 = param_1[5] * (fVar5 - fVar6) - param_1[6] * (fVar3 - fVar4);
  param_4[1] = param_1[6] * (fVar1 - fVar2) - param_1[4] * (fVar5 - fVar6);
  param_4[2] = param_1[4] * (fVar3 - fVar4) - param_1[5] * (fVar1 - fVar2);
  *param_4 = *param_1 + *param_4;
  param_4[1] = param_1[1] + param_4[1];
  param_4[2] = param_1[2] + param_4[2];
  return;
}

