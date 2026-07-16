
/* [C1 2026-05-19] If `*(short *)(param_1 + 0x15) != 0`: iterates `param_1[0x14]` array of vec3 (16
   bytes per element) (cited… */

void FUN_0055c0f0(float *param_1,float *param_2)

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
  uint uVar13;

  uVar13 = 0;
  if (*(short *)(param_1 + 0x15) != 0) {
    iVar12 = 0;
    do {
      pfVar11 = (float *)((int)param_1[0x14] + iVar12);
      uVar13 = uVar13 + 1;
      fVar1 = param_2[5];
      fVar2 = param_2[9];
      fVar3 = *pfVar11;
      fVar4 = param_2[1];
      fVar5 = pfVar11[2];
      fVar6 = param_2[10];
      fVar7 = pfVar11[1];
      fVar8 = param_2[6];
      fVar9 = param_2[2];
      fVar10 = *pfVar11;
      *pfVar11 = param_2[0xc] +
                 pfVar11[2] * param_2[8] + *pfVar11 * *param_2 + pfVar11[1] * param_2[4];
      *(float *)(iVar12 + 4 + (int)param_1[0x14]) =
           param_2[0xd] + fVar3 * fVar4 + pfVar11[2] * fVar2 + fVar1 * pfVar11[1];
      *(float *)(iVar12 + 8 + (int)param_1[0x14]) =
           param_2[0xe] + fVar9 * fVar10 + fVar7 * fVar8 + fVar5 * fVar6;
      iVar12 = iVar12 + 0x10;
    } while (uVar13 < *(ushort *)(param_1 + 0x15));
  }
  fVar1 = *param_1;
  fVar2 = param_1[1];
  fVar3 = param_1[2];
  *param_1 = fVar1 * *param_2 + fVar2 * param_2[4] + fVar3 * param_2[8];
  param_1[1] = fVar1 * param_2[1] + fVar2 * param_2[5] + fVar3 * param_2[9];
  fVar1 = fVar1 * param_2[2] + fVar2 * param_2[6] + fVar3 * param_2[10];
  param_1[2] = fVar1;
  if (*(short *)(param_1 + 0x15) != 0) {
    pfVar11 = (float *)param_1[0x14];
    param_1[3] = pfVar11[2] * fVar1 + *pfVar11 * *param_1 + pfVar11[1] * param_1[1];
    return;
  }
  param_1[3] = param_2[0xe] * fVar1 + param_2[0xc] * *param_1 + param_2[0xd] * param_1[1];
  return;
}

