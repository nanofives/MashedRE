
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `int FUN_00579c00(float *out_pos, uint feature_bitmask, int
   use_alternate_row,… */

undefined4 FUN_00579c00(float *param_1,uint param_2,int param_3,float *param_4)

{
  float fVar1;
  float fVar2;
  float *pfVar3;
  float *pfVar4;
  uint uVar5;
  float local_c;
  int local_8;
  float local_4;

  fVar2 = DAT_005d757c;
  uVar5 = 1;
  local_c = 0.0;
  local_4 = -1.0;
  param_1[2] = 0.0;
  param_1[1] = 0.0;
  *param_1 = 0.0;
  local_8 = 0;
  pfVar4 = param_4;
  if (0 < (int)param_2) {
    do {
      if ((param_2 & uVar5) != 0) {
        fVar1 = param_4[local_8 + 0x28 + param_2 * 4];
        if (fVar1 < fVar2) {
          fVar2 = fVar1;
        }
        if (local_4 < fVar1) {
          local_4 = fVar1;
        }
        local_c = fVar1 + local_c;
        pfVar3 = pfVar4 + 3;
        if (param_3 == 0) {
          pfVar3 = pfVar4;
        }
        *param_1 = fVar1 * *pfVar3 + *param_1;
        pfVar3 = pfVar4 + 3;
        if (param_3 == 0) {
          pfVar3 = pfVar4;
        }
        param_1[1] = pfVar3[1] * fVar1 + param_1[1];
        pfVar3 = pfVar4 + 3;
        if (param_3 == 0) {
          pfVar3 = pfVar4;
        }
        param_1[2] = pfVar3[2] * fVar1 + param_1[2];
      }
      uVar5 = uVar5 * 2;
      local_8 = local_8 + 1;
      pfVar4 = pfVar4 + 6;
    } while ((int)uVar5 <= (int)param_2);
    if (local_c != DAT_005d757c) {
      local_c = _DAT_005cc320 / local_c;
      *param_1 = local_c * *param_1;
      param_1[1] = param_1[1] * local_c;
      param_1[2] = param_1[2] * local_c;
    }
  }
  if (fVar2 <= local_4 * _DAT_005cea1c) {
    return 0;
  }
  return 1;
}

