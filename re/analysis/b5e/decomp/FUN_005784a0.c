
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `int FUN_005784a0(uint *count, float *contact_array, float
   *new_pt, float… */

undefined4 FUN_005784a0(uint *param_1,float *param_2,float *param_3,float *param_4,float param_5)

{
  uint uVar1;
  float fVar2;
  float fVar3;
  float fVar4;

  uVar1 = *param_1;
  if (uVar1 != 0) {
    fVar3 = -(param_4[2] * (param_3[2] - param_2[uVar1 * 4 + -2]) +
             (*param_3 - param_2[uVar1 * 4 + -4]) * *param_4 +
             param_4[1] * (param_3[1] - param_2[uVar1 * 4 + -3]));
    fVar2 = fVar3 * *param_4 + (*param_3 - param_2[uVar1 * 4 + -4]);
    fVar4 = fVar3 * param_4[1] + (param_3[1] - param_2[uVar1 * 4 + -3]);
    fVar3 = fVar3 * param_4[2] + (param_3[2] - param_2[uVar1 * 4 + -2]);
    if (fVar3 * fVar3 + fVar2 * fVar2 + fVar4 * fVar4 < _DAT_005ce54c) {
      return 0;
    }
  }
  if ((1 < uVar1) &&
     (fVar3 = -(param_4[2] * (param_3[2] - param_2[2]) +
               (*param_3 - *param_2) * *param_4 + param_4[1] * (param_3[1] - param_2[1])),
     fVar2 = fVar3 * *param_4 + (*param_3 - *param_2),
     fVar4 = fVar3 * param_4[1] + (param_3[1] - param_2[1]),
     fVar3 = fVar3 * param_4[2] + (param_3[2] - param_2[2]),
     fVar3 * fVar3 + fVar2 * fVar2 + fVar4 * fVar4 < _DAT_005ce54c)) {
    return 1;
  }
  param_5 = param_5 - (param_3[2] * param_4[2] + *param_3 * *param_4 + param_3[1] * param_4[1]);
  param_2[uVar1 * 4] = param_5 * *param_4 + *param_3;
  param_2[*param_1 * 4 + 1] = param_5 * param_4[1] + param_3[1];
  param_2[*param_1 * 4 + 2] = param_5 * param_4[2] + param_3[2];
  *param_1 = *param_1 + 1;
  return 0;
}

