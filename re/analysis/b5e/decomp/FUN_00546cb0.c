
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Two-arg matrix-to-quaternion — "Z is largest" branch of the Shoemake algorithm.
   Sibling of 0x00546bf0/0x00546c50. */

void FUN_00546cb0(float *param_1,float *param_2)

{
  float10 fVar1;

  fVar1 = (float10)FUN_004c3b30((param_2[10] - (param_2[5] + *param_2)) + _DAT_005cc320);
  param_1[2] = (float)((float10)_DAT_005cc32c * fVar1);
  fVar1 = (float10)_DAT_005cc32c / fVar1;
  param_1[3] = (float)(((float10)param_2[1] - (float10)param_2[4]) * fVar1);
  *param_1 = (float)(((float10)param_2[8] + (float10)param_2[2]) * fVar1);
  param_1[1] = (float)(((float10)param_2[9] + (float10)param_2[6]) * fVar1);
  return;
}

