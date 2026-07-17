
/* [C1 2026-05-20] Signature recovered: `int FUN_00577be0(float *line_start, float *line_dir, float
   t_min, float t_max,… */

undefined4
FUN_00577be0(float *param_1,float *param_2,float param_3,float param_4,float *param_5,float *param_6
            )

{
  float fVar1;
  float fVar2;
  undefined4 uVar3;

  fVar1 = param_2[2] * param_2[2] + param_2[1] * param_2[1] + *param_2 * *param_2;
  fVar2 = param_2[2] * (param_5[2] - param_1[2]) +
          (*param_5 - *param_1) * *param_2 + param_2[1] * (param_5[1] - param_1[1]);
  if (fVar1 * param_3 < fVar2) {
    if (fVar1 * param_4 < fVar2 == (fVar1 * param_4 == fVar2)) {
      param_3 = fVar2 / fVar1;
      uVar3 = 1;
    }
    else {
      uVar3 = 0;
      param_3 = param_4;
    }
  }
  else {
    uVar3 = 0;
  }
  *param_6 = param_3 * *param_2 + *param_1;
  param_6[1] = param_3 * param_2[1] + param_1[1];
  param_6[2] = param_3 * param_2[2] + param_1[2];
  return uVar3;
}

