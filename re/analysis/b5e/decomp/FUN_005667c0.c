
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `float10 FUN_005667c0(float *param_1, float *param_2)`. */

float10 FUN_005667c0(float *param_1,float *param_2)

{
  float10 fVar1;
  float10 fVar2;

  fVar2 = (float10)param_2[2] * (float10)param_2[2] +
          (float10)param_2[1] * (float10)param_2[1] + (float10)*param_2 * (float10)*param_2;
  if ((float10)(float)PTR_DAT_005ceabc < fVar2) {
    fVar1 = (float10)_DAT_005cc320 / SQRT(fVar2);
    *param_1 = (float)(fVar1 * (float10)*param_2);
    param_1[1] = (float)((float10)param_2[1] * fVar1);
    param_1[2] = (float)((float10)param_2[2] * fVar1);
    return fVar1 * fVar2;
  }
  *param_1 = 0.0;
  param_1[1] = 0.0;
  param_1[2] = 1.0;
  return fVar2;
}

