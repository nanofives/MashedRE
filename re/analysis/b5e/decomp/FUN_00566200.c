
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_00566200(float *param_1, float *param_2, float *param_3)`.
    */

void FUN_00566200(float *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;

  fVar1 = (*param_2 - param_2[4]) * _DAT_005cc32c;
  fVar2 = (param_2[1] - param_2[5]) * _DAT_005cc32c;
  fVar3 = (param_2[2] - param_2[6]) * _DAT_005cc32c;
  fVar8 = ABS(*param_3) * fVar1 + ABS(param_3[2]) * fVar3 + ABS(param_3[1]) * fVar2;
  fVar7 = ABS(param_3[4]) * fVar1 + ABS(param_3[6]) * fVar3 + ABS(param_3[5]) * fVar2;
  fVar1 = ABS(param_3[8]) * fVar1 + ABS(param_3[10]) * fVar3 + ABS(param_3[9]) * fVar2;
  fVar6 = (param_2[4] + *param_2) * _DAT_005cc32c - param_3[0xc];
  fVar2 = (param_2[5] + param_2[1]) * _DAT_005cc32c - param_3[0xd];
  fVar4 = (param_2[6] + param_2[2]) * _DAT_005cc32c - param_3[0xe];
  fVar5 = param_3[2] * fVar4 + fVar6 * *param_3 + param_3[1] * fVar2;
  fVar3 = param_3[6] * fVar4 + param_3[4] * fVar6 + param_3[5] * fVar2;
  fVar2 = param_3[10] * fVar4 + param_3[8] * fVar6 + param_3[9] * fVar2;
  param_1[4] = fVar5 - fVar8;
  param_1[5] = fVar3 - fVar7;
  param_1[6] = fVar2 - fVar1;
  *param_1 = fVar8 + fVar5;
  param_1[1] = fVar7 + fVar3;
  param_1[2] = fVar1 + fVar2;
  return;
}

