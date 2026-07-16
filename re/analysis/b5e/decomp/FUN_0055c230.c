
/* [C1 2026-05-19] If `param_2 != 0`: rotates `param_4` by 3x3 of `param_2` (cited
   0x0055c241..0x0055c270); points `param_4 = &local_c`. */

int FUN_0055c230(int param_1,float *param_2,undefined4 param_3,float *param_4,undefined4 param_5)

{
  int iVar1;
  float local_c;
  float local_8;
  float local_4;

  if (param_2 != (float *)0x0) {
    local_c = param_2[2] * param_4[2] + *param_2 * *param_4 + param_2[1] * param_4[1];
    local_8 = param_2[6] * param_4[2] + param_2[4] * *param_4 + param_2[5] * param_4[1];
    local_4 = param_2[10] * param_4[2] + param_2[8] * *param_4 + param_2[9] * param_4[1];
    param_4 = &local_c;
  }
  iVar1 = (**(code **)(*(int *)(param_1 + 0x5c) + 0x18))(param_1,param_3,param_4,param_5);
  if ((iVar1 != 0) && (param_2 != (float *)0x0)) {
    FUN_0055c0f0(iVar1,param_2);
  }
  return iVar1;
}

