
/* [C1 2026-05-19] If `param_2 != 0`: */

void FUN_0055c2d0(int param_1,float *param_2,float *param_3,float *param_4,float *param_5)

{
  float local_10;
  float local_c;
  float local_8;
  float local_4;

  local_10 = 0.0;
  if (param_2 != (float *)0x0) {
    local_10 = param_2[0xe] * param_3[2] + param_2[0xc] * *param_3 + param_2[0xd] * param_3[1];
    local_c = param_2[2] * param_3[2] + *param_2 * *param_3 + param_2[1] * param_3[1];
    local_8 = param_2[6] * param_3[2] + param_2[4] * *param_3 + param_2[5] * param_3[1];
    local_4 = param_2[10] * param_3[2] + param_2[8] * *param_3 + param_2[9] * param_3[1];
    param_3 = &local_c;
  }
  (**(code **)(*(int *)(param_1 + 0x5c) + 0x20))(param_1,param_3,param_4,param_5);
  *param_4 = local_10 + *param_4;
  *param_5 = local_10 + *param_5;
  return;
}

