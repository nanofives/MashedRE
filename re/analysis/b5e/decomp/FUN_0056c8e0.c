
/* [C1 2026-05-19] Signature: `void FUN_0056c8e0(int param_1, int param_2, float *param_3, int
   param_4)`. */

void FUN_0056c8e0(int param_1,int param_2,float *param_3,int param_4)

{
  float *pfVar1;
  float *pfVar2;
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
  int iVar15;

  iVar15 = param_4 * 0x20;
  fVar3 = *(float *)(iVar15 + 4 + param_2);
  fVar4 = *param_3;
  fVar5 = *(float *)(iVar15 + 8 + param_2);
  fVar6 = *param_3;
  fVar7 = *(float *)(iVar15 + 0x44 + param_2);
  fVar8 = param_3[1];
  fVar9 = *(float *)(iVar15 + 0x48 + param_2);
  fVar10 = param_3[1];
  pfVar1 = (float *)(iVar15 + 0x14 + param_1);
  fVar11 = *(float *)(iVar15 + 0x84 + param_2);
  fVar12 = param_3[2];
  fVar13 = *(float *)(iVar15 + 0x88 + param_2);
  fVar14 = param_3[2];
  pfVar2 = (float *)(iVar15 + 0x10 + param_1);
  *(float *)(iVar15 + param_1) =
       *(float *)((param_4 + 6) * 0x20 + param_2) * param_3[3] +
       *(float *)((param_4 + 4) * 0x20 + param_2) * param_3[2] +
       *(float *)((param_4 + 2) * 0x20 + param_2) * param_3[1] +
       *(float *)(iVar15 + param_2) * *param_3 + *(float *)(iVar15 + param_1);
  *(float *)(iVar15 + 4 + param_1) =
       *(float *)(iVar15 + 0xc4 + param_2) * param_3[3] +
       fVar11 * fVar12 + fVar7 * fVar8 + fVar3 * fVar4 + *(float *)(iVar15 + 4 + param_1);
  *(float *)(iVar15 + 8 + param_1) =
       *(float *)(iVar15 + 200 + param_2) * param_3[3] +
       fVar13 * fVar14 + fVar9 * fVar10 + fVar5 * fVar6 + *(float *)(iVar15 + 8 + param_1);
  fVar3 = *(float *)(iVar15 + 0x14 + param_2);
  fVar4 = *param_3;
  fVar5 = *(float *)(iVar15 + 0x18 + param_2);
  fVar6 = *param_3;
  fVar7 = *(float *)(iVar15 + 0x54 + param_2);
  fVar8 = param_3[1];
  fVar9 = *(float *)(iVar15 + 0x58 + param_2);
  fVar10 = param_3[1];
  fVar11 = *(float *)(iVar15 + 0x94 + param_2);
  fVar12 = param_3[2];
  fVar13 = *(float *)(iVar15 + 0x98 + param_2);
  fVar14 = param_3[2];
  *pfVar2 = *(float *)(iVar15 + 0xd0 + param_2) * param_3[3] +
            *(float *)(iVar15 + 0x90 + param_2) * param_3[2] +
            *(float *)(iVar15 + 0x50 + param_2) * param_3[1] +
            *(float *)(iVar15 + 0x10 + param_2) * *param_3 + *pfVar2;
  *pfVar1 = *(float *)(iVar15 + 0xd4 + param_2) * param_3[3] +
            fVar11 * fVar12 + fVar7 * fVar8 + fVar3 * fVar4 + *pfVar1;
  *(float *)(iVar15 + 0x18 + param_1) =
       *(float *)(iVar15 + 0xd8 + param_2) * param_3[3] +
       fVar13 * fVar14 + fVar9 * fVar10 + fVar5 * fVar6 + *(float *)(iVar15 + 0x18 + param_1);
  return;
}

