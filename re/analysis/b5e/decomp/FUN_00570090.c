
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_00570090(int param_1, int param_2, float param_3, float
   param_4)`. */

void FUN_00570090(int param_1,int param_2,float param_3,float param_4)

{
  float fVar1;
  float fVar2;
  uint uVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  float *pfVar13;
  int iVar14;
  float *pfVar15;
  undefined4 *puVar16;
  float *pfVar17;
  undefined4 *puVar18;
  float10 fVar19;
  float10 fVar20;
  float10 extraout_ST1;
  float10 extraout_ST1_00;
  float local_320;
  int local_31c;
  float local_318;
  float *local_304;
  float local_300;
  float local_2fc;
  float local_2f8;
  float local_2f4;
  float local_2f0;
  float *local_2ec;
  int local_2e8;
  int local_2e4;
  float local_2e0;
  float local_2dc;
  float local_2d8;
  float local_2d4;
  float local_2d0;
  float local_2cc;
  float local_2c8;
  float local_2c0;
  float local_2bc;
  float local_2b8;
  float local_2b0;
  float local_2ac;
  float local_2a8;
  float local_2a4;
  float *local_2a0;
  float local_29c [4];
  float local_28c;
  float local_288;
  float local_284;
  float local_27c;
  float local_278;
  float local_274;
  float local_26c;
  float local_268;
  float local_264;
  float local_25c;
  float local_258;
  float local_254;
  float local_250;
  float local_24c;
  float local_248;
  float local_244;
  float local_240;
  undefined4 local_23c;
  float local_230;
  float local_22c;
  float local_228;
  float local_224;
  float local_220;
  float local_21c;
  float local_218;
  float local_214;
  float local_210;
  float local_20c;
  float local_208;
  float local_204;
  float local_200;
  float local_1fc;
  float local_1f8;
  float local_1f4;
  float local_1f0;
  float local_1ec;
  float local_1e8;
  float local_1e4;
  float local_1e0;
  float local_1dc [6];
  float local_1c4;
  float local_1bc;
  float local_1b8;
  float local_1b4;
  float local_1ac;
  float local_1a8;
  float local_1a4;
  float local_19c;
  float local_198;
  float local_194;
  float local_16c;
  float local_168;
  float local_164;
  float local_15c;
  float local_158;
  float local_154;
  float local_150;
  float local_14c;
  float local_148;
  float local_144;
  float local_140;
  float local_13c;
  float local_11c;
  float local_118;
  float local_114;
  undefined4 local_110;
  float local_10c;
  float local_108;
  float local_104;
  float local_fc;
  float local_f8;
  float local_f4;
  float local_ec [4];
  float local_dc;
  undefined4 local_d8 [4];
  float local_c8;
  float local_c4;
  float local_c0;
  float local_a8;
  float local_a4;
  float local_a0;
  float local_98;
  float local_94;
  float local_90;
  float local_8c;
  undefined4 local_88;
  float local_84;
  float local_80;
  float local_7c;
  undefined4 local_78;
  undefined4 local_6c [4];
  float local_5c;
  float local_58;
  float local_54;
  float local_3c;
  float local_38;
  float local_34;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  undefined4 local_1c;
  undefined4 local_18;
  float local_14;
  float local_10;
  undefined4 local_c;

  param_3 = param_3 * param_4;
  local_31c = 0;
  iVar10 = FUN_0056fb90(param_2 + 0x50,*(int *)(param_2 + 0x4c) + 4,&local_2c0,&local_2ec,&local_19c
                       );
  local_2e4 = iVar10;
  local_2e8 = FUN_0056fb90(param_2 + 0x58,*(int *)(param_2 + 0x4c) + 0x20,&local_2b0,&local_2a0,
                           local_1dc);
  local_2dc = local_16c - *local_2ec;
  local_2d8 = local_168 - local_2ec[1];
  local_2d4 = local_164 - local_2ec[2];
  local_2d0 = local_16c - *local_2a0;
  local_2cc = local_168 - local_2a0[1];
  local_2c8 = local_164 - local_2a0[2];
  local_1e8 = local_16c - local_1ac;
  local_1e4 = local_168 - local_1a8;
  local_1e0 = local_164 - local_1a4;
  *(undefined4 *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4) = 0;
  *(int *)(param_1 + 0xbc) = *(int *)(param_1 + 0xbc) + 1;
  if (iVar10 == 0) {
    iVar10 = -1;
  }
  else {
    iVar10 = (int)*(short *)(iVar10 + 0x20);
  }
  *(int *)(*(int *)(param_1 + 0xac) + *(int *)(param_1 + 0xf8) * 8) = iVar10;
  if (local_2e8 == 0) {
    iVar10 = -1;
  }
  else {
    iVar10 = (int)*(short *)(local_2e8 + 0x20);
  }
  *(int *)(*(int *)(param_1 + 0xac) + 4 + *(int *)(param_1 + 0xf8) * 8) = iVar10;
  FUN_0056fea0(&local_2c0,&local_2b0,&local_2fc,&local_218);
  local_2a0 = (float *)(local_2fc * local_2fc);
  local_228 = local_2f8 * local_2f8;
  local_dc = local_2f4 * local_2f4;
  local_2b0 = local_2f0 * local_2fc;
  local_2ac = local_2f0 * local_2f8;
  local_11c = _DAT_005cc320 - (local_dc + local_228 + local_dc + local_228);
  fVar1 = local_2f0 * local_2f4 + local_2f8 * local_2fc;
  local_118 = fVar1 + fVar1;
  fVar1 = local_2f4 * local_2fc - local_2ac;
  local_114 = fVar1 + fVar1;
  fVar1 = local_2f8 * local_2fc - local_2f0 * local_2f4;
  local_10c = fVar1 + fVar1;
  local_108 = _DAT_005cc320 - (local_dc + (float)local_2a0 + local_dc + (float)local_2a0);
  fVar1 = local_2b0 + local_2f4 * local_2f8;
  local_104 = fVar1 + fVar1;
  fVar1 = local_2ac + local_2f4 * local_2fc;
  local_fc = fVar1 + fVar1;
  fVar1 = local_2f4 * local_2f8 - local_2b0;
  local_f8 = fVar1 + fVar1;
  local_110 = 3;
  local_300 = 1.12104e-44;
  local_318 = 0.0;
  pfVar13 = local_1dc + 1;
  local_f4 = _DAT_005cc320 - (local_228 + (float)local_2a0 + local_228 + (float)local_2a0);
  local_ec[0] = local_1dc[2] * local_1e0 + local_1dc[0] * local_1e8 + local_1dc[1] * local_1e4;
  local_ec[1] = local_1c4 * local_1e0 + local_1dc[4] * local_1e8 + local_1dc[5] * local_1e4;
  local_ec[2] = local_1b4 * local_1e0 + local_1bc * local_1e8 + local_1b8 * local_1e4;
  fVar19 = (float10)_DAT_005ceac0;
  fVar20 = (float10)_DAT_005e5050;
  do {
    if ((*(uint *)(*(int *)(param_2 + 0x4c) + 0x3c) & (uint)local_300) != 0) {
      fVar1 = local_ec[(int)local_318];
      pfVar15 = (float *)&DAT_005e5738;
      pfVar17 = local_29c;
      for (iVar10 = 0x1b; iVar10 != 0; iVar10 = iVar10 + -1) {
        *pfVar17 = *pfVar15;
        pfVar15 = pfVar15 + 1;
        pfVar17 = pfVar17 + 1;
      }
      if (local_2e4 != 0) {
        local_29c[0] = -pfVar13[-1];
        local_29c[1] = -*pfVar13;
        local_29c[2] = -pfVar13[1];
        local_28c = *pfVar13 * local_2d4 - pfVar13[1] * local_2d8;
        local_288 = pfVar13[1] * local_2dc - pfVar13[-1] * local_2d4;
        local_284 = pfVar13[-1] * local_2d8 - *pfVar13 * local_2dc;
      }
      if (local_2e8 != 0) {
        pfVar15 = pfVar13 + -1;
        local_27c = *pfVar15;
        local_26c = pfVar13[1] * local_2cc - *pfVar13 * local_2c8;
        local_278 = *pfVar13;
        local_274 = pfVar13[1];
        local_268 = *pfVar15 * local_2c8 - pfVar13[1] * local_2d0;
        local_264 = *pfVar13 * local_2d0 - *pfVar15 * local_2cc;
      }
      local_240 = (float)fVar19;
      local_244 = (float)fVar20;
      local_254 = 0.0;
      local_250 = 0.0;
      local_248 = (float)fVar19;
      local_23c = 0x3f4ccccd;
      local_24c = (float)fVar20;
      local_25c = -fVar1;
      fVar20 = (float10)FUN_0056f1f0(param_1,local_29c);
      local_31c = local_31c + 1;
      fVar19 = extraout_ST1;
    }
    local_318 = (float)((int)local_318 + 1);
    pfVar13 = pfVar13 + 4;
    local_300 = (float)((int)local_300 * 2);
  } while ((uint)local_318 < 3);
  if ((*(byte *)(*(int *)(param_2 + 0x4c) + 0x3c) & 1) != 0) {
    pfVar13 = (float *)&DAT_005e5738;
    pfVar15 = local_29c;
    for (iVar10 = 0x1b; iVar10 != 0; iVar10 = iVar10 + -1) {
      *pfVar15 = *pfVar13;
      pfVar13 = pfVar13 + 1;
      pfVar15 = pfVar15 + 1;
    }
    if (local_2e4 != 0) {
      local_28c = local_20c;
      local_288 = local_208;
      local_284 = local_204;
    }
    if (local_2e8 != 0) {
      local_26c = -local_20c;
      local_268 = -local_208;
      local_264 = -local_204;
    }
    local_240 = (float)fVar19;
    local_244 = (float)fVar20;
    local_254 = 0.0;
    local_250 = 0.0;
    local_248 = (float)fVar19;
    local_23c = 0x3f4ccccd;
    local_24c = (float)fVar20;
    local_25c = -local_2fc;
    fVar20 = (float10)FUN_0056f1f0(param_1,local_29c);
    local_31c = local_31c + 1;
    fVar19 = extraout_ST1_00;
  }
  uVar11 = *(uint *)(*(int *)(param_2 + 0x4c) + 0x3c) & 6;
  if (uVar11 == 2) {
    fVar1 = local_198 * local_1c4;
    fVar2 = local_194 * local_1dc[5];
    pfVar13 = (float *)&DAT_005e5738;
    pfVar15 = local_29c;
    for (iVar10 = 0x1b; iVar10 != 0; iVar10 = iVar10 + -1) {
      *pfVar15 = *pfVar13;
      pfVar13 = pfVar13 + 1;
      pfVar15 = pfVar15 + 1;
    }
    fVar1 = fVar1 - fVar2;
    local_25c = local_10c;
    fVar2 = local_194 * local_1dc[4] - local_19c * local_1c4;
    fVar4 = local_19c * local_1dc[5] - local_198 * local_1dc[4];
    if (local_2e4 != 0) {
      local_28c = fVar1;
      local_288 = fVar2;
      local_284 = fVar4;
    }
    if (local_2e8 != 0) {
      local_26c = -fVar1;
      local_268 = -fVar2;
      local_264 = -fVar4;
    }
    local_240 = (float)fVar19;
    local_24c = -3.4028235e+38;
    local_254 = 0.0;
    local_244 = (float)fVar20;
    local_250 = 0.0;
    local_23c = 0x3f4ccccd;
    local_248 = (float)fVar19;
    FUN_0056f1f0(param_1,local_29c);
    local_31c = local_31c + 1;
  }
  else if (uVar11 == 4) {
    fVar1 = local_198 * local_1b4;
    fVar2 = local_194 * local_1b8;
    pfVar13 = (float *)&DAT_005e5738;
    pfVar15 = local_29c;
    for (iVar10 = 0x1b; iVar10 != 0; iVar10 = iVar10 + -1) {
      *pfVar15 = *pfVar13;
      pfVar13 = pfVar13 + 1;
      pfVar15 = pfVar15 + 1;
    }
    fVar1 = fVar1 - fVar2;
    local_25c = local_fc;
    fVar2 = local_194 * local_1bc - local_19c * local_1b4;
    fVar4 = local_19c * local_1b8 - local_198 * local_1bc;
    if (local_2e4 != 0) {
      local_28c = fVar1;
      local_288 = fVar2;
      local_284 = fVar4;
    }
    if (local_2e8 != 0) {
      local_26c = -fVar1;
      local_268 = -fVar2;
      local_264 = -fVar4;
    }
    local_240 = (float)fVar19;
    local_24c = -3.4028235e+38;
    local_254 = 0.0;
    local_244 = (float)fVar20;
    local_250 = 0.0;
    local_23c = 0x3f4ccccd;
    local_248 = (float)fVar19;
    FUN_0056f1f0(param_1,local_29c);
    local_31c = local_31c + 1;
  }
  else if (uVar11 == 6) {
    pfVar13 = (float *)&DAT_005e5738;
    pfVar15 = local_29c;
    for (iVar10 = 0x1b; iVar10 != 0; iVar10 = iVar10 + -1) {
      *pfVar15 = *pfVar13;
      pfVar13 = pfVar13 + 1;
      pfVar15 = pfVar15 + 1;
    }
    puVar16 = &DAT_005e5738;
    puVar18 = local_d8;
    for (iVar10 = 0x1b; iVar10 != 0; iVar10 = iVar10 + -1) {
      *puVar18 = *puVar16;
      puVar16 = puVar16 + 1;
      puVar18 = puVar18 + 1;
    }
    if (local_2e4 != 0) {
      local_28c = local_200;
      local_288 = local_1fc;
      local_284 = local_1f8;
      local_c8 = local_1f4;
      local_c4 = local_1f0;
      local_c0 = local_1ec;
    }
    if (local_2e8 != 0) {
      local_26c = -local_200;
      local_268 = -local_1fc;
      local_264 = -local_1f8;
      local_a8 = -local_1f4;
      local_a4 = -local_1f0;
      local_a0 = -local_1ec;
    }
    local_240 = (float)fVar19;
    local_244 = (float)fVar20;
    local_88 = 0xff7fffff;
    local_254 = 0.0;
    local_248 = (float)fVar19;
    local_250 = 0.0;
    local_23c = 0x3f4ccccd;
    local_24c = (float)fVar20;
    local_90 = 0.0;
    local_8c = 0.0;
    local_7c = (float)fVar19;
    local_78 = 0x3f4ccccd;
    local_80 = (float)fVar20;
    local_84 = (float)fVar19;
    local_25c = -local_2f8;
    local_98 = -local_2f4;
    FUN_0056f1f0(param_1,local_29c);
    FUN_0056f1f0(param_1,local_d8);
    local_31c = local_31c + 2;
  }
  fVar19 = (float10)FUN_005667c0(&local_224,&local_1e8);
  iVar10 = *(int *)(param_2 + 0x4c);
  fVar1 = (float)fVar19;
  if (((*(byte *)(iVar10 + 0x40) & 0x38) == 0) ||
     (fVar2 = fVar1 - *(float *)(iVar10 + 0x44), fVar2 <= DAT_005d757c)) {
    if (((*(byte *)(iVar10 + 0x54) & 0x38) != 0) &&
       (fVar2 = fVar1 - *(float *)(iVar10 + 0x58), DAT_005d757c < fVar2)) {
      iVar14 = 1;
      goto LAB_00570b99;
    }
  }
  else {
    iVar14 = 2;
LAB_00570b99:
    pfVar13 = (float *)&DAT_005e5738;
    pfVar15 = local_29c;
    for (iVar12 = 0x1b; iVar12 != 0; iVar12 = iVar12 + -1) {
      *pfVar15 = *pfVar13;
      pfVar13 = pfVar13 + 1;
      pfVar15 = pfVar15 + 1;
    }
    local_2bc = local_224 * local_2d4 - local_21c * local_2dc;
    local_2b8 = local_220 * local_2dc - local_224 * local_2d8;
    local_2b0 = local_21c * local_2cc - local_220 * local_2c8;
    local_2ac = local_224 * local_2c8 - local_21c * local_2d0;
    local_2a8 = local_220 * local_2d0 - local_224 * local_2cc;
    if (local_2e4 != 0) {
      local_29c[0] = -local_224;
      local_29c[1] = -local_220;
      local_29c[2] = -local_21c;
      local_28c = -(local_21c * local_2d8 - local_220 * local_2d4);
      local_288 = -local_2bc;
      local_284 = -local_2b8;
    }
    if (local_2e8 != 0) {
      local_27c = local_224;
      local_278 = local_220;
      local_274 = local_21c;
      local_26c = local_2b0;
      local_268 = local_2ac;
      local_264 = local_2a8;
    }
    if ((((iVar14 != 2) || ((*(byte *)(iVar10 + 0x54) & 0x38) == 0)) ||
        (fVar1 - *(float *)(iVar10 + 0x58) <= DAT_005d757c)) ||
       (*(int *)(iVar10 + 0x68) == 0x7f7fffff)) {
      local_244 = 0.0;
    }
    else {
      local_244 = (*(float *)(iVar10 + 0x44) - *(float *)(iVar10 + 0x58)) *
                  *(float *)(iVar10 + 0x68);
    }
    local_240 = 3.4028235e+38;
    local_24c = 0.0;
    local_248 = 3.4028235e+38;
    local_25c = -fVar2;
    if (iVar14 == 1) {
      fVar1 = param_4 * *(float *)(iVar10 + 0x68) + *(float *)(iVar10 + 0x6c);
      if (fVar1 <= DAT_005d757c) goto LAB_00570e2d;
      fVar1 = _DAT_005cc320 / fVar1;
      if (param_3 * _DAT_005cc56c <= fVar1) {
        local_254 = fVar1 - param_3;
      }
      else {
        local_254 = param_3 * _DAT_005e57dc;
      }
      local_250 = fVar1 * param_4 * *(float *)(iVar10 + 0x68);
    }
    local_23c = 0x3f4ccccd;
    FUN_0056f1f0(param_1,local_29c);
    local_31c = local_31c + 1;
  }
LAB_00570e2d:
  pfVar13 = local_1dc + 1;
  local_304 = (float *)(param_2 + 0x7c);
  uVar11 = 8;
  local_318 = 0.0;
  do {
    if (((*(uint *)(*(int *)(param_2 + 0x4c) + 0x3c) & uVar11) == 0) &&
       (uVar3 = *(uint *)(*(int *)(param_2 + 0x4c) + 0x8c), (uVar3 & uVar11 * 0x101) != 0)) {
      pfVar15 = (float *)&DAT_005e5738;
      pfVar17 = local_29c;
      for (iVar10 = 0x1b; iVar10 != 0; iVar10 = iVar10 + -1) {
        *pfVar17 = *pfVar15;
        pfVar15 = pfVar15 + 1;
        pfVar17 = pfVar17 + 1;
      }
      if ((uVar11 & uVar3) != 0) {
        local_25c = local_304[-3] - local_ec[(int)local_318];
      }
      if ((uVar3 & uVar11 << 8) != 0) {
        local_258 = -*local_304;
      }
      if ((uVar3 & uVar11 << 0x10) == 0) {
        local_240 = 3.4028235e+38;
        local_244 = -3.4028235e+38;
      }
      else {
        local_240 = *(float *)(*(int *)(param_2 + 0x4c) + 0x98);
        local_244 = -*(float *)(*(int *)(param_2 + 0x4c) + 0x98);
      }
      if (local_2e4 != 0) {
        local_29c[0] = -pfVar13[-1];
        local_29c[1] = -*pfVar13;
        local_29c[2] = -pfVar13[1];
        local_28c = *pfVar13 * local_2d4 - pfVar13[1] * local_2d8;
        local_288 = pfVar13[1] * local_2dc - pfVar13[-1] * local_2d4;
        local_284 = pfVar13[-1] * local_2d8 - *pfVar13 * local_2dc;
      }
      if (local_2e8 != 0) {
        pfVar15 = pfVar13 + -1;
        local_27c = *pfVar15;
        local_26c = pfVar13[1] * local_2cc - *pfVar13 * local_2c8;
        local_278 = *pfVar13;
        local_274 = pfVar13[1];
        local_268 = *pfVar15 * local_2c8 - pfVar13[1] * local_2d0;
        local_264 = *pfVar13 * local_2d0 - *pfVar15 * local_2cc;
      }
      iVar10 = *(int *)(param_2 + 0x4c);
      fVar1 = param_4 * *(float *)(iVar10 + 0x90) + *(float *)(iVar10 + 0x94);
      if (DAT_005d757c < fVar1) {
        fVar1 = _DAT_005cc320 / fVar1;
        if (param_3 * _DAT_005cc56c <= fVar1) {
          local_254 = fVar1 - param_3;
        }
        else {
          local_254 = param_3 * _DAT_005e57dc;
        }
        local_23c = 0x3f4ccccd;
        local_250 = fVar1 * param_4 * *(float *)(iVar10 + 0x90);
        FUN_0056f1f0(param_1,local_29c);
        local_31c = local_31c + 1;
      }
    }
    local_318 = (float)((int)local_318 + 1);
    local_304 = local_304 + 1;
    pfVar13 = pfVar13 + 4;
    uVar11 = uVar11 * 2;
  } while ((uint)local_318 < 3);
  iVar10 = *(int *)(param_2 + 0x4c);
  uVar11 = *(uint *)(iVar10 + 0x8c);
  if ((uVar11 & 0x707) != 0) {
    local_2a4 = ((-(*(float *)(param_2 + 0x60) * *(float *)(param_2 + 0x8c)) -
                 *(float *)(param_2 + 100) * *(float *)(param_2 + 0x90)) -
                *(float *)(param_2 + 0x68) * *(float *)(param_2 + 0x94)) * _DAT_005cc32c;
    local_2b0 = ((*(float *)(param_2 + 0x8c) * *(float *)(param_2 + 0x6c) +
                 *(float *)(param_2 + 0x68) * *(float *)(param_2 + 0x90)) -
                *(float *)(param_2 + 100) * *(float *)(param_2 + 0x94)) * _DAT_005cc32c;
    local_2ac = (*(float *)(param_2 + 0x60) * *(float *)(param_2 + 0x94) +
                (*(float *)(param_2 + 0x90) * *(float *)(param_2 + 0x6c) -
                *(float *)(param_2 + 0x68) * *(float *)(param_2 + 0x8c))) * _DAT_005cc32c;
    local_2a8 = (*(float *)(param_2 + 0x94) * *(float *)(param_2 + 0x6c) +
                (*(float *)(param_2 + 100) * *(float *)(param_2 + 0x8c) -
                *(float *)(param_2 + 0x60) * *(float *)(param_2 + 0x90))) * _DAT_005cc32c;
    if (((*(uint *)(iVar10 + 0x3c) & 7) == 0) && ((uVar11 & 0x7000000) != 0)) {
      fVar1 = *(float *)(param_2 + 0x6c);
      local_2c0 = *(float *)(param_2 + 0x6c) * local_2fc +
                  *(float *)(param_2 + 0x60) * local_2f0 +
                  (*(float *)(param_2 + 0x68) * local_2f8 - *(float *)(param_2 + 100) * local_2f4);
      local_2bc = *(float *)(param_2 + 0x6c) * local_2f8 +
                  *(float *)(param_2 + 100) * local_2f0 +
                  (*(float *)(param_2 + 0x60) * local_2f4 - *(float *)(param_2 + 0x68) * local_2fc);
      local_2b8 = *(float *)(param_2 + 0x6c) * local_2f4 +
                  *(float *)(param_2 + 0x68) * local_2f0 +
                  (*(float *)(param_2 + 100) * local_2fc - *(float *)(param_2 + 0x60) * local_2f8);
      local_2d0 = local_2a4 * local_2fc +
                  local_2b0 * local_2f0 + (local_2a8 * local_2f8 - local_2ac * local_2f4);
      local_2cc = local_2a4 * local_2f8 +
                  local_2ac * local_2f0 + (local_2b0 * local_2f4 - local_2a8 * local_2fc);
      local_2c8 = local_2a4 * local_2f4 +
                  local_2a8 * local_2f0 + (local_2ac * local_2fc - local_2b0 * local_2f8);
      local_15c = (local_200 * *(float *)(param_2 + 0x68) +
                  local_218 * *(float *)(param_2 + 0x60) + local_20c * fVar1) -
                  local_1f4 * *(float *)(param_2 + 100);
      local_158 = (local_1fc * *(float *)(param_2 + 0x68) +
                  local_214 * *(float *)(param_2 + 0x60) + local_208 * fVar1) -
                  local_1f0 * *(float *)(param_2 + 100);
      local_154 = (local_1f8 * *(float *)(param_2 + 0x68) +
                  local_210 * *(float *)(param_2 + 0x60) + local_204 * fVar1) -
                  local_1ec * *(float *)(param_2 + 100);
      local_150 = local_1f4 * *(float *)(param_2 + 0x60) +
                  fVar1 * local_200 +
                  (local_218 * *(float *)(param_2 + 100) - local_20c * *(float *)(param_2 + 0x68));
      local_14c = local_1f0 * *(float *)(param_2 + 0x60) +
                  fVar1 * local_1fc +
                  (local_214 * *(float *)(param_2 + 100) - local_208 * *(float *)(param_2 + 0x68));
      local_148 = local_1ec * *(float *)(param_2 + 0x60) +
                  fVar1 * local_1f8 +
                  (local_210 * *(float *)(param_2 + 100) - local_204 * *(float *)(param_2 + 0x68));
      local_144 = fVar1 * local_1f4 +
                  ((local_218 * *(float *)(param_2 + 0x68) + local_20c * *(float *)(param_2 + 100))
                  - local_200 * *(float *)(param_2 + 0x60));
      local_140 = fVar1 * local_1f0 +
                  ((local_214 * *(float *)(param_2 + 0x68) + local_208 * *(float *)(param_2 + 100))
                  - local_1fc * *(float *)(param_2 + 0x60));
      fVar4 = local_204 * *(float *)(param_2 + 100);
      fVar5 = local_210 * *(float *)(param_2 + 0x68);
      fVar2 = *(float *)(param_2 + 0x60);
      pfVar13 = (float *)&DAT_005e5738;
      pfVar15 = local_29c;
      for (iVar14 = 0x1b; iVar14 != 0; iVar14 = iVar14 + -1) {
        *pfVar15 = *pfVar13;
        pfVar13 = pfVar13 + 1;
        pfVar15 = pfVar15 + 1;
      }
      local_13c = fVar1 * local_1ec + ((fVar5 + fVar4) - local_1f8 * fVar2);
      puVar16 = &DAT_005e5738;
      puVar18 = local_d8;
      for (iVar14 = 0x1b; iVar14 != 0; iVar14 = iVar14 + -1) {
        *puVar18 = *puVar16;
        puVar16 = puVar16 + 1;
        puVar18 = puVar18 + 1;
      }
      puVar16 = &DAT_005e5738;
      puVar18 = local_6c;
      for (iVar14 = 0x1b; iVar14 != 0; iVar14 = iVar14 + -1) {
        *puVar18 = *puVar16;
        puVar16 = puVar16 + 1;
        puVar18 = puVar18 + 1;
      }
      if ((uVar11 & 7) != 0) {
        local_25c = -local_2c0;
        local_98 = -local_2bc;
        local_2c = -local_2b8;
      }
      if ((uVar11 & 0x700) != 0) {
        local_258 = local_2d0;
        local_94 = local_2cc;
        local_28 = local_2c8;
      }
      if ((uVar11 & 0x70000) == 0) {
        local_14 = -3.4028235e+38;
        local_10 = 3.4028235e+38;
      }
      else {
        local_10 = *(float *)(iVar10 + 0xb0);
        local_14 = -*(float *)(iVar10 + 0xb0);
      }
      local_24c = 0.0;
      local_248 = 0.0;
      local_88 = 0;
      local_84 = 0.0;
      local_1c = 0;
      local_18 = 0;
      if (local_2e4 != 0) {
        local_28c = local_15c;
        local_288 = local_158;
        local_284 = local_154;
        local_c8 = local_150;
        local_c4 = local_14c;
        local_c0 = local_148;
        local_5c = local_144;
        local_58 = local_140;
        local_54 = local_13c;
      }
      if (local_2e8 != 0) {
        local_26c = -local_15c;
        local_268 = -local_158;
        local_264 = -local_154;
        local_a8 = -local_150;
        local_a4 = -local_14c;
        local_a0 = -local_148;
        local_3c = -local_144;
        local_38 = -local_140;
        local_34 = -local_13c;
      }
      fVar1 = param_4 * *(float *)(iVar10 + 0xa8) + *(float *)(iVar10 + 0xac);
      local_244 = local_14;
      local_240 = local_10;
      local_80 = local_14;
      local_7c = local_10;
      if (fVar1 <= DAT_005d757c) {
LAB_005721a5:
        local_248 = 0.0;
        local_24c = 0.0;
      }
      else {
        fVar1 = _DAT_005cc320 / fVar1;
        if (param_3 * _DAT_005cc56c <= fVar1) {
          local_254 = fVar1 - param_3;
        }
        else {
          local_254 = param_3 * _DAT_005e57dc;
        }
        local_23c = 0x3f4ccccd;
        local_78 = 0x3f4ccccd;
        local_250 = fVar1 * *(float *)(iVar10 + 0xa8) * param_4;
        local_c = 0x3f4ccccd;
        local_90 = local_254;
        local_8c = local_250;
        local_24 = local_254;
        local_20 = local_250;
        FUN_0056f1f0(param_1,local_29c);
        FUN_0056f1f0(param_1,local_d8);
        FUN_0056f1f0(param_1,local_6c);
        local_31c = local_31c + 3;
      }
    }
    else {
      if (((*(uint *)(iVar10 + 0x3c) & 1) == 0) && ((uVar11 & 0x101) != 0)) {
        fVar1 = local_11c + _DAT_005cc320;
        pfVar13 = (float *)&DAT_005e5738;
        pfVar15 = local_29c;
        for (iVar14 = 0x1b; iVar14 != 0; iVar14 = iVar14 + -1) {
          *pfVar15 = *pfVar13;
          pfVar13 = pfVar13 + 1;
          pfVar15 = pfVar15 + 1;
        }
        fVar1 = _DAT_005cc320 / SQRT(fVar1 * _DAT_005cc32c);
        fVar5 = _DAT_005cc320 /
                SQRT(*(float *)(param_2 + 0x60) * *(float *)(param_2 + 0x60) +
                     *(float *)(param_2 + 0x6c) * *(float *)(param_2 + 0x6c));
        fVar2 = fVar5 * fVar1;
        fVar4 = (local_2f0 * *(float *)(param_2 + 0x6c) - local_2fc * *(float *)(param_2 + 0x60)) *
                fVar2;
        if ((uVar11 & 1) != 0) {
          local_25c = (local_2fc * *(float *)(param_2 + 0x6c) +
                      local_2f0 * *(float *)(param_2 + 0x60)) * fVar2;
        }
        if ((uVar11 & 0x100) != 0) {
          local_258 = -((local_2b0 * *(float *)(param_2 + 0x6c) -
                        local_2a4 * *(float *)(param_2 + 0x60)) * fVar4 * fVar5);
        }
        if ((uVar11 & 0x10000) == 0) {
          local_244 = -3.4028235e+38;
          local_240 = 3.4028235e+38;
        }
        else {
          local_240 = *(float *)(iVar10 + 0xa4);
          local_244 = -*(float *)(iVar10 + 0xa4);
        }
        fVar4 = fVar4 * fVar1;
        fVar1 = local_2fc * fVar4;
        local_24c = 0.0;
        local_248 = 0.0;
        fVar2 = -(fVar4 * local_2f0);
        fVar5 = local_20c * fVar2 + local_218 * fVar1;
        fVar4 = local_208 * fVar2 + local_214 * fVar1;
        fVar1 = fVar2 * local_204 + fVar1 * local_210;
        if (local_2e4 != 0) {
          local_28c = fVar5;
          local_288 = fVar4;
          local_284 = fVar1;
        }
        if (local_2e8 != 0) {
          local_26c = -fVar5;
          local_268 = -fVar4;
          local_264 = -fVar1;
        }
        fVar1 = param_4 * *(float *)(iVar10 + 0x9c) + *(float *)(iVar10 + 0xa0);
        if (DAT_005d757c < fVar1) {
          fVar1 = _DAT_005cc320 / fVar1;
          if (param_3 * _DAT_005cc56c <= fVar1) {
            local_254 = fVar1 - param_3;
          }
          else {
            local_254 = param_3 * _DAT_005e57dc;
          }
          local_23c = 0x3f4ccccd;
          local_250 = fVar1 * *(float *)(iVar10 + 0x9c) * param_4;
          FUN_0056f1f0(param_1,local_29c);
          local_31c = local_31c + 1;
        }
      }
      iVar10 = *(int *)(param_2 + 0x4c);
      uVar11 = ~(*(uint *)(iVar10 + 0x3c) & 6);
      if ((uVar11 != 0) && ((*(uint *)(iVar10 + 0x8c) & 0x606) != 0)) {
        fVar1 = local_11c + _DAT_005cc320;
        local_2e0 = *(float *)(param_2 + 0x68);
        fVar2 = fVar1 + fVar1;
        fVar6 = (*(float *)(param_2 + 0x60) * *(float *)(param_2 + 0x60) +
                *(float *)(param_2 + 0x6c) * *(float *)(param_2 + 0x6c)) * _DAT_005cc35c;
        fVar4 = SQRT(fVar2);
        fVar7 = SQRT(fVar6);
        fVar5 = _DAT_005cc320 / (fVar4 + fVar1);
        fVar8 = fVar6 * _DAT_005cc32c;
        fVar9 = _DAT_005cc320 / (fVar7 + fVar8);
        local_22c = (_DAT_005cc320 + fVar4) /
                    ((fVar2 + fVar2 + (local_11c + _DAT_005cc31c) * fVar4) * fVar1);
        local_230 = (fVar7 + _DAT_005cc320) /
                    ((fVar6 + fVar6 + (fVar8 + _DAT_005cc574) * fVar7) * fVar8);
        fVar1 = *(float *)(param_2 + 100);
        fVar4 = local_2e0 * local_2e0 + fVar1 * fVar1;
        local_15c = _DAT_005cc320 - (fVar4 + fVar4);
        local_158 = *(float *)(param_2 + 0x6c) * local_2e0 + fVar1 * *(float *)(param_2 + 0x60);
        local_158 = local_158 + local_158;
        local_154 = local_2e0 * *(float *)(param_2 + 0x60) - *(float *)(param_2 + 0x6c) * fVar1;
        local_154 = local_154 + local_154;
        local_2b0 = local_154 * *(float *)(param_2 + 0x90) - local_158 * *(float *)(param_2 + 0x94);
        local_2ac = *(float *)(param_2 + 0x94) * local_15c - local_154 * *(float *)(param_2 + 0x8c);
        local_2a8 = local_158 * *(float *)(param_2 + 0x8c) - *(float *)(param_2 + 0x90) * local_15c;
        local_2c0 = local_198 * local_1dc[2] - local_194 * local_1dc[1];
        local_2bc = local_194 * local_1dc[0] - local_19c * local_1dc[2];
        local_2b8 = local_19c * local_1dc[1] - local_198 * local_1dc[0];
        local_2d0 = local_198 * local_1c4 - local_194 * local_1dc[5];
        local_2cc = local_194 * local_1dc[4] - local_19c * local_1c4;
        local_2c8 = local_19c * local_1dc[5] - local_198 * local_1dc[4];
        local_2dc = local_198 * local_1b4 - local_194 * local_1b8;
        local_2d8 = local_194 * local_1bc - local_19c * local_1b4;
        local_2d4 = local_19c * local_1b8 - local_198 * local_1bc;
        if (((uVar11 & 2) != 0) && (_DAT_005cc328 < fVar2)) {
          uVar3 = *(uint *)(iVar10 + 0x8c);
          pfVar13 = (float *)&DAT_005e5738;
          pfVar15 = local_29c;
          for (iVar14 = 0x1b; iVar14 != 0; iVar14 = iVar14 + -1) {
            *pfVar15 = *pfVar13;
            pfVar13 = pfVar13 + 1;
            pfVar15 = pfVar15 + 1;
          }
          if ((uVar3 & 2) != 0) {
            local_25c = fVar5 * local_10c - local_158 * fVar9;
          }
          if ((uVar3 & 0x200) != 0) {
            local_258 = local_2ac * fVar9 - local_2b0 * local_158 * local_230;
          }
          fVar1 = local_22c * local_10c;
          local_2d0 = local_2c0 * fVar1 + local_2d0 * fVar5;
          local_2cc = local_2bc * fVar1 + local_2cc * fVar5;
          local_2c8 = local_2b8 * fVar1 + local_2c8 * fVar5;
          if (local_2e4 != 0) {
            local_28c = local_2d0;
            local_288 = local_2cc;
            local_284 = local_2c8;
          }
          if (local_2e8 != 0) {
            local_26c = -local_2d0;
            local_268 = -local_2cc;
            local_264 = -local_2c8;
          }
          if ((uVar3 & 0x20000) == 0) {
            local_244 = -3.4028235e+38;
            local_240 = 3.4028235e+38;
          }
          else {
            local_240 = *(float *)(iVar10 + 0xb0);
            local_244 = -*(float *)(iVar10 + 0xb0);
          }
          local_24c = 0.0;
          local_248 = 0.0;
          fVar1 = param_4 * *(float *)(iVar10 + 0xa8) + *(float *)(iVar10 + 0xac);
          if (DAT_005d757c < fVar1) {
            fVar1 = _DAT_005cc320 / fVar1;
            if (param_3 * _DAT_005cc56c <= fVar1) {
              local_254 = fVar1 - param_3;
            }
            else {
              local_254 = param_3 * _DAT_005e57dc;
            }
            local_23c = 0x3f4ccccd;
            local_250 = fVar1 * *(float *)(iVar10 + 0xa8) * param_4;
            FUN_0056f1f0(param_1,local_29c);
            local_31c = local_31c + 1;
          }
        }
        if (((uVar11 & 4) != 0) && (_DAT_005cc328 < fVar2)) {
          pfVar13 = (float *)&DAT_005e5738;
          pfVar15 = local_29c;
          for (iVar10 = 0x1b; iVar10 != 0; iVar10 = iVar10 + -1) {
            *pfVar15 = *pfVar13;
            pfVar13 = pfVar13 + 1;
            pfVar15 = pfVar15 + 1;
          }
          iVar10 = *(int *)(param_2 + 0x4c);
          uVar11 = *(uint *)(iVar10 + 0x8c);
          if ((uVar11 & 4) != 0) {
            local_25c = fVar5 * local_fc - local_154 * fVar9;
          }
          if ((uVar11 & 0x400) != 0) {
            local_258 = local_2a8 * fVar9 - local_2b0 * local_154 * local_230;
          }
          fVar1 = local_22c * local_fc;
          local_2dc = local_2c0 * fVar1 + local_2dc * fVar5;
          local_2d8 = local_2bc * fVar1 + local_2d8 * fVar5;
          local_2d4 = local_2b8 * fVar1 + local_2d4 * fVar5;
          if (local_2e4 != 0) {
            local_28c = local_2dc;
            local_288 = local_2d8;
            local_284 = local_2d4;
          }
          if (local_2e8 != 0) {
            local_26c = -local_2dc;
            local_268 = -local_2d8;
            local_264 = -local_2d4;
          }
          if ((uVar11 & 0x40000) == 0) {
            local_244 = -3.4028235e+38;
            local_240 = 3.4028235e+38;
          }
          else {
            local_240 = *(float *)(iVar10 + 0xb0);
            local_244 = -*(float *)(iVar10 + 0xb0);
          }
          local_24c = 0.0;
          local_248 = 0.0;
          fVar1 = param_4 * *(float *)(iVar10 + 0xa8) + *(float *)(iVar10 + 0xac);
          if (fVar1 <= DAT_005d757c) goto LAB_005721a5;
          fVar1 = _DAT_005cc320 / fVar1;
          if (param_3 * _DAT_005cc56c <= fVar1) {
            local_254 = fVar1 - param_3;
          }
          else {
            local_254 = param_3 * _DAT_005e57dc;
          }
          local_23c = 0x3f4ccccd;
          local_250 = fVar1 * *(float *)(iVar10 + 0xa8) * param_4;
          FUN_0056f1f0(param_1,local_29c);
          local_31c = local_31c + 1;
        }
      }
    }
  }
  iVar10 = *(int *)(param_2 + 0x4c);
  fVar2 = *(float *)(iVar10 + 0x48) * *(float *)(iVar10 + 0x48);
  fVar1 = *(float *)(iVar10 + 0x5c) * *(float *)(iVar10 + 0x5c);
  local_2ec = (float *)(_DAT_005cc320 - fVar1);
  if (((*(byte *)(iVar10 + 0x40) & 1) == 0) ||
     (local_318 = local_2fc * fVar2 * local_2fc - (_DAT_005cc320 - fVar2) * local_2f0 * local_2f0,
     local_318 <= DAT_005d757c)) {
    if (((*(byte *)(iVar10 + 0x54) & 1) != 0) &&
       (local_318 = fVar1 * local_2fc * local_2fc - (float)local_2ec * local_2f0 * local_2f0,
       DAT_005d757c < local_318)) {
      iVar14 = 1;
      goto LAB_0057224e;
    }
  }
  else {
    iVar14 = 2;
    fVar1 = fVar2;
LAB_0057224e:
    fVar2 = (_DAT_005cc320 - fVar1) * local_2f0;
    pfVar13 = (float *)&DAT_005e5738;
    pfVar15 = local_29c;
    for (iVar12 = 0x1b; iVar12 != 0; iVar12 = iVar12 + -1) {
      *pfVar15 = *pfVar13;
      pfVar13 = pfVar13 + 1;
      pfVar15 = pfVar15 + 1;
    }
    fVar2 = fVar2 + fVar2;
    local_25c = local_318;
    fVar1 = fVar1 * local_2fc * _DAT_005cc34c;
    fVar5 = local_20c * fVar1 + local_218 * fVar2;
    fVar4 = local_208 * fVar1 + local_214 * fVar2;
    fVar1 = fVar1 * local_204 + fVar2 * local_210;
    if (local_2e4 != 0) {
      local_28c = fVar5;
      local_288 = fVar4;
      local_284 = fVar1;
    }
    if (local_2e8 != 0) {
      local_26c = -fVar5;
      local_268 = -fVar4;
      local_264 = -fVar1;
    }
    local_244 = -3.4028235e+38;
    local_240 = 0.0;
    if (iVar14 == 1) {
      fVar1 = param_4 * *(float *)(iVar10 + 0x74) + *(float *)(iVar10 + 0x78);
      if (fVar1 <= DAT_005d757c) goto LAB_0057240f;
      fVar1 = _DAT_005cc320 / fVar1;
      if (param_3 * _DAT_005cc56c <= fVar1) {
        local_254 = fVar1 - param_3;
      }
      else {
        local_254 = param_3 * _DAT_005e57dc;
      }
      local_24c = 0.0;
      local_250 = fVar1 * param_4 * *(float *)(iVar10 + 0x74);
    }
    else {
      local_24c = -3.4028235e+38;
    }
    local_248 = 0.0;
    local_23c = 0x3f4ccccd;
    FUN_0056f1f0(param_1,local_29c);
    local_31c = local_31c + 1;
  }
LAB_0057240f:
  iVar10 = *(int *)(param_2 + 0x4c);
  if (_DAT_005ceae4 <= *(float *)(iVar10 + 0x60)) {
    local_300 = 0.999999;
  }
  else {
    local_300 = *(float *)(iVar10 + 0x60);
  }
  fVar1 = _DAT_005ceae4;
  if (*(float *)(iVar10 + 100) < _DAT_005ceae4) {
    fVar1 = *(float *)(iVar10 + 100);
  }
  if (_DAT_005ceae4 <= *(float *)(iVar10 + 0x4c)) {
    local_320 = 0.999999;
  }
  else {
    local_320 = *(float *)(iVar10 + 0x4c);
  }
  fVar2 = _DAT_005ceae4;
  if (*(float *)(iVar10 + 0x50) < _DAT_005ceae4) {
    fVar2 = *(float *)(iVar10 + 0x50);
  }
  fVar4 = SQRT(local_2f0 * local_2f0 + (float)local_2a0);
  local_2e0 = SQRT(local_228 + local_dc);
  local_230 = (_DAT_005cc320 + fVar4) * fVar4;
  local_230 = local_230 + local_230;
  local_230 = local_230 * local_230;
  fVar5 = _DAT_005cc320 / (_DAT_005cc320 + fVar4);
  fVar4 = local_2e0 * fVar4 + local_2e0 * fVar4;
  local_228 = ((local_320 + _DAT_005cc320) * local_10c) / (_DAT_005cc320 - local_320);
  fVar6 = ((local_300 + _DAT_005cc320) * local_10c) / (_DAT_005cc320 - local_300);
  local_2a0 = (float *)(((_DAT_005cc320 + fVar2) * local_fc) / (_DAT_005cc320 - fVar2));
  pfVar13 = (float *)(((_DAT_005cc320 + fVar1) * local_fc) / (_DAT_005cc320 - fVar1));
  local_22c = local_228 * local_10c + (float)local_2a0 * local_fc;
  fVar1 = fVar6 * local_10c + (float)pfVar13 * local_fc;
  if (((*(byte *)(iVar10 + 0x40) & 6) == 0) || (local_22c <= local_230)) {
    local_2ec = pfVar13;
    if (((*(byte *)(iVar10 + 0x54) & 6) == 0) || (fVar1 <= local_230)) goto LAB_0057290f;
    local_2ec = (float *)(_DAT_005cc320 / fVar1);
    iVar14 = 1;
    fVar1 = SQRT((float)local_2ec);
    fVar2 = fVar5 * local_2e0 - fVar4 * fVar1;
    local_2a0 = local_2ec;
    local_230 = fVar6;
    local_228 = fVar1;
  }
  else {
    local_2ec = (float *)(_DAT_005cc320 / local_22c);
    iVar14 = 2;
    fVar1 = SQRT((float)local_2ec);
    fVar2 = fVar5 * local_2e0 - fVar4 * fVar1;
    pfVar13 = local_2a0;
    local_230 = local_228;
    local_22c = fVar1;
  }
  local_2ec = (float *)((float)local_2ec * fVar1 * fVar4);
  fVar4 = (fVar1 * local_11c - fVar5 * _DAT_005cc32c) / fVar4;
  fVar5 = (float)local_2ec * local_230;
  fVar1 = (float)local_2ec * (float)pfVar13;
  fVar6 = local_1bc * fVar1 + local_1dc[4] * fVar5 + local_1dc[0] * fVar4;
  fVar7 = local_1b8 * fVar1 + local_1dc[5] * fVar5 + fVar4 * local_1dc[1];
  fVar1 = local_1b4 * fVar1 + local_1c4 * fVar5 + fVar4 * local_1dc[2];
  local_2c0 = local_194 * fVar7 - local_198 * fVar1;
  local_2bc = local_19c * fVar1 - local_194 * fVar6;
  local_2b8 = local_198 * fVar6 - local_19c * fVar7;
  pfVar13 = (float *)&DAT_005e5738;
  pfVar15 = local_29c;
  for (iVar12 = 0x1b; iVar12 != 0; iVar12 = iVar12 + -1) {
    *pfVar15 = *pfVar13;
    pfVar13 = pfVar13 + 1;
    pfVar15 = pfVar15 + 1;
  }
  local_25c = -fVar2;
  if (local_2e4 != 0) {
    local_28c = local_2c0;
    local_288 = local_2bc;
    local_284 = local_2b8;
  }
  if (local_2e8 != 0) {
    local_26c = -local_2c0;
    local_268 = -local_2bc;
    local_264 = -local_2b8;
  }
  local_244 = 0.0;
  local_240 = 3.4028235e+38;
  if (iVar14 == 1) {
    fVar1 = param_4 * *(float *)(iVar10 + 0x80) + *(float *)(iVar10 + 0x84);
    if (fVar1 <= DAT_005d757c) goto LAB_0057290f;
    fVar1 = _DAT_005cc320 / fVar1;
    if (param_3 * _DAT_005cc56c <= fVar1) {
      local_254 = fVar1 - param_3;
    }
    else {
      local_254 = param_3 * _DAT_005e57dc;
    }
    local_248 = 0.0;
    local_250 = fVar1 * *(float *)(iVar10 + 0x80) * param_4;
  }
  else {
    local_248 = 3.4028235e+38;
  }
  local_24c = 0.0;
  local_23c = 0x3f4ccccd;
  FUN_0056f1f0(param_1,local_29c);
  local_31c = local_31c + 1;
LAB_0057290f:
  if (local_31c == 0) {
    pfVar13 = (float *)&DAT_005e5738;
    pfVar15 = local_29c;
    for (iVar10 = 0x1b; iVar10 != 0; iVar10 = iVar10 + -1) {
      *pfVar15 = *pfVar13;
      pfVar13 = pfVar13 + 1;
      pfVar15 = pfVar15 + 1;
    }
    local_244 = -3.4028235e+38;
    local_240 = 3.4028235e+38;
    local_24c = 0.0;
    local_248 = 0.0;
    local_23c = 0;
    FUN_0056f1f0(param_1,local_29c);
  }
  FUN_0056f0a0(param_1);
  return;
}

