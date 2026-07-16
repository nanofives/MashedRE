
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `int *FUN_0056fb90(uint *param_1, float *param_2, float *param_3, int
   *param_4, float *param_5)`. */

int * FUN_0056fb90(uint *param_1,float *param_2,float *param_3,int *param_4,float *param_5)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  ushort uVar5;
  float fVar6;
  float fVar7;
  float *pfVar8;
  float *pfVar9;
  int *piVar10;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  float local_8;
  float local_4;

  uVar5 = (ushort)param_1[1];
  if (uVar5 == 0xffff) {
    piVar10 = (int *)0x0;
  }
  else {
    piVar10 = *(int **)(*(int *)(*param_1 + 0x10) + 4 + (uint)uVar5 * 0xc);
    if (piVar10 != (int *)0x0) {
      pfVar9 = (float *)(piVar10[1] * 0x40 + **(int **)(*piVar10 + 0x10));
      local_1c = (float)piVar10[2];
      local_18 = (float)piVar10[3];
      local_14 = (float)piVar10[4];
      pfVar8 = (float *)(piVar10[1] * 0x10 + (*(int **)(*piVar10 + 0x10))[4]);
      local_10 = *pfVar8;
      local_c = pfVar8[1];
      local_8 = pfVar8[2];
      local_4 = pfVar8[3];
      goto LAB_0056fc5f;
    }
  }
  if (uVar5 == 0xffff) {
    pfVar9 = (float *)(~-(uint)((*(byte *)(*(int *)(*param_1 + 0x5c) + 0x40) & 2) != 0) & *param_1);
  }
  else {
    pfVar9 = *(float **)(*(int *)(*param_1 + 0x10) + 8 + (uint)uVar5 * 0xc);
  }
  local_1c = 0.0;
  local_18 = 0.0;
  local_14 = 0.0;
  FUN_00546b10(&local_10,pfVar9);
LAB_0056fc5f:
  *param_4 = (int)(pfVar9 + 0xc);
  param_3[3] = local_4 * param_2[3] -
               (param_2[2] * local_8 + local_10 * *param_2 + param_2[1] * local_c);
  *param_3 = param_2[2] * local_c - param_2[1] * local_8;
  param_3[1] = local_8 * *param_2 - param_2[2] * local_10;
  param_3[2] = param_2[1] * local_10 - local_c * *param_2;
  *param_3 = local_4 * *param_2 + *param_3;
  param_3[1] = param_2[1] * local_4 + param_3[1];
  param_3[2] = param_2[2] * local_4 + param_3[2];
  *param_3 = param_2[3] * local_10 + *param_3;
  param_3[1] = param_2[3] * local_c + param_3[1];
  fVar6 = param_3[1];
  fVar2 = param_2[3] * local_8 + param_3[2];
  param_3[2] = fVar2;
  fVar1 = *param_3;
  fVar7 = param_3[3];
  fVar3 = fVar2 * fVar2 + fVar6 * fVar6;
  *param_5 = _DAT_005cc320 - (fVar3 + fVar3);
  fVar3 = fVar6 * fVar1 + fVar7 * fVar2;
  param_5[1] = fVar3 + fVar3;
  fVar3 = fVar2 * fVar1 - fVar7 * fVar6;
  param_5[2] = fVar3 + fVar3;
  fVar3 = fVar6 * fVar1 - fVar7 * fVar2;
  param_5[4] = fVar3 + fVar3;
  fVar3 = fVar2 * fVar2 + fVar1 * fVar1;
  param_5[5] = _DAT_005cc320 - (fVar3 + fVar3);
  fVar3 = fVar1 * fVar7 + fVar2 * fVar6;
  param_5[0xc] = 0.0;
  param_5[0xd] = 0.0;
  param_5[0xe] = 0.0;
  param_5[3] = 4.2039e-45;
  param_5[6] = fVar3 + fVar3;
  fVar3 = fVar7 * fVar6 + fVar2 * fVar1;
  param_5[8] = fVar3 + fVar3;
  fVar7 = fVar2 * fVar6 - fVar1 * fVar7;
  param_5[9] = fVar7 + fVar7;
  fVar1 = fVar6 * fVar6 + fVar1 * fVar1;
  param_5[10] = _DAT_005cc320 - (fVar1 + fVar1);
  local_1c = param_2[4] - local_1c;
  local_18 = param_2[5] - local_18;
  local_14 = param_2[6] - local_14;
  fVar1 = pfVar9[9];
  fVar6 = pfVar9[5];
  fVar7 = pfVar9[1];
  fVar2 = pfVar9[10];
  fVar3 = pfVar9[6];
  fVar4 = pfVar9[2];
  param_5[0xc] = pfVar9[0xc] + local_1c * *pfVar9 + local_18 * pfVar9[4] + local_14 * pfVar9[8];
  param_5[0xd] = pfVar9[0xd] + local_1c * fVar7 + local_18 * fVar6 + local_14 * fVar1;
  param_5[0xe] = pfVar9[0xe] + local_1c * fVar4 + local_18 * fVar3 + local_14 * fVar2;
  return piVar10;
}

