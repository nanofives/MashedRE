
/* [C1 2026-05-19] Signature: `void FUN_00568560(int *param_1, int param_2, int *param_3, undefined4
   *param_4, undefined4 *param_5)`. */

void FUN_00568560(int *param_1,int param_2,int *param_3,undefined4 *param_4,undefined4 *param_5)

{
  if (param_2 != 0) {
    *param_1 = *param_1 + 1;
    *param_3 = param_2;
    param_3[1] = 0;
    param_3[2] = 0;
    *(int **)(param_1[3] + 8) = param_3;
    param_1[3] = (int)param_3;
    param_1[1] = param_1[1] + 1;
  }
  if (param_4 != (undefined4 *)0x0) {
    *param_4 = 0;
    if (param_1[4] == 0) {
      param_1[6] = (int)param_4;
      param_1[5] = (int)param_4;
      param_1[4] = 1;
    }
    else {
      *(undefined4 **)param_1[6] = param_4;
      param_1[6] = (int)param_4;
      param_1[4] = param_1[4] + 1;
    }
  }
  if (param_5 != (undefined4 *)0x0) {
    *param_5 = 0;
    if (param_1[7] == 0) {
      param_1[9] = (int)param_5;
      param_1[8] = (int)param_5;
      param_1[7] = 1;
      return;
    }
    *(undefined4 **)param_1[9] = param_5;
    param_1[9] = (int)param_5;
    param_1[7] = param_1[7] + 1;
  }
  return;
}

