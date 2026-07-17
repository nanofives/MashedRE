
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_0056c310(int param_1, ..., int param_5, ..., int *param_9,
   int param_10, ..., float param_12)`. */

void FUN_0056c310(int param_1,undefined4 param_2,int param_3,undefined4 param_4,int param_5,
                 undefined4 param_6,undefined4 param_7,int param_8,int *param_9,int param_10,
                 undefined4 param_11,float param_12)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  float *extraout_EDX;
  int iVar9;
  float *pfVar10;
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

  iVar9 = 0;
  if (param_9[1] != 0) {
    pfVar10 = (float *)(param_5 + 0x14);
    do {
      local_48 = pfVar10[-1];
      local_44 = *pfVar10;
      iVar5 = *(int *)(*param_9 + iVar9 * 4);
      local_40 = pfVar10[1];
      if ((*(byte *)(param_10 + iVar5 * 4) & 1) != 0) {
        iVar8 = iVar5 * 0x10;
        pfVar1 = (float *)(iVar8 + param_8);
        fVar3 = pfVar1[2] * local_40 +
                *pfVar1 * local_48 + *(float *)(iVar8 + 4 + param_8) * local_44;
        FUN_0056bb80(iVar8 + param_3,pfVar1,fVar3 * param_12);
        local_48 = local_48 - fVar3 * *pfVar1;
        local_44 = local_44 - fVar3 * pfVar1[1];
        local_40 = local_40 - fVar3 * pfVar1[2];
      }
      FUN_0056bce0(iVar5 * 0x10 + param_3,&local_48,param_12);
      pfVar2 = (float *)(iVar5 * 0x40 + param_1);
      pfVar1 = pfVar2 + 0xc;
      local_24 = *pfVar1;
      local_20 = pfVar2[0xd];
      local_c = _DAT_005cc574 /
                (extraout_EDX[3] * extraout_EDX[3] +
                extraout_EDX[2] * extraout_EDX[2] +
                extraout_EDX[1] * extraout_EDX[1] + *extraout_EDX * *extraout_EDX);
      local_1c = pfVar2[0xe];
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
      pfVar2[3] = 4.2039e-45;
      *pfVar1 = 0.0;
      pfVar2[0xd] = 0.0;
      fVar7 = _DAT_005cc320 - (local_28 + local_2c);
      pfVar2[0xe] = 0.0;
      *pfVar2 = fVar7;
      pfVar2[1] = local_34 + local_14 * fVar4;
      pfVar2[2] = fVar6 * fVar3 - local_38;
      pfVar2[4] = local_14 * fVar4 - local_34;
      pfVar2[5] = _DAT_005cc320 - (local_28 + local_30);
      pfVar2[6] = local_c + local_3c;
      pfVar2[8] = fVar6 * fVar3 + local_38;
      pfVar2[9] = local_c - local_3c;
      pfVar2[10] = _DAT_005cc320 - (local_2c + local_30);
      *pfVar1 = pfVar10[-5] * param_12;
      pfVar2[0xd] = pfVar10[-4] * param_12;
      pfVar2[0xe] = pfVar10[-3] * param_12;
      *pfVar1 = *pfVar1 + local_24;
      pfVar2[0xd] = pfVar2[0xd] + local_20;
      pfVar2[0xe] = pfVar2[0xe] + local_1c;
      FUN_004c45f0(pfVar2);
      iVar9 = iVar9 + 1;
      pfVar10 = pfVar10 + 8;
    } while (iVar9 != param_9[1]);
  }
  return;
}

