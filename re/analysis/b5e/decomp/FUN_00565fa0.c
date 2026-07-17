
/* [C1 2026-05-19] Signature: `void FUN_00565fa0(float *param_1, float *param_2, float *param_3,
   float param_4)`. */

void FUN_00565fa0(float *param_1,float *param_2,float *param_3,float param_4)

{
  float fVar1;

  if (*param_3 <= *param_2) {
    param_1[4] = *param_3 - param_4;
    fVar1 = *param_2;
  }
  else {
    param_1[4] = *param_2 - param_4;
    fVar1 = *param_3;
  }
  *param_1 = param_4 + fVar1;
  if (param_3[1] <= param_2[1]) {
    param_1[5] = param_3[1] - param_4;
    fVar1 = param_2[1];
  }
  else {
    param_1[5] = param_2[1] - param_4;
    fVar1 = param_3[1];
  }
  param_1[1] = param_4 + fVar1;
  if (param_2[2] < param_3[2]) {
    param_1[6] = param_2[2] - param_4;
    param_1[2] = param_4 + param_3[2];
    return;
  }
  param_1[6] = param_3[2] - param_4;
  param_1[2] = param_4 + param_2[2];
  return;
}

