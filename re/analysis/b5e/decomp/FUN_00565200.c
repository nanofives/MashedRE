
/* [C1 2026-05-19] Reads per-primitive tight AABB at `this + (param_2 & 0xffff) * 0x20` (per-prim
   stride 0x20 = 8 floats = AABB, matches… */

void FUN_00565200(int param_1,uint param_2,float *param_3)

{
  float *pfVar1;

  pfVar1 = (float *)((param_2 & 0xffff) * 0x20 + param_1);
  param_3[4] = pfVar1[4] + *(float *)(param_1 + 0xc014);
  param_3[5] = pfVar1[5] + *(float *)(param_1 + 0xc014);
  param_3[6] = pfVar1[6] + *(float *)(param_1 + 0xc014);
  *param_3 = *pfVar1 - *(float *)(param_1 + 0xc014);
  param_3[1] = pfVar1[1] - *(float *)(param_1 + 0xc014);
  param_3[2] = pfVar1[2] - *(float *)(param_1 + 0xc014);
  return;
}

