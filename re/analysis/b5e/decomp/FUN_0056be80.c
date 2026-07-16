
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_0056be80(int *param_1, int param_2, float param_3)`. */

void FUN_0056be80(int *param_1,int param_2,float param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float *pfVar5;
  float *pfVar6;
  float *pfVar7;
  float *pfVar8;
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

  pfVar5 = (float *)(param_2 * 0x20 + param_1[2]);
  local_3c = pfVar5[4];
  pfVar8 = (float *)(param_2 * 0x40 + *param_1);
  local_38 = pfVar5[5];
  local_34 = pfVar5[6];
  pfVar7 = (float *)(param_1[4] + param_2 * 0x10);
  if ((*(byte *)(param_1[6] + param_2 * 4) & 1) != 0) {
    pfVar6 = (float *)(param_1[0x14] + param_2 * 0x10);
    fVar1 = pfVar6[2] * local_34 + local_3c * *pfVar6 + pfVar6[1] * local_38;
    FUN_0056bb80(pfVar7,pfVar6,fVar1 * param_3);
    fVar1 = -fVar1;
    local_3c = fVar1 * *pfVar6 + local_3c;
    local_38 = pfVar6[1] * fVar1 + local_38;
    local_34 = pfVar6[2] * fVar1 + local_34;
  }
  FUN_0056bce0(pfVar7,&local_3c,param_3);
  local_24 = param_3 * *pfVar5 + pfVar8[0xc];
  local_20 = param_3 * pfVar5[1] + pfVar8[0xd];
  local_1c = param_3 * pfVar5[2] + pfVar8[0xe];
  local_c = _DAT_005cc574 /
            (pfVar7[3] * pfVar7[3] +
            pfVar7[2] * pfVar7[2] + pfVar7[1] * pfVar7[1] + *pfVar7 * *pfVar7);
  fVar3 = local_c * *pfVar7;
  local_14 = pfVar7[1] * local_c;
  local_c = pfVar7[2] * local_c;
  local_3c = pfVar7[3] * fVar3;
  local_38 = pfVar7[3] * local_14;
  local_34 = pfVar7[3] * local_c;
  local_30 = fVar3 * *pfVar7;
  local_2c = local_14 * pfVar7[1];
  local_28 = local_c * pfVar7[2];
  local_c = local_c * pfVar7[1];
  fVar1 = pfVar7[2];
  fVar2 = *pfVar7;
  pfVar8[0xc] = 0.0;
  pfVar8[0xd] = 0.0;
  pfVar8[0xe] = 0.0;
  fVar4 = _DAT_005cc320 - (local_28 + local_2c);
  pfVar8[0xc] = local_24;
  pfVar8[0xd] = local_20;
  pfVar8[3] = 4.2039e-45;
  pfVar8[0xe] = local_1c;
  *pfVar8 = fVar4;
  pfVar8[1] = local_34 + local_14 * fVar2;
  pfVar8[2] = fVar3 * fVar1 - local_38;
  pfVar8[4] = local_14 * fVar2 - local_34;
  pfVar8[5] = _DAT_005cc320 - (local_28 + local_30);
  pfVar8[6] = local_c + local_3c;
  pfVar8[8] = fVar3 * fVar1 + local_38;
  pfVar8[9] = local_c - local_3c;
  pfVar8[10] = _DAT_005cc320 - (local_2c + local_30);
  FUN_004c45f0(pfVar8);
  return;
}

