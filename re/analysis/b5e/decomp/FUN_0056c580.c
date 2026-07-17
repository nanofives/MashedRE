
/* [C1 2026-05-19] Signature: `void FUN_0056c580(int param_1, ..., int param_6, int *param_7, int
   param_8)`. */

void FUN_0056c580(int param_1,undefined4 param_2,int param_3,undefined4 param_4,undefined4 param_5,
                 int param_6,int *param_7,int param_8)

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
  float *pfVar11;
  int iVar12;
  int iVar13;
  float *pfVar14;
  float *pfVar15;
  float *pfVar16;
  int local_54;
  float local_50 [4];
  float local_40;
  float local_3c;
  float local_38;

  local_54 = 0;
  if (param_7[1] != 0) {
    pfVar11 = (float *)(param_8 + 4);
    pfVar14 = (float *)(param_3 + 8);
    do {
      fVar4 = pfVar11[2] * pfVar11[2];
      fVar1 = pfVar11[-1];
      iVar13 = *(int *)(*param_7 + local_54 * 4);
      fVar2 = pfVar11[-1];
      fVar3 = *pfVar11;
      pfVar15 = pfVar14 + -2;
      pfVar16 = local_50;
      for (iVar12 = 8; iVar12 != 0; iVar12 = iVar12 + -1) {
        *pfVar16 = *pfVar15;
        pfVar15 = pfVar15 + 1;
        pfVar16 = pfVar16 + 1;
      }
      iVar13 = iVar13 * 0x20;
      fVar5 = fVar1 * fVar1 + pfVar11[3] * *pfVar11 + pfVar11[7] * pfVar11[1];
      fVar2 = pfVar11[8] * pfVar11[1] + pfVar11[4] * *pfVar11 + fVar2 * fVar3;
      fVar3 = pfVar11[9] * pfVar11[1] + pfVar11[5] * *pfVar11 + pfVar11[1] * pfVar11[-1];
      fVar7 = pfVar11[7] * pfVar11[5] + pfVar11[4] * pfVar11[3] + pfVar11[3] * pfVar11[-1];
      fVar1 = pfVar11[8] * pfVar11[5] + pfVar11[4] * pfVar11[4] + pfVar11[3] * *pfVar11;
      fVar6 = pfVar11[9] * pfVar11[5] + pfVar11[5] * pfVar11[4] + pfVar11[3] * pfVar11[1];
      fVar8 = pfVar11[9] * pfVar11[7] + pfVar11[8] * pfVar11[3] + pfVar11[7] * pfVar11[-1];
      fVar9 = pfVar11[9] * pfVar11[8] + pfVar11[8] * pfVar11[4] + pfVar11[7] * *pfVar11;
      fVar10 = pfVar11[9] * pfVar11[9] + pfVar11[8] * pfVar11[5] + pfVar11[7] * pfVar11[1];
      *(float *)(iVar13 + param_1) = fVar4 * *(float *)(iVar13 + param_6);
      *(float *)(iVar13 + 4 + param_1) = fVar4 * *(float *)(iVar13 + 4 + param_6);
      *(float *)(iVar13 + 8 + param_1) = fVar4 * *(float *)(iVar13 + 8 + param_6);
      *(float *)(iVar13 + 0x10 + param_1) =
           fVar5 * *(float *)(iVar13 + 0x10 + param_6) +
           fVar8 * *(float *)(iVar13 + 0x18 + param_6) + fVar7 * *(float *)(iVar13 + 0x14 + param_6)
      ;
      *(float *)(iVar13 + 0x14 + param_1) =
           fVar2 * *(float *)(iVar13 + 0x10 + param_6) +
           fVar9 * *(float *)(iVar13 + 0x18 + param_6) + fVar1 * *(float *)(iVar13 + 0x14 + param_6)
      ;
      *(float *)(iVar13 + 0x18 + param_1) =
           fVar3 * *(float *)(iVar13 + 0x10 + param_6) +
           fVar10 * *(float *)(iVar13 + 0x18 + param_6) +
           fVar6 * *(float *)(iVar13 + 0x14 + param_6);
      local_54 = local_54 + 1;
      pfVar11 = pfVar11 + 0xc;
      pfVar14[-2] = local_50[0] * fVar4;
      pfVar14[-1] = local_50[1] * fVar4;
      *pfVar14 = local_50[2] * fVar4;
      pfVar14[2] = local_38 * fVar8 + local_40 * fVar5 + local_3c * fVar7;
      pfVar14[3] = local_38 * fVar9 + local_40 * fVar2 + local_3c * fVar1;
      pfVar14[4] = local_38 * fVar10 + local_40 * fVar3 + local_3c * fVar6;
      pfVar14 = pfVar14 + 8;
    } while (local_54 != param_7[1]);
  }
  return;
}

