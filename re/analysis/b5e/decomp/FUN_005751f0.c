
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `float10 FUN_005751f0(float *A, float *B, float *C)` —
   pure leaf returning a… */

float10 FUN_005751f0(float *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float10 fVar6;
  float10 fVar7;

  fVar4 = *param_3 - *param_1;
  fVar5 = param_3[1] - param_1[1];
  fVar1 = (float)((float10)param_3[2] - (float10)param_1[2]);
  fVar2 = (float)(((float10)param_3[2] - (float10)param_1[2]) *
                  ((float10)param_2[1] - (float10)param_1[1]) -
                 (float10)fVar5 * ((float10)param_2[2] - (float10)param_1[2]));
  fVar3 = (float)((float10)fVar4 * ((float10)param_2[2] - (float10)param_1[2]) -
                 (float10)fVar1 * ((float10)*param_2 - (float10)*param_1));
  fVar7 = (float10)fVar5 * ((float10)*param_2 - (float10)*param_1) -
          (float10)fVar4 * ((float10)param_2[1] - (float10)param_1[1]);
  fVar6 = (float10)fVar1 * (float10)fVar1 +
          (float10)fVar4 * (float10)fVar4 + (float10)fVar5 * (float10)fVar5;
  return (fVar7 * fVar7 + (float10)fVar2 * (float10)fVar2 + (float10)fVar3 * (float10)fVar3) /
         (fVar6 * fVar6 + (float10)_DAT_005cc320);
}

