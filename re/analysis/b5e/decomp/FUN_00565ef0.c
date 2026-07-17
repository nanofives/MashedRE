
/* [C1 2026-05-19] Signature: `void FUN_00565ef0(float *param_1, float *param_2, float *param_3)`.
    */

void FUN_00565ef0(float *param_1,float *param_2,float *param_3)

{
  float fVar1;

  if (param_2[4] < param_3[4] == (param_2[4] == param_3[4])) {
    fVar1 = param_3[4];
  }
  else {
    fVar1 = param_2[4];
  }
  param_1[4] = fVar1;
  if (param_3[5] < param_2[5]) {
    fVar1 = param_3[5];
  }
  else {
    fVar1 = param_2[5];
  }
  param_1[5] = fVar1;
  if (param_3[6] < param_2[6]) {
    fVar1 = param_3[6];
  }
  else {
    fVar1 = param_2[6];
  }
  param_1[6] = fVar1;
  if (*param_2 < *param_3) {
    fVar1 = *param_3;
  }
  else {
    fVar1 = *param_2;
  }
  *param_1 = fVar1;
  if (param_3[1] < param_2[1] == (param_3[1] == param_2[1])) {
    fVar1 = param_3[1];
  }
  else {
    fVar1 = param_2[1];
  }
  param_1[1] = fVar1;
  if (param_3[2] < param_2[2] != (param_3[2] == param_2[2])) {
    param_1[2] = param_2[2];
    return;
  }
  param_1[2] = param_3[2];
  return;
}

