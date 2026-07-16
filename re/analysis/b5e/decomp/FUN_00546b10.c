
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C2 2026-06-02] Null-check both pointers @ 0x00546b1c → return 0 if either null. 2. Trace
   `fVar1 = rot_mat[10] + rot_mat[0] +… */

undefined4 FUN_00546b10(float *param_1,float *param_2)

{
  float fVar1;
  code *pcVar2;
  float10 fVar3;

  if ((param_1 == (float *)0x0) || (param_2 == (float *)0x0)) {
    return 0;
  }
  fVar1 = param_2[10] + *param_2 + param_2[5];
  if (DAT_005d757c < fVar1) {
    fVar3 = (float10)FUN_004c3b30(fVar1 + _DAT_005cc320);
    param_1[3] = (float)((float10)_DAT_005cc32c * fVar3);
    fVar3 = (float10)_DAT_005cc32c / fVar3;
    *param_1 = (float)(((float10)param_2[6] - (float10)param_2[9]) * fVar3);
    param_1[1] = (float)(((float10)param_2[8] - (float10)param_2[2]) * fVar3);
    param_1[2] = (float)(((float10)param_2[1] - (float10)param_2[4]) * fVar3);
    return 1;
  }
  if (*param_2 <= param_2[5]) {
    pcVar2 = FUN_00546c50;
    if (param_2[10] < param_2[5]) goto LAB_00546bd9;
  }
  else if (param_2[10] < *param_2) {
    FUN_00546bf0(param_1,param_2);
    return 1;
  }
  pcVar2 = FUN_00546cb0;
LAB_00546bd9:
  (*pcVar2)(param_1,param_2);
  return 1;
}

