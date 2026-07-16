
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `void FUN_00578d90(float *pos_A, float *pos_B, float
   *fallback_axis, float… */

void FUN_00578d90(float *param_1,float *param_2,float *param_3,float *param_4)

{
  float fVar1;
  float fVar2;
  float10 fVar3;

  *param_4 = *param_1 - *param_2;
  param_4[1] = param_1[1] - param_2[1];
  fVar1 = param_1[2];
  fVar2 = param_2[2];
  param_4[2] = fVar1 - fVar2;
  fVar1 = -(param_3[2] * (fVar1 - fVar2) + *param_3 * *param_4 + param_3[1] * param_4[1]);
  *param_4 = fVar1 * *param_3 + *param_4;
  param_4[1] = param_3[1] * fVar1 + param_4[1];
  param_4[2] = param_3[2] * fVar1 + param_4[2];
  fVar3 = (float10)FUN_005667c0(param_4,param_4);
  if ((ABS((float10)fVar1) <= (float10)(float)PTR_DAT_005ceabc) ||
     (fVar3 / ABS((float10)fVar1) <= (float10)_DAT_005cc558)) {
    FUN_00566830(param_4,param_3);
    FUN_005667c0(param_4,param_4);
  }
  return;
}

