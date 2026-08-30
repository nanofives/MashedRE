
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-06-01] `void FUN_00446520(int *param_1, int param_2 /*nonzero = force reset*/)` â€” the
   master */

void FUN_00446520(int *param_1,int param_2)

{
  float fVar1;
  int iVar2;
  float *pfVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  float10 fVar6;
  float fVar7;
  int local_1c4;
  float local_1bc;
  int local_1b8;
  float local_1b4;
  float local_19c;
  float local_198;
  float local_194;
  float local_190;
  float local_18c;
  float local_188;
  float local_184;
  float local_180;
  float local_17c;
  float local_178;
  undefined4 local_174;
  float local_170;
  undefined1 local_16c [56];
  float local_134;
  float local_12c;
  float local_128;
  float local_124;
  float local_120;
  float local_11c;
  float local_118;
  undefined4 local_114;
  int local_110;
  float local_10c;
  float local_108;
  float local_104;
  float local_100;
  float local_fc;
  float local_f8;
  float local_f4;
  float local_f0;
  int local_ec;
  float local_e8;
  float local_e4;
  int local_e0;
  float local_dc;
  float local_d8;
  int local_d4;
  int local_d0;
  float local_cc;
  float local_c8;
  float local_c4;
  float local_c0;
  float local_bc;
  float local_b8;
  float local_b4;
  float local_b0;
  undefined1 local_ac [68];
  int local_68;
  int local_64;
  float local_60;
  int local_5c;
  uint local_58;
  float local_54;
  float local_50;
  float local_4c;
  float local_48;
  int local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  int local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  int local_8;
  
  for (local_58 = 0; (int)local_58 < 4; local_58 = local_58 + 1) {
    FUN_0041ef60();
  }
  param_1[9] = DAT_00803344;
  FUN_00442600();
  local_d0 = FUN_0040e350();
  if ((DAT_007f0fd0 == 5) && (iVar2 = FUN_00405890(), iVar2 != 0)) {
    pfVar3 = (float *)FUN_00407600();
    local_12c = *pfVar3;
    local_128 = pfVar3[1];
    local_124 = pfVar3[2];
    local_114 = FUN_004671a0();
    uVar4 = FUN_00467210();
    local_178 = DAT_006146f0;
    local_174 = DAT_006146f4;
    local_170 = (float)DAT_006146f8;
    local_120 = 0.0;
    fVar6 = (float10)FUN_004a37b0();
    local_11c = (float)(fVar6 * (float10)_DAT_005ce1e0 + (float10)_DAT_005ce1d8);
    local_118 = 0.0;
    FUN_004c39b0(&local_120,&local_120);
    local_190 = local_12c;
    local_18c = local_128;
    local_188 = local_124;
    local_19c = local_120 * 5.0 + local_12c;
    local_198 = local_11c * 5.0 + local_128;
    local_194 = local_118 * 5.0 + local_124;
    local_184 = local_19c;
    local_180 = local_198;
    local_17c = local_194;
    uVar5 = FUN_0045bfe0(&local_190,local_16c);
    iVar2 = FUN_004b4cd0(uVar5);
    if (iVar2 == 0) {
      if ((float)param_1[0x269] < _DAT_005cd088 != ((float)param_1[0x269] == _DAT_005cd088)) {
        param_1[0x269] = 0x40200000;
      }
      if (5.0 <= (float)param_1[0x269]) {
        param_1[0x269] = 0x40a00000;
      }
      else {
        param_1[0x269] = (int)((float)param_1[0x269] * _DAT_005ce1d4);
      }
    }
    else {
      FUN_0045c350(local_16c,&local_12c);
      param_1[0x269] = (int)(local_134 * 5.0 - _DAT_005cc56c);
    }
    local_19c = (float)param_1[0x269] * local_120 + local_12c;
    local_198 = (float)param_1[0x269] * local_11c + local_128;
    local_194 = (float)param_1[0x269] * local_118 + local_124;
    FUN_004c51a0(uVar4,&local_19c,0);
    fVar6 = (float10)FUN_004a3700();
    local_178 = (float)fVar6;
    fVar6 = (float10)FUN_004a37b0();
    local_170 = (float)fVar6;
    FUN_004b4430(uVar4,&local_12c,&local_178);
    param_1[0x16] = 0x3f4ccccd;
    FUN_00441700();
  }
  else if (((local_d0 == 6) && (iVar2 = FUN_0042f6a0(), iVar2 != 0xb)) ||
          (FUN_004464c0(), param_1[7] == 0)) {
    for (local_58 = 0; (int)local_58 < 4; local_58 = local_58 + 1) {
      FUN_0041f120(local_58,0);
      if (_DAT_005ce1d0 <= 0.0) {
        FUN_0041ef60();
      }
      else {
        FUN_0041ef60();
      }
    }
    iVar2 = FUN_0042f6a0();
    if ((((iVar2 == 2) || (DAT_007f0fd0 == 5)) || (DAT_007f0fd0 == 10)) || (DAT_007f0fd0 == 9)) {
      FUN_0041ef60(0,0);
    }
    FUN_00442a60();
    FUN_0040e180(&local_2c,&local_68);
    if (DAT_007f0fd0 == 7) {
      local_1b8 = 0;
      local_1b4 = -1.0;
      local_1bc = -1.0;
      for (local_58 = 0; (int)local_58 < 4; local_58 = local_58 + 1) {
        fVar6 = (float10)FUN_00417730();
        if ((float10)local_1b4 < fVar6) {
          local_1b8 = local_58;
          local_1b4 = (float)fVar6;
        }
      }
      local_1c4 = local_1b8;
      for (local_58 = 0; (int)local_58 < 4; local_58 = local_58 + 1) {
        if ((local_58 != local_1b8) && (iVar2 = FUN_0046c7b0(), iVar2 == 1)) {
          fVar6 = (float10)FUN_00417730();
          fVar7 = local_1b4 - (float)fVar6;
          if ((fVar7 < 0.03) && (local_1bc < fVar7)) {
            local_1c4 = local_58;
            local_1bc = fVar7;
          }
        }
      }
      local_2c = local_1b8;
      local_68 = local_1c4;
    }
    if (DAT_007f0fd0 == 4) {
      local_68 = 0;
      local_2c = 0;
    }
    if (DAT_007f0fd0 == 9) {
      local_68 = 0;
      local_2c = 0;
    }
    if (DAT_007f0fd0 == 8) {
      local_68 = 0;
      local_2c = 0;
    }
    FUN_0046d4a0(&local_64,local_2c);
    local_40 = *(float *)(local_64 + 0x30);
    local_3c = *(float *)(local_64 + 0x34);
    local_c0 = *(float *)(local_64 + 0x38);
    FUN_0046cb30(&local_24,local_2c);
    local_24 = local_24 * _DAT_005ccd18;
    local_20 = local_20 * _DAT_005ccd18;
    local_1c = local_1c * _DAT_005ccd18;
    local_40 = local_40 + local_24;
    local_c0 = local_c0 + local_1c;
    FUN_0046d4a0(&local_64,local_68);
    local_24 = local_40 - *(float *)(local_64 + 0x30);
    local_20 = local_3c - *(float *)(local_64 + 0x34);
    local_1c = local_c0 - *(float *)(local_64 + 0x38);
    FUN_0046cb30(&local_54,local_2c);
    local_54 = local_54 * _DAT_005ccd18;
    local_50 = local_50 * _DAT_005ccd18;
    local_4c = local_4c * _DAT_005ccd18;
    local_cc = local_24 - local_54;
    local_c4 = local_1c - local_4c;
    local_c8 = 0.0;
    local_24 = local_cc;
    local_1c = local_c4;
    fVar6 = (float10)FUN_004c3ac0();
    local_d8 = (float)(fVar6 * (float10)_DAT_005cc9bc);
    local_30 = local_d8;
    if (((local_2c == local_68) && (local_d8 < _DAT_005cc320)) &&
       (FUN_0046cbb0(local_2c,&local_e0,&local_5c), local_e0 != 0)) {
      local_d8 = 8.0;
      local_30 = 8.0;
    }
    local_48 = local_40 - local_24 / _DAT_005cc574;
    local_fc = local_3c - local_20 / _DAT_005cc574;
    local_e4 = local_c0 - local_1c / _DAT_005cc574;
    local_d4 = 0;
    if ((local_2c == param_1[0x265]) && (local_68 == param_1[0x266])) {
      local_d4 = 1;
    }
    if ((local_2c == param_1[0x266]) && (local_68 == param_1[0x265])) {
      local_d4 = 1;
    }
    if (param_2 != 0) {
      local_d4 = 0;
    }
    local_c0 = local_e4;
    local_40 = local_48;
    local_3c = local_fc;
    if (local_d4 == 0) {
      param_1[0x267] = (int)(_DAT_007f100c + _DAT_007f100c + (float)param_1[0x267]);
      if ((float)param_1[0x267] < _DAT_005cc320) {
        FUN_0046d4a0(&local_64,param_1[0x265]);
        local_38 = *(float *)(local_64 + 0x30);
        local_34 = *(float *)(local_64 + 0x34);
        local_b0 = *(float *)(local_64 + 0x38);
        FUN_0046d4a0(&local_64,param_1[0x266]);
        local_cc = local_38 - *(float *)(local_64 + 0x30);
        local_20 = local_34 - *(float *)(local_64 + 0x34);
        local_c4 = local_b0 - *(float *)(local_64 + 0x38);
        local_c8 = 0.0;
        local_24 = local_cc;
        local_1c = local_c4;
        fVar6 = (float10)FUN_004c3ac0();
        local_e8 = (float)(fVar6 * (float10)_DAT_005cc9bc);
        local_38 = local_38 - local_24 / _DAT_005cc574;
        local_34 = local_34 - local_20 / _DAT_005cc574;
        local_b0 = local_b0 - local_1c / _DAT_005cc574;
        local_b8 = ((float)param_1[0x267] + (float)param_1[0x267]) - _DAT_005cc320;
        if (local_b8 < DAT_005d757c) {
          local_b8 = 0.0;
        }
        local_dc = _DAT_005cc320 - local_b8;
        local_40 = local_38 * local_dc + local_40 * local_b8;
        local_3c = local_34 * local_dc + local_3c * local_b8;
        local_c0 = local_b0 * local_dc + local_c0 * local_b8;
        local_d8 = local_e8 * local_dc + local_d8 * local_b8;
      }
      else {
        param_1[0x265] = local_2c;
        param_1[0x266] = local_68;
      }
    }
    else {
      param_1[0x267] = 0;
    }
    fVar6 = (float10)FUN_00408a50();
    local_b4 = (float)fVar6;
    local_58 = FUN_004a2c48();
    FUN_00441820(local_58,&local_24,&local_18);
    local_110 = local_58 + 1;
    iVar2 = FUN_00426bb0();
    if (iVar2 <= local_110) {
      local_110 = 0;
    }
    FUN_00441820(local_110,&local_54,&local_60);
    local_b4 = local_b4 - (float)(int)local_58;
    local_b8 = _DAT_005cc320 - local_b4;
    local_10c = local_54 * local_b4 + local_24 * local_b8;
    local_108 = local_50 * local_b4 + local_20 * local_b8;
    local_104 = local_4c * local_b4 + local_1c * local_b8;
    local_bc = local_60 * local_b4 + local_18 * local_b8;
    fVar6 = (float10)FUN_00408a50();
    local_b4 = (float)fVar6;
    local_58 = FUN_004a2c48();
    FUN_00441820(local_58,&local_24,&local_18);
    local_110 = local_58 + 1;
    iVar2 = FUN_00426bb0();
    if (iVar2 <= local_110) {
      local_110 = 0;
    }
    FUN_00441820(local_110,&local_54,&local_60);
    local_b4 = local_b4 - (float)(int)local_58;
    local_b8 = _DAT_005cc320 - local_b4;
    local_cc = local_54 * local_b4 + local_24 * local_b8;
    local_c8 = local_50 * local_b4 + local_20 * local_b8;
    local_c4 = local_4c * local_b4 + local_1c * local_b8;
    local_100 = local_60 * local_b4 + local_18 * local_b8;
    local_bc = (local_bc + local_100) * _DAT_005cc32c;
    local_24 = local_10c + local_cc;
    local_20 = local_108 + local_c8;
    local_1c = local_104 + local_c4;
    FUN_004c39b0(&local_24,&local_24);
    local_44 = 0;
    local_ec = 0;
    for (local_8 = 0; local_8 < 4; local_8 = local_8 + 1) {
      iVar2 = FUN_0040e370();
      if ((iVar2 != 0) && (iVar2 = FUN_0046c7b0(), iVar2 == 1)) {
        local_44 = local_44 + 1;
        FUN_0046cbb0(local_8,&local_e0,&local_5c);
        if (local_e0 != 0) {
          local_ec = local_ec + 1;
        }
      }
    }
    if ((local_44 == 1) && (local_ec == 1)) {
      local_b8 = (float)local_5c / _DAT_005cc9fc + local_bc;
      if (_DAT_005cc55c < local_b8) {
        local_b8 = 10.0;
      }
      local_bc = local_b8;
    }
    local_bc = local_bc * _DAT_005ccabc;
    if (DAT_007f0fd0 == 8) {
      local_bc = 4.0;
    }
    if (DAT_007f0fd0 == 4) {
      local_bc = 4.0;
    }
    fVar7 = local_bc;
    local_b8 = local_bc;
    if (local_bc < local_d8) {
      local_bc = local_d8;
    }
    if (_DAT_005cc55c < local_bc) {
      local_bc = 10.0;
    }
    if (DAT_007f0f38 != 0) {
      local_bc = 10.0;
    }
    if (fVar7 < local_30) {
      local_b8 = local_30;
    }
    if (_DAT_005cc55c < local_b8) {
      local_b8 = 10.0;
    }
    param_1[0x268] = (int)local_b8;
    local_28 = _DAT_00614704 * local_1c + _DAT_00614700 * local_20 + _DAT_006146fc * local_24;
    if (local_28 < _DAT_005cc33c) {
      local_28 = -1.0;
    }
    if (_DAT_005cc320 < local_28) {
      local_28 = 1.0;
    }
    fVar6 = (float10)FUN_004a3384((double)local_28);
    fVar7 = (float)(fVar6 * (float10)_DAT_005ccae0);
    local_28 = (float)((float10)_DAT_005ccad0 -
                      ((float10)_DAT_005cd09c - fVar6 * (float10)_DAT_005ccae0));
    local_b8 = (float)_DAT_005ce1c0 - (local_bc * (float)_DAT_005ce1c8) / (float)_DAT_005cd030;
    local_24 = local_24 * local_b8;
    local_20 = local_20 * local_b8;
    local_1c = local_1c * local_b8;
    FUN_004c39b0(&local_54,&local_24,fVar7);
    local_10c = local_50 * _DAT_00614704 - local_4c * _DAT_00614700;
    local_108 = local_4c * _DAT_006146fc - local_54 * _DAT_00614704;
    local_104 = local_54 * _DAT_00614700 - local_50 * _DAT_006146fc;
    local_b8 = (_DAT_005cd120 * local_bc) / _DAT_005cc55c;
    iVar2 = FUN_00426c00();
    if (iVar2 == 0x1a) {
      local_b8 = (_DAT_005ce1b8 * local_bc) / _DAT_005cc55c;
      local_bc = local_bc * _DAT_005cd074;
    }
    local_b8 = local_b8 - _DAT_005cc358;
    if (_DAT_005ce1b4 <= local_b8 + local_28) {
      local_b8 = _DAT_005ce1b4 - local_28;
    }
    if (DAT_007f0f38 != 0) {
      local_b8 = 90.0;
    }
    FUN_004c4d20(local_ac,&local_10c,local_b8,0);
    FUN_004c3df0(&local_24,&local_24,1,local_ac);
    local_b8 = (local_bc / _DAT_005cc9bc) / _DAT_005cc358 + _DAT_005cc320;
    local_24 = local_24 * local_b8;
    local_1c = local_1c * local_b8;
    local_f8 = local_24 + local_40;
    local_f0 = local_1c + local_c0;
    local_f4 = local_20 * local_b8;
    if (DAT_007f0f38 != 0) {
      local_f8 = local_40;
      local_f0 = local_c0;
      local_f4 = _DAT_005cc9b0;
    }
    local_f4 = local_f4 + local_3c;
    local_20 = 0.0;
    FUN_004c39b0(&local_24,&local_24,fVar7);
    local_b8 = (_DAT_005cd088 * local_bc) / _DAT_005cc55c +
               ((_DAT_005cc55c - local_bc) * _DAT_005ce1b0) / _DAT_005cc55c;
    iVar2 = FUN_00426c00();
    if (iVar2 == 0x1a) {
      local_b8 = local_b8 - (_DAT_005ce1ac * local_bc) / _DAT_005cc55c;
    }
    if (DAT_007f0f38 == 0) {
      local_40 = local_40 + local_24 * local_b8;
      local_3c = local_3c + local_20 * local_b8;
      local_c0 = local_c0 + local_1c * local_b8;
      local_f8 = local_f8 + local_24 * local_b8;
      local_f4 = local_f4 + local_20 * local_b8;
      local_f0 = local_f0 + local_1c * local_b8;
    }
    local_d4 = 0;
    local_24 = local_f8 - (float)param_1[0x259];
    local_20 = local_f4 - (float)param_1[0x25a];
    local_1c = local_f0 - (float)param_1[0x25b];
    fVar6 = (float10)FUN_004c3ac0();
    local_b8 = (float)fVar6;
    if ((float10)_DAT_005cc358 < fVar6) {
      local_d4 = 1;
    }
    if ((param_2 != 0) || (local_d4 != 0)) {
      param_1[0x259] = (int)local_f8;
      param_1[0x25a] = (int)local_f4;
      param_1[0x25b] = (int)local_f0;
      param_1[0x25e] = 0;
      param_1[0x25d] = 0;
      param_1[0x25c] = 0;
      local_48 = local_40;
      param_1[0x25f] = (int)local_40;
      local_fc = local_3c;
      param_1[0x260] = (int)local_3c;
      local_e4 = local_c0;
      param_1[0x261] = (int)local_c0;
      param_1[0x264] = 0;
      param_1[0x263] = 0;
      param_1[0x262] = 0;
      param_1[0x265] = local_2c;
      param_1[0x266] = local_68;
      param_1[0x267] = 0;
    }
    fVar7 = (local_f4 - (float)param_1[0x25a]) * _DAT_005cc9a0;
    fVar1 = (local_f0 - (float)param_1[0x25b]) * _DAT_005cc9a0;
    param_1[0x25c] =
         (int)((float)param_1[0x25c] + (local_f8 - (float)param_1[0x259]) * _DAT_005cc9a0);
    param_1[0x25d] = (int)((float)param_1[0x25d] + fVar7);
    param_1[0x25e] = (int)((float)param_1[0x25e] + fVar1);
    param_1[0x25c] = (int)((float)param_1[0x25c] * _DAT_005cc9bc);
    param_1[0x25d] = (int)((float)param_1[0x25d] * _DAT_005cc9bc);
    param_1[0x25e] = (int)((float)param_1[0x25e] * _DAT_005cc9bc);
    local_f8 = local_f8 -
               ((_DAT_005cc55c - local_bc) / _DAT_005cc55c) *
               (local_f8 - ((float)param_1[0x259] + (float)param_1[0x25c]));
    local_f4 = local_f4 -
               ((_DAT_005cc55c - local_bc) / _DAT_005cc55c) *
               (local_f4 - ((float)param_1[0x25a] + (float)param_1[0x25d]));
    local_f0 = local_f0 -
               ((_DAT_005cc55c - local_bc) / _DAT_005cc55c) *
               (local_f0 - ((float)param_1[0x25b] + (float)param_1[0x25e]));
    fVar7 = (local_3c - (float)param_1[0x260]) * _DAT_005cc9a0;
    fVar1 = (local_c0 - (float)param_1[0x261]) * _DAT_005cc9a0;
    param_1[0x262] =
         (int)((float)param_1[0x262] + (local_40 - (float)param_1[0x25f]) * _DAT_005cc9a0);
    param_1[0x263] = (int)((float)param_1[0x263] + fVar7);
    param_1[0x264] = (int)((float)param_1[0x264] + fVar1);
    param_1[0x262] = (int)((float)param_1[0x262] * _DAT_005cc9bc);
    param_1[0x263] = (int)((float)param_1[0x263] * _DAT_005cc9bc);
    param_1[0x264] = (int)((float)param_1[0x264] * _DAT_005cc9bc);
    local_24 = (float)param_1[0x25f] + (float)param_1[0x262];
    local_20 = (float)param_1[0x260] + (float)param_1[0x263];
    local_1c = (float)param_1[0x261] + (float)param_1[0x264];
    local_54 = ((_DAT_005cc55c - local_bc) / _DAT_005cc55c) * (local_40 - local_24);
    local_50 = ((_DAT_005cc55c - local_bc) / _DAT_005cc55c) * (local_3c - local_20);
    local_4c = ((_DAT_005cc55c - local_bc) / _DAT_005cc55c) * (local_c0 - local_1c);
    local_14 = local_40 - local_54;
    local_10 = local_3c - local_50;
    local_c = local_c0 - local_4c;
    if (DAT_007f0f38 == 0) {
      local_58 = DAT_007f1030 / 60000;
      local_dc = (local_bc + _DAT_005cc9c0) / _DAT_005cc358;
      local_b8 = (float)local_58 / _DAT_005ce040;
      fVar6 = (float10)FUN_004a3700();
      local_14 = local_14 + (float)((fVar6 / (float10)_DAT_005cc55c) * (float10)local_dc);
      local_b8 = (float)(int)local_58 / _DAT_005ce1a8;
      fVar6 = (float10)FUN_004a3700();
      local_c = local_c + (float)((fVar6 / (float10)_DAT_005cc55c) * (float10)local_dc);
      local_b8 = (float)(int)local_58 / _DAT_005ce044;
      fVar6 = (float10)FUN_004a3700();
      local_f8 = local_f8 + (float)((fVar6 / (float10)_DAT_005cc358) * (float10)local_dc);
      local_b8 = (float)(int)local_58 / _DAT_005cd9e8;
      fVar6 = (float10)FUN_004a3700();
      local_f4 = local_f4 + (float)((fVar6 / (float10)_DAT_005cc358) * (float10)local_dc);
      local_b8 = (float)(int)local_58 / _DAT_005ce1a4;
      fVar6 = (float10)FUN_004a3700();
      local_f0 = local_f0 + (float)((fVar6 / (float10)_DAT_005cc358) * (float10)local_dc);
      local_b8 = (float)(int)local_58 / _DAT_005ce1a0;
      fVar6 = (float10)FUN_004a3700();
      local_f8 = local_f8 + (float)((fVar6 / (float10)_DAT_005cc55c) * (float10)local_dc);
      local_b8 = (float)(int)local_58 / _DAT_005ce19c;
      fVar6 = (float10)FUN_004a3700();
      local_f0 = local_f0 + (float)((fVar6 / (float10)_DAT_005cc55c) * (float10)local_dc);
      local_b8 = (float)(int)local_58 / _DAT_005ce198;
      fVar6 = (float10)FUN_004a3700();
      local_f8 = local_f8 + (float)((fVar6 / (float10)_DAT_005ce194) * (float10)local_dc);
      local_b8 = (float)(int)local_58 / _DAT_005ce190;
      fVar6 = (float10)FUN_004a3700();
      local_b8 = (float)((fVar6 / (float10)_DAT_005ce194) * (float10)local_dc);
      local_f0 = local_f0 + local_b8;
    }
    fVar6 = (float10)FUN_00472650(-DAT_007f0fc8,DAT_007f0fc8);
    local_14 = (float)(fVar6 + (float10)local_14);
    fVar6 = (float10)FUN_00472650(-DAT_007f0fc8,DAT_007f0fc8);
    local_10 = (float)(fVar6 + (float10)local_10);
    fVar6 = (float10)FUN_00472650(-DAT_007f0fc8,DAT_007f0fc8);
    local_c = (float)(fVar6 + (float10)local_c);
    param_1[0x10] = (int)local_f8;
    param_1[0x11] = (int)local_f4;
    param_1[0x12] = (int)local_f0;
    param_1[0x259] = (int)local_f8;
    param_1[0x25a] = (int)local_f4;
    param_1[0x25b] = (int)local_f0;
    param_1[0x25f] = (int)local_14;
    param_1[0x260] = (int)local_10;
    param_1[0x261] = (int)local_c;
    local_40 = local_14 - (float)param_1[0x10];
    local_3c = local_10 - (float)param_1[0x11];
    local_c0 = local_c - (float)param_1[0x12];
    param_1[0x13] = (int)local_40;
    param_1[0x14] = (int)local_3c;
    param_1[0x15] = (int)local_c0;
    param_1[0x16] = 0x3f19999a;
    if (local_c0 == DAT_005d757c) {
      if (DAT_005d757c <= local_40) {
        local_b4 = 90.0;
      }
      else {
        local_b4 = 270.0;
      }
    }
    else {
      local_b4 = local_40 / local_c0;
      fVar6 = (float10)FUN_004a3620();
      local_b4 = _DAT_005cd09c - (float)-(fVar6 * (float10)_DAT_005ccae0);
      if (DAT_005d757c < local_c0) {
        local_b4 = _DAT_005cd09c + local_b4;
      }
    }
    param_1[0xe] = (int)local_b4;
    local_40 = local_40 * local_40;
    local_c0 = local_c0 * local_c0 + local_40;
    if (local_c0 != DAT_005d757c) {
      fVar6 = (float10)FUN_004c3b30();
      local_c0 = (float)fVar6;
    }
    if (local_c0 == DAT_005d757c) {
      if (DAT_005d757c <= local_3c) {
        local_b4 = 90.0;
      }
      else {
        local_b4 = 270.0;
      }
    }
    else {
      local_b4 = local_3c / local_c0;
      fVar6 = (float10)FUN_004a3620();
      local_b4 = _DAT_005cd09c - (float)-(fVar6 * (float10)_DAT_005ccae0);
      if (DAT_005d757c < local_c0) {
        local_b4 = _DAT_005cd09c + local_b4;
      }
    }
    param_1[0xd] = (int)(_DAT_005ccac4 - local_b4);
    param_1[0xf] = 0;
    if (DAT_007f0f38 != 0) {
      param_1[0xe] = 0;
    }
    FUN_00441760();
    param_1[1] = param_1[0x10];
    param_1[2] = param_1[0x11];
    param_1[3] = param_1[0x12];
    param_1[4] = (int)local_14;
    param_1[5] = (int)local_10;
    param_1[6] = (int)local_c;
    if (*param_1 != 0) {
      FUN_00442a20();
    }
  }
  else {
    FUN_004427c0();
  }
  return;
}

