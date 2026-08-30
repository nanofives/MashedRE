
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-06-01] `void FUN_00445aa0(float param_1, int *param_2)` â€” per-frame "type-0"
   camera-path */

void FUN_00445aa0(float param_1,int *param_2)

{
  float *pfVar1;
  undefined4 *puVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  int iVar10;
  int *piVar11;
  int iVar12;
  char *pcVar13;
  char *extraout_ECX;
  float10 fVar14;
  float10 fVar15;
  float10 fVar16;
  float local_4c;
  float local_44;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  float local_8;
  float local_4;
  
  piVar11 = param_2;
  iVar10 = (int)param_1;
  fVar7 = (float)DAT_007f1008 * _DAT_005cc948;
  FUN_00441c80(&local_18,param_2);
  if (*param_2 == 0) {
    *(undefined4 *)((int)param_1 + 0xa4) = 0x41400000;
    iVar12 = FUN_00426c00();
    if (iVar12 == 0x1a) {
      *(undefined4 *)((int)param_1 + 0xa4) = 0x41a00000;
    }
  }
  else {
    local_18 = (float)param_2[1];
    local_10 = (float)param_2[3];
    local_28 = *(float *)((int)param_1 + 0x3c) - local_18;
    local_24 = *(float *)((int)param_1 + 0x44) - local_10;
    fVar14 = (float10)FUN_004c3bf0(&local_28);
    if ((float10)_DAT_005cc35c < fVar14) {
      fVar14 = (float10)_DAT_005cc35c;
    }
    fVar15 = (float10)_DAT_005ccac8;
    *(int *)((int)param_1 + 0xa4) = param_2[2];
    local_18 = (float)((float10)local_18 + fVar14 * fVar15);
    local_10 = (float)(fVar14 * fVar15 + (float10)local_10);
  }
  fVar3 = *(float *)((int)param_1 + 0x40) - *(float *)((int)param_1 + 0xa4);
  if (fVar3 < _DAT_005cc9a0) {
    *(float *)((int)param_1 + 0x90) = fVar7 * _DAT_005cc31c + *(float *)((int)param_1 + 0x90);
  }
  if (_DAT_005cc9a0 < fVar3) {
    *(float *)((int)param_1 + 0x90) = *(float *)((int)param_1 + 0x90) - fVar7 * _DAT_005cc31c;
  }
  fVar8 = _DAT_005cc320 - fVar7;
  fVar5 = fVar8;
  if ((fVar3 < _DAT_005cc320) && (_DAT_005cc33c < fVar3)) {
    fVar5 = _DAT_005cc320 - (fVar7 + fVar7);
  }
  pfVar1 = (float *)((int)param_1 + 0x3c);
  *(float *)((int)param_1 + 0x90) = fVar5 * *(float *)((int)param_1 + 0x90);
  local_28 = *pfVar1 - local_18;
  local_24 = *(float *)((int)param_1 + 0x44) - local_10;
  FUN_004c3c60(&local_20,&local_28);
  fVar14 = (float10)FUN_004c3bf0(&local_28);
  fVar3 = (float)fVar14;
  fVar5 = fVar3;
  if (*(int *)((int)param_1 + 0xa8) == 0) {
    iVar12 = *param_2;
    param_2 = (int *)0x0;
    if (iVar12 == 0) {
      param_2 = (int *)0x40e00000;
      fVar5 = _DAT_005cd96c;
      if (fVar3 < 7.0) {
        *(undefined4 *)((int)param_1 + 0xa8) = 1;
        fVar5 = _DAT_005cd96c;
      }
    }
    else {
      if (fVar3 < _DAT_005ce18c) {
        FUN_004430a0(0);
      }
      pcVar13 = &DAT_007f1042;
      do {
        if (*pcVar13 != '\0') {
          FUN_004430a0(0);
          pcVar13 = extraout_ECX;
        }
        pcVar13 = pcVar13 + 0x4c;
        fVar5 = _DAT_005cd0a0;
      } while ((int)pcVar13 < 0x7f12a2);
    }
    fVar6 = fVar3 - (float)param_2;
    if ((*piVar11 != 0) && (fVar6 < _DAT_005cc950)) {
      fVar6 = _DAT_005cc950;
    }
    param_1 = fVar3;
    if (fVar3 < DAT_005d757c) {
      param_1 = 0.0;
    }
    fVar9 = DAT_005d757c;
    if ((DAT_005d757c <= fVar6) && (fVar9 = fVar6, _DAT_005cc358 < fVar6)) {
      fVar9 = _DAT_005cc358;
    }
    fVar5 = fVar9 * fVar7 * fVar5 * _DAT_005cc9c0;
    *(float *)(iVar10 + 0x8c) = *(float *)(iVar10 + 0x8c) - local_20 * fVar5;
    *(float *)(iVar10 + 0x94) = *(float *)(iVar10 + 0x94) - local_1c * fVar5;
    fVar5 = param_1;
    if (*(int *)(iVar10 + 0xa8) != 0) goto LAB_00445d74;
  }
  else {
LAB_00445d74:
    param_1 = fVar5;
    if (_DAT_005cc55c < param_1) {
      *(undefined4 *)(iVar10 + 0xa8) = 0;
    }
    if (*piVar11 != 0) {
      *(undefined4 *)(iVar10 + 0xa8) = 0;
    }
    fVar5 = _DAT_005cc9b8 - param_1;
    if (_DAT_005cc9b8 - param_1 < DAT_005d757c) {
      fVar5 = DAT_005d757c;
    }
    fVar5 = fVar5 * fVar7 * _DAT_005cc728 * _DAT_005ce188;
    *(float *)(iVar10 + 0x8c) = local_20 * fVar5 + *(float *)(iVar10 + 0x8c);
    *(float *)(iVar10 + 0x94) = local_1c * fVar5 + *(float *)(iVar10 + 0x94);
  }
  fVar5 = fVar8;
  if ((param_1 < _DAT_005cc55c) && (*(int *)(iVar10 + 0xa8) == 0)) {
    fVar5 = _DAT_005cc320 - (fVar7 + fVar7);
  }
  *(float *)(iVar10 + 0x8c) = fVar5 * *(float *)(iVar10 + 0x8c);
  *(float *)(iVar10 + 0x94) = fVar5 * *(float *)(iVar10 + 0x94);
  local_c = *(float *)(iVar10 + 0x8c) * fVar7;
  local_8 = *(float *)(iVar10 + 0x90) * fVar7;
  local_4 = *(float *)(iVar10 + 0x94) * fVar7;
  FUN_004c51a0(iVar10 + 0xc,&local_c,2);
  if ((*piVar11 != 0) && (FUN_00441c80(&local_18,piVar11), fVar3 < _DAT_005cc9f4)) {
    fVar5 = (_DAT_005cc9f4 - fVar3) * _DAT_005cd050;
    local_18 = local_18 - (local_18 - (float)piVar11[4]) * fVar5;
    local_14 = local_14 - (local_14 - (float)piVar11[5]) * fVar5;
    local_10 = local_10 - (local_10 - (float)piVar11[6]) * fVar5;
  }
  fVar14 = (float10)local_18 - (float10)*pfVar1;
  fVar5 = local_14 - *(float *)(iVar10 + 0x40);
  fVar6 = local_10 - *(float *)(iVar10 + 0x44);
  fVar15 = (float10)fsin((float10)(DAT_007f101c - 0x200 & 0x3ff) * (float10)_DAT_005ce184);
  if (fVar15 < (float10)DAT_005d757c) {
    fVar15 = -fVar15;
  }
  if (fVar15 < (float10)_DAT_005cc9c0) {
    fVar15 = (float10)_DAT_005cc9c0 - ((float10)_DAT_005cc9c0 - fVar15) * (float10)_DAT_005cc564;
  }
  *(float *)(iVar10 + 0xc0) = (float)(fVar15 * (float10)_DAT_005cc32c + (float10)_DAT_005cc56c);
  if (*piVar11 != 0) {
    *(undefined4 *)(iVar10 + 0xc0) = 0x3f19999a;
  }
  local_4c = *(float *)(iVar10 + 0xc0) * _DAT_005cc9b0;
  if (_DAT_005cc358 < local_4c) {
    local_4c = 5.0;
  }
  if ((*piVar11 != 0) && (fVar3 < _DAT_005cc35c)) {
    local_4c = fVar3;
  }
  if (fVar6 == DAT_005d757c) {
    if ((float10)DAT_005d757c <= fVar14) {
      fVar15 = (float10)_DAT_005ccad0;
    }
    else {
      fVar15 = (float10)_DAT_005cd324;
    }
  }
  else {
    fVar15 = (float10)fpatan(fVar14 / (float10)fVar6,(float10)1);
    fVar15 = (float10)_DAT_005cd09c - -(fVar15 * (float10)_DAT_005ccae0);
    if (DAT_005d757c < fVar6) {
      fVar15 = fVar15 + (float10)_DAT_005cd09c;
    }
  }
  fVar9 = fVar7 * _DAT_005cc358;
  local_44 = fVar9;
  if ((*piVar11 != 0) && (_DAT_005cc574 < fVar3)) {
    local_44 = fVar7 * _DAT_005cc55c;
  }
  fVar15 = (float10)*(float *)(iVar10 + 0xb0) - fVar15;
  if ((float10)_DAT_005ccac4 <= fVar15) {
    do {
      fVar15 = fVar15 - (float10)_DAT_005ccac4;
    } while ((float10)_DAT_005ccac4 <= fVar15);
  }
  if (fVar15 < (float10)DAT_005d757c) {
    do {
      fVar15 = fVar15 + (float10)_DAT_005ccac4;
    } while (fVar15 < (float10)DAT_005d757c);
  }
  if ((float10)_DAT_005cd09c < fVar15) {
    fVar15 = -((float10)_DAT_005ccac4 - fVar15);
  }
  if (fVar15 <= (float10)local_4c) {
    if (fVar15 < (float10)DAT_005d757c) {
      fVar15 = -fVar15;
    }
    if (fVar15 <= (float10)local_4c) {
      local_44 = *(float *)(iVar10 + 0xbc) * _DAT_005cc32c;
    }
    else {
      local_44 = local_44 + *(float *)(iVar10 + 0xbc);
    }
  }
  else {
    local_44 = *(float *)(iVar10 + 0xbc) - local_44;
  }
  *(float *)(iVar10 + 0xbc) = local_44;
  fVar4 = (float)(fVar15 / (float10)local_4c);
  if ((float10)_DAT_005cc35c <= fVar15 / (float10)local_4c) {
    if (_DAT_005cc55c <= fVar4) {
      param_1 = 0.0;
    }
    else {
      param_1 = (_DAT_005cc55c - fVar4) * _DAT_005ce180;
    }
  }
  else {
    param_1 = 100.0;
  }
  local_c = *pfVar1 - local_18;
  local_8 = *(float *)(iVar10 + 0x40) - local_14;
  local_4 = *(float *)(iVar10 + 0x44) - local_10;
  fVar15 = (float10)FUN_004c3ac0(&local_c);
  fVar15 = fVar15 - (float10)*(float *)(iVar10 + 0xa4);
  if (fVar15 < (float10)DAT_005d757c) {
    fVar15 = -fVar15;
  }
  fVar16 = (float10)param_1 - (fVar15 + fVar15);
  if (fVar16 < (float10)DAT_005d757c) {
    fVar16 = (float10)DAT_005d757c;
  }
  *(float *)(iVar10 + 200) = (float)fVar16;
  *(float *)(iVar10 + 0xb0) = *(float *)(iVar10 + 0xbc) + *(float *)(iVar10 + 0xb0);
  fVar4 = fVar8;
  if (fVar15 < (float10)(local_4c + local_4c)) {
    fVar4 = _DAT_005cc320 - fVar7 * _DAT_005cc35c;
  }
  *(float *)(iVar10 + 0xbc) = fVar4 * *(float *)(iVar10 + 0xbc);
  fVar6 = (float)fVar14 * (float)fVar14 + fVar6 * fVar6;
  if (fVar6 == DAT_005d757c) {
LAB_004462a0:
    if (DAT_005d757c <= fVar5) {
      fVar15 = (float10)_DAT_005ccad0;
    }
    else {
      fVar15 = (float10)_DAT_005cd324;
    }
  }
  else {
    fVar14 = (float10)FUN_004c3b30(fVar6);
    if (fVar14 == (float10)DAT_005d757c) goto LAB_004462a0;
    fVar15 = (float10)fpatan((float10)fVar5 / (float10)(float)fVar14,(float10)1);
    fVar15 = (float10)_DAT_005cd09c - -(fVar15 * (float10)_DAT_005ccae0);
    if (DAT_005d757c < (float)fVar14) {
      fVar15 = fVar15 + (float10)_DAT_005cd09c;
    }
  }
  fVar14 = (float10)*(float *)(iVar10 + 0xac) - ((float10)_DAT_005ccac4 - fVar15);
  if ((float10)_DAT_005ccac4 <= fVar14) {
    do {
      fVar14 = fVar14 - (float10)_DAT_005ccac4;
    } while ((float10)_DAT_005ccac4 <= fVar14);
  }
  if (fVar14 < (float10)DAT_005d757c) {
    do {
      fVar14 = fVar14 + (float10)_DAT_005ccac4;
    } while (fVar14 < (float10)DAT_005d757c);
  }
  if ((float10)_DAT_005cd09c < fVar14) {
    fVar14 = -((float10)_DAT_005ccac4 - fVar14);
  }
  if (fVar14 <= (float10)local_4c) {
    if (fVar14 < -(float10)local_4c) {
      *(float *)(iVar10 + 0xb8) = fVar9 + *(float *)(iVar10 + 0xb8);
      fVar14 = -fVar14;
      goto LAB_00446376;
    }
    fVar9 = *(float *)(iVar10 + 0xb8) * _DAT_005cc32c;
  }
  else {
    fVar9 = *(float *)(iVar10 + 0xb8) - fVar9;
  }
  *(float *)(iVar10 + 0xb8) = fVar9;
LAB_00446376:
  *(float *)(iVar10 + 0xac) = *(float *)(iVar10 + 0xb8) + *(float *)(iVar10 + 0xac);
  if (fVar14 < (float10)(local_4c + local_4c)) {
    fVar8 = _DAT_005cc320 - fVar7 * _DAT_005cc35c;
  }
  *(float *)(iVar10 + 0xb8) = fVar8 * *(float *)(iVar10 + 0xb8);
  fVar14 = (float10)fsin((float10)(DAT_007f101c & 0x3ff) * (float10)_DAT_005ce17c);
  fVar14 = fVar14 * (float10)_DAT_005cc72c;
  if ((*piVar11 != 0) && (fVar3 < _DAT_005cc35c)) {
    fVar15 = (float10)fVar3 - (float10)_DAT_005cc320;
    if (fVar15 < (float10)DAT_005d757c) {
      fVar15 = (float10)DAT_005d757c;
    }
    fVar14 = fVar14 * fVar15 * (float10)_DAT_005ccac8;
  }
  *(float *)(iVar10 + 0xb4) = (float)fVar14;
  puVar2 = (undefined4 *)(iVar10 + 0x4c);
  *(undefined4 *)(iVar10 + 0x74) = 0x3f800000;
  *(undefined4 *)(iVar10 + 0x60) = 0x3f800000;
  *puVar2 = 0x3f800000;
  *(undefined4 *)(iVar10 + 0x5c) = 0;
  *(undefined4 *)(iVar10 + 0x54) = 0;
  *(undefined4 *)(iVar10 + 0x50) = 0;
  *(undefined4 *)(iVar10 + 0x70) = 0;
  *(undefined4 *)(iVar10 + 0x6c) = 0;
  *(undefined4 *)(iVar10 + 100) = 0;
  *(undefined4 *)(iVar10 + 0x84) = 0;
  *(undefined4 *)(iVar10 + 0x80) = 0;
  *(undefined4 *)(iVar10 + 0x7c) = 0;
  *(uint *)(iVar10 + 0x58) = *(uint *)(iVar10 + 0x58) | 0x20003;
  FUN_004c4d20(puVar2,&DAT_006146fc,*(undefined4 *)(iVar10 + 0xb0),2);
  FUN_004c4d20(puVar2,puVar2,*(undefined4 *)(iVar10 + 0xac),2);
  FUN_004c4d20(puVar2,(undefined4 *)(iVar10 + 0x6c),*(undefined4 *)(iVar10 + 0xb4),2);
  FUN_004c51a0(puVar2,pfVar1,2);
  return;
}

