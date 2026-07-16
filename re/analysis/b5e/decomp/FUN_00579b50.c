
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `int FUN_00579b50(float *m_a, int m_b, float tolerance)` —
   manifold-merge… */

undefined4 FUN_00579b50(float *param_1,int param_2,float param_3)

{
  float fVar1;

  if (*(int *)(param_2 + 0xac) == 0) {
    return 1;
  }
  fVar1 = ABS(*(float *)(param_2 + 0x18));
  if (ABS(*(float *)(param_2 + 0x18)) <= ABS(param_1[6])) {
    fVar1 = ABS(param_1[6]);
  }
  if (ABS(param_1[2] * (param_1[5] - *(float *)(param_2 + 0x14)) +
          (param_1[3] - *(float *)(param_2 + 0xc)) * *param_1 +
          param_1[1] * (param_1[4] - *(float *)(param_2 + 0x10))) <
      param_3 * _DAT_005cc56c + fVar1 * _DAT_005cc32c) {
    return 1;
  }
  return 0;
}

