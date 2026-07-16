
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_0056fea0(float *param_1, float *param_2, float *param_3,
   float *param_4)`. */

void FUN_0056fea0(float *param_1,float *param_2,float *param_3,float *param_4)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;

  fVar9 = param_2[3] * param_1[3];
  fVar11 = *param_2 * param_1[3];
  fVar12 = param_2[1] * param_1[3];
  fVar14 = param_2[2] * param_1[3];
  fVar1 = *param_1 * param_2[3];
  fVar2 = *param_1 * *param_2;
  fVar16 = param_2[1] * *param_1;
  fVar3 = param_2[2] * *param_1;
  fVar13 = param_2[3] * param_1[1];
  fVar15 = *param_2 * param_1[1];
  fVar4 = param_2[1] * param_1[1];
  fVar5 = param_2[2] * param_1[1];
  fVar6 = param_1[2] * param_2[3];
  fVar7 = param_1[2] * *param_2;
  fVar8 = param_1[2] * param_2[1];
  fVar10 = param_1[2] * param_2[2];
  param_3[3] = fVar10 + fVar4 + fVar2 + fVar9;
  *param_3 = ((fVar8 - fVar1) + fVar11) - fVar5;
  param_3[1] = (fVar3 - (fVar7 + fVar13)) + fVar12;
  param_3[2] = ((fVar15 - fVar6) - fVar16) + fVar14;
  *param_4 = (((fVar1 - fVar11) - fVar5) + fVar8) * _DAT_005cc32c;
  param_4[1] = (((fVar13 - fVar12) + fVar3) - fVar7) * _DAT_005cc32c;
  param_4[2] = ((fVar6 - (fVar16 + fVar14)) + fVar15) * _DAT_005cc32c;
  param_4[3] = (((fVar2 + fVar9) - fVar10) - fVar4) * _DAT_005cc32c;
  param_4[4] = (fVar6 + fVar15 + fVar16 + fVar14) * _DAT_005cc32c;
  param_4[5] = (((fVar3 - fVar12) + fVar7) - fVar13) * _DAT_005cc32c;
  param_4[6] = (((fVar15 - fVar6) - fVar14) + fVar16) * _DAT_005cc32c;
  param_4[7] = (((fVar4 - fVar10) + fVar9) - fVar2) * _DAT_005cc32c;
  param_4[8] = (fVar8 + fVar5 + fVar1 + fVar11) * _DAT_005cc32c;
  param_4[9] = (fVar7 + fVar13 + fVar3 + fVar12) * _DAT_005cc32c;
  param_4[10] = (((fVar8 + fVar5) - fVar1) - fVar11) * _DAT_005cc32c;
  param_4[0xb] = (((fVar10 - fVar4) - fVar2) + fVar9) * _DAT_005cc32c;
  return;
}

