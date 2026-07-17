
/* [C1 2026-05-19] Projects a contact-point into world space and updates a path-length accumulator.
    */

float * FUN_00561280(int param_1,int param_2,float *param_3)

{
  float *pfVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  int *piVar11;
  float *pfVar12;
  undefined4 *puVar13;
  int iVar14;

  param_2 = param_2 * 0xc;
  puVar13 = (undefined4 *)(*(int *)(param_1 + 0x10) + param_2);
  piVar11 = (int *)puVar13[1];
  pfVar1 = (float *)(**(int **)(*piVar11 + 0x10) + 0x30 + piVar11[1] * 0x40);
  iVar14 = FUN_0055bb70(*puVar13,0,param_3);
  if (iVar14 == 0) {
    return (float *)0x0;
  }
  pfVar12 = *(float **)(param_2 + 8 + *(int *)(param_1 + 0x10));
  fVar2 = pfVar12[5];
  fVar3 = pfVar12[1];
  fVar4 = *param_3;
  fVar5 = pfVar12[9];
  fVar6 = pfVar12[6];
  fVar7 = param_3[1];
  fVar8 = pfVar12[2];
  fVar9 = *param_3;
  fVar10 = pfVar12[10];
  *param_3 = pfVar12[0xc] + *pfVar12 * *param_3 + pfVar12[8] * param_3[2] + pfVar12[4] * param_3[1];
  param_3[1] = *(float *)(*(int *)(param_2 + 8 + *(int *)(param_1 + 0x10)) + 0x34) +
               fVar5 * param_3[2] + fVar3 * fVar4 + fVar2 * param_3[1];
  fVar2 = *(float *)(*(int *)(param_2 + 8 + *(int *)(param_1 + 0x10)) + 0x38) +
          fVar10 * param_3[2] + fVar8 * fVar9 + fVar6 * fVar7;
  param_3[2] = fVar2;
  fVar2 = pfVar1[2] - fVar2;
  param_3[3] = SQRT((pfVar1[1] - param_3[1]) * (pfVar1[1] - param_3[1]) +
                    (*pfVar1 - *param_3) * (*pfVar1 - *param_3) + fVar2 * fVar2) + param_3[3];
  *param_3 = *pfVar1;
  param_3[1] = pfVar1[1];
  param_3[2] = pfVar1[2];
  return param_3;
}

