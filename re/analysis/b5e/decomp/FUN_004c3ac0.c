
float10 FUN_004c3ac0(float *param_1)

{
  param_1 = (float *)(param_1[2] * param_1[2] + param_1[1] * param_1[1] + *param_1 * *param_1);
  if (param_1 != (float *)0x0) {
    param_1 = (float *)(*(int *)(*(int *)(DAT_007d3ffc + DAT_007d3ff8) +
                                ((int)param_1 + 0x800U >> 0xc & 0xfff) * 4) +
                       ((int)param_1 + 0x800U >> 1 & 0x3fc00000));
  }
  return (float10)(float)param_1;
}

