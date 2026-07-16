
/* [C1 2026-05-20] Signature recovered: `int FUN_00577cb0(float *poly_verts, uint vert_count, float
   *probe_dir, float… */

undefined4
FUN_00577cb0(float *param_1,uint param_2,float *param_3,float *param_4,float *param_5,uint *param_6)

{
  uint uVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float *pfVar6;
  undefined4 uVar7;
  float *pfVar8;
  uint uVar9;
  float local_14;
  float local_10;
  float local_c;
  float local_8;
  float local_4;

  local_14 = -3.4028235e+38;
  local_10 = 1.0;
  *param_6 = param_2;
  if (param_2 != 0) {
    pfVar8 = param_1 + 2;
    uVar9 = 0;
    do {
      uVar1 = uVar9 + 1;
      pfVar6 = param_1 + ((int)((1 - param_2) + uVar9) >> 0x1f & uVar1) * 4;
      fVar2 = *pfVar6 - pfVar8[-2];
      fVar3 = pfVar6[1] - pfVar8[-1];
      fVar4 = pfVar6[2] - *pfVar8;
      fVar5 = ((param_4[1] - pfVar8[-1]) * *param_3 - (*param_4 - pfVar8[-2]) * param_3[1]) * fVar4
              + ((param_4[2] - *pfVar8) * param_3[1] - (param_4[1] - pfVar8[-1]) * param_3[2]) *
                fVar2 + ((*param_4 - pfVar8[-2]) * param_3[2] - (param_4[2] - *pfVar8) * *param_3) *
                        fVar3;
      if (DAT_005d757c <= fVar5) {
        fVar2 = fVar4 * fVar4 + fVar2 * fVar2 + fVar3 * fVar3;
        if (fVar2 * local_14 < local_10 * fVar5 * fVar5) {
          *param_6 = uVar9;
          local_14 = fVar5 * fVar5;
          local_10 = fVar2;
        }
      }
      pfVar8 = pfVar8 + 4;
      uVar9 = uVar1;
    } while (uVar1 < param_2);
    uVar9 = *param_6;
    if (uVar9 != param_2) {
      pfVar8 = param_1 + ((int)((uVar9 - param_2) + 1) >> 0x1f & uVar9 + 1) * 4;
      param_1 = param_1 + uVar9 * 4;
      local_c = *pfVar8 - *param_1;
      local_8 = pfVar8[1] - param_1[1];
      local_4 = pfVar8[2] - param_1[2];
      uVar7 = FUN_00577be0(param_1,&local_c,0,0x3f800000,param_4,param_5);
      return uVar7;
    }
  }
  fVar2 = *param_1 - *param_4;
  fVar4 = param_1[1] - param_4[1];
  fVar3 = param_1[2] - param_4[2];
  *param_5 = (param_3[2] * fVar3 + fVar2 * *param_3 + param_3[1] * fVar4) * *param_3 + *param_4;
  param_5[1] = (param_3[2] * fVar3 + fVar2 * *param_3 + param_3[1] * fVar4) * param_3[1] +
               param_4[1];
  param_5[2] = (param_3[2] * fVar3 + fVar2 * *param_3 + param_3[1] * fVar4) * param_3[2] +
               param_4[2];
  return 2;
}

