
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_0056fad0(float *param_1, float *param_2, float *param_3,
   float *param_4, int param_5, int… */

void FUN_0056fad0(float *param_1,float *param_2,float *param_3,float *param_4,int param_5,
                 int param_6)

{
  if (param_5 != 0) {
    *param_1 = *param_2;
    param_1[1] = param_2[1];
    param_1[2] = param_2[2];
    param_1[4] = param_2[2] * param_3[1] - param_2[1] * param_3[2];
    param_1[5] = param_3[2] * *param_2 - param_2[2] * *param_3;
    param_1[6] = param_2[1] * *param_3 - param_3[1] * *param_2;
  }
  if (param_6 != 0) {
    param_1[8] = *param_2 * _DAT_005cc33c;
    param_1[9] = param_2[1] * _DAT_005cc33c;
    param_1[10] = param_2[2] * _DAT_005cc33c;
    param_1[0xc] = param_2[1] * param_4[2] - param_4[1] * param_2[2];
    param_1[0xd] = param_2[2] * *param_4 - param_4[2] * *param_2;
    param_1[0xe] = param_4[1] * *param_2 - param_2[1] * *param_4;
  }
  return;
}

