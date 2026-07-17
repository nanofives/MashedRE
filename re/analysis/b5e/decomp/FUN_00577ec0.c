
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `int FUN_00577ec0(float *p1_start, float *p1_dir, float
   p1_min, float p1_max,… */

undefined4
FUN_00577ec0(float *param_1,float *param_2,float param_3,float param_4,float *param_5,float *param_6
            ,float param_7,float param_8,float *param_9,float *param_10,float *param_11)

{
  bool bVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;

  fVar2 = param_2[2] * param_2[2] + param_2[1] * param_2[1] + *param_2 * *param_2;
  fVar3 = param_6[2] * param_6[2] + param_6[1] * param_6[1] + *param_6 * *param_6;
  fVar4 = *param_5 - *param_1;
  fVar5 = param_5[1] - param_1[1];
  fVar6 = param_5[2] - param_1[2];
  if (fVar2 == DAT_005d757c) {
    *param_9 = param_3;
    bVar1 = fVar3 == DAT_005d757c;
    *param_11 = 0.0;
    if (bVar1) {
      *param_10 = param_7;
      return 0;
    }
    fVar2 = -(param_6[2] * fVar6 + fVar4 * *param_6 + param_6[1] * fVar5);
    if (fVar3 * param_7 < fVar2) {
      if (fVar3 * param_8 < fVar2 == (fVar3 * param_8 == fVar2)) {
        *param_10 = fVar2 / fVar3;
        return 0;
      }
      *param_10 = param_8;
      return 0;
    }
    *param_10 = param_7;
    return 0;
  }
  if (fVar3 != DAT_005d757c) {
    fVar7 = param_2[2] * param_6[2] + *param_6 * *param_2 + param_6[1] * param_2[1];
    fVar9 = fVar3 * fVar2;
    fVar10 = fVar9 - fVar7 * fVar7;
    fVar8 = param_2[2] * fVar6 + fVar4 * *param_2 + param_2[1] * fVar5;
    fVar4 = param_6[2] * fVar6 + fVar4 * *param_6 + param_6[1] * fVar5;
    if (fVar10 <= _DAT_005cd03c * fVar9) {
      fVar9 = _DAT_005cc320 / fVar9;
      fVar3 = fVar9 * fVar3;
      fVar5 = fVar7 * fVar3;
      fVar3 = fVar3 * fVar8;
      if (fVar5 <= DAT_005d757c) {
        param_1 = (float *)param_8;
        param_8 = param_7;
      }
      else {
        param_1 = (float *)param_7;
      }
      fVar6 = (float)param_1 * fVar5 + fVar3;
      fVar3 = fVar5 * param_8 + fVar3;
      if (fVar3 < param_3 == (fVar3 == param_3)) {
        if (fVar6 < param_4) {
          if (fVar6 < param_3) {
            fVar6 = param_3;
          }
          *param_9 = fVar6;
          if (param_4 < fVar3) {
            fVar3 = param_4;
          }
          *param_11 = fVar3 - fVar6;
          *param_10 = fVar9 * fVar2 * (fVar7 * *param_9 - fVar4);
          return 0;
        }
        *param_9 = param_4;
        *param_11 = 0.0;
        *param_10 = (float)param_1;
        return 0;
      }
      *param_9 = param_3;
      *param_11 = 0.0;
      *param_10 = param_8;
      return 0;
    }
    fVar5 = fVar8 * fVar3 - fVar4 * fVar7;
    fVar6 = param_3;
    if ((fVar10 * param_3 <= fVar5) && (fVar6 = param_4, fVar5 <= fVar10 * param_4)) {
      fVar3 = fVar8 * fVar7 - fVar4 * fVar2;
      if ((fVar3 < fVar10 * param_7) || (param_7 = param_8, fVar10 * param_8 < fVar3)) {
        *param_10 = param_7;
        fVar8 = fVar7 * *param_10 + fVar8;
        if (fVar8 < fVar2 * param_3) {
          *param_9 = param_3;
          *param_11 = 0.0;
          return 1;
        }
        param_10 = param_9;
        if (fVar8 <= fVar2 * param_4) {
          *param_9 = fVar8 / fVar2;
          *param_11 = 0.0;
          return 1;
        }
      }
      else {
        param_4 = _DAT_005cc320 / fVar10;
        *param_9 = fVar5 * param_4;
        param_4 = param_4 * fVar3;
      }
      *param_10 = param_4;
      *param_11 = 0.0;
      return 1;
    }
    *param_9 = fVar6;
    fVar4 = fVar7 * *param_9 - fVar4;
    if ((fVar3 * param_7 <= fVar4) && (param_7 = param_8, fVar4 <= fVar3 * param_8)) {
      *param_10 = fVar4 / fVar3;
      *param_11 = 0.0;
      return 1;
    }
    *param_10 = param_7;
    fVar8 = fVar7 * *param_10 + fVar8;
    if (fVar8 < fVar2 * param_3) {
      *param_9 = param_3;
      *param_11 = 0.0;
      return 1;
    }
    if (fVar8 <= fVar2 * param_4) {
      *param_9 = fVar8 / fVar2;
      *param_11 = 0.0;
      return 1;
    }
    *param_9 = param_4;
    *param_11 = 0.0;
    return 1;
  }
  *param_10 = param_7;
  *param_11 = 0.0;
  fVar3 = param_2[2] * fVar6 + fVar4 * *param_2 + param_2[1] * fVar5;
  if (fVar3 <= fVar2 * param_3) {
    *param_9 = param_3;
    return 0;
  }
  if (fVar2 * param_4 < fVar3 == (fVar2 * param_4 == fVar3)) {
    *param_9 = fVar3 / fVar2;
    return 0;
  }
  *param_9 = param_4;
  return 0;
}

