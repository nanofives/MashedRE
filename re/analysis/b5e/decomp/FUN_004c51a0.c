
/* [C1 2026-05-07] RwMatrixTranslate */

void FUN_004c51a0(float *param_1,float *param_2,int param_3)

{
  undefined4 local_8;
  undefined4 local_4;

  if (param_3 == 0) {
    param_1[10] = 1.0;
    param_1[5] = 1.0;
    *param_1 = 1.0;
    param_1[4] = 0.0;
    param_1[3] = (float)((uint)param_1[3] | 0x20003);
    param_1[2] = 0.0;
    param_1[1] = 0.0;
    param_1[9] = 0.0;
    param_1[8] = 0.0;
    param_1[6] = 0.0;
    param_1[0xe] = 0.0;
    param_1[0xd] = 0.0;
    param_1[0xc] = 0.0;
    param_1[0xc] = *param_2;
    param_1[0xd] = param_2[1];
    param_1[0xe] = param_2[2];
    param_1[3] = (float)((uint)param_1[3] & 0xfffdffff);
    return;
  }
  if (param_3 != 1) {
    if (param_3 != 2) {
      local_8 = 1;
      local_4 = FUN_004d7ff0(0x80000003,s_Invalid_combination_type_0061811c);
      FUN_004d8480(&local_8);
      uRam0000000c = uRam0000000c & 0xfffdffff;
      return;
    }
    param_1[0xc] = param_1[0xc] + *param_2;
    param_1[0xd] = param_2[1] + param_1[0xd];
    param_1[0xe] = param_2[2] + param_1[0xe];
    param_1[3] = (float)((uint)param_1[3] & 0xfffdffff);
    return;
  }
  param_1[0xc] = *param_1 * *param_2 + param_1[4] * param_2[1] + param_1[8] * param_2[2] +
                 param_1[0xc];
  param_1[0xd] = param_1[1] * *param_2 + param_1[5] * param_2[1] + param_1[9] * param_2[2] +
                 param_1[0xd];
  param_1[0xe] = param_1[2] * *param_2 + param_1[6] * param_2[1] + param_1[10] * param_2[2] +
                 param_1[0xe];
  param_1[3] = (float)((uint)param_1[3] & 0xfffdffff);
  return;
}

