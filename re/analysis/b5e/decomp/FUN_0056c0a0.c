
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_0056c0a0(int param_1, ..., int *param_6, int param_7, ...,
   int param_9, ..., float param_11)`. */

void FUN_0056c0a0(int param_1,undefined4 param_2,int param_3,undefined4 param_4,int param_5,
                 int *param_6,int param_7,undefined4 param_8,int param_9,undefined4 param_10,
                 float param_11)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  float *pfVar9;
  float *extraout_EDX;
  int iVar10;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_14;
  float local_c;

  iVar10 = 0;
  if (param_6[1] != 0) {
    do {
      iVar5 = *(int *)(*param_6 + iVar10 * 4);
      local_48 = *(float *)(iVar5 * 0x20 + 0x10 + param_5);
      pfVar1 = (float *)(iVar5 * 0x20 + param_5);
      local_44 = pfVar1[5];
      local_40 = pfVar1[6];
      if ((*(byte *)(param_9 + iVar5 * 4) & 1) != 0) {
        iVar8 = iVar5 * 0x10;
        pfVar2 = (float *)(iVar8 + param_7);
        fVar3 = pfVar2[2] * local_40 +
                *pfVar2 * local_48 + *(float *)(iVar8 + 4 + param_7) * local_44;
        FUN_0056bb80(iVar8 + param_3,pfVar2,fVar3 * param_11);
        local_48 = local_48 - fVar3 * *pfVar2;
        local_44 = local_44 - fVar3 * pfVar2[1];
        local_40 = local_40 - fVar3 * pfVar2[2];
      }
      FUN_0056bce0(iVar5 * 0x10 + param_3,&local_48,param_11);
      pfVar9 = (float *)(param_1 + iVar5 * 0x40);
      pfVar2 = pfVar9 + 0xc;
      local_24 = *pfVar2;
      local_20 = pfVar9[0xd];
      local_c = _DAT_005cc574 /
                (extraout_EDX[3] * extraout_EDX[3] +
                extraout_EDX[2] * extraout_EDX[2] +
                extraout_EDX[1] * extraout_EDX[1] + *extraout_EDX * *extraout_EDX);
      local_1c = pfVar9[0xe];
      fVar6 = *extraout_EDX * local_c;
      local_14 = extraout_EDX[1] * local_c;
      local_c = extraout_EDX[2] * local_c;
      local_3c = extraout_EDX[3] * fVar6;
      local_38 = extraout_EDX[3] * local_14;
      local_34 = extraout_EDX[3] * local_c;
      local_30 = fVar6 * *extraout_EDX;
      local_2c = local_14 * extraout_EDX[1];
      local_28 = local_c * extraout_EDX[2];
      local_c = local_c * extraout_EDX[1];
      fVar3 = extraout_EDX[2];
      fVar4 = *extraout_EDX;
      pfVar9[3] = 4.2039e-45;
      *pfVar2 = 0.0;
      pfVar9[0xd] = 0.0;
      fVar7 = _DAT_005cc320 - (local_28 + local_2c);
      pfVar9[0xe] = 0.0;
      *pfVar9 = fVar7;
      pfVar9[1] = local_34 + local_14 * fVar4;
      pfVar9[2] = fVar6 * fVar3 - local_38;
      pfVar9[4] = local_14 * fVar4 - local_34;
      pfVar9[5] = _DAT_005cc320 - (local_28 + local_30);
      pfVar9[6] = local_c + local_3c;
      pfVar9[8] = fVar6 * fVar3 + local_38;
      pfVar9[9] = local_c - local_3c;
      pfVar9[10] = _DAT_005cc320 - (local_2c + local_30);
      *pfVar2 = *pfVar1 * param_11;
      pfVar9[0xd] = pfVar1[1] * param_11;
      pfVar9[0xe] = pfVar1[2] * param_11;
      *pfVar2 = *pfVar2 + local_24;
      pfVar9[0xd] = pfVar9[0xd] + local_20;
      pfVar9[0xe] = pfVar9[0xe] + local_1c;
      FUN_004c45f0(pfVar9);
      iVar10 = iVar10 + 1;
    } while (iVar10 != param_6[1]);
  }
  return;
}

