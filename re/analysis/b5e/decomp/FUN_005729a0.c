
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `undefined4 FUN_005729a0(float *param_1, int *param_2, float *param_3,
   float *param_4)`. */

undefined4 FUN_005729a0(float *param_1,int *param_2,float *param_3,float *param_4)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  float fVar9;
  float fVar10;
  int iVar11;
  uint uVar12;
  int *piVar13;
  float *pfVar14;
  float *pfVar15;
  float *pfVar16;
  float10 fVar17;
  float10 fVar18;
  float fStack_250;
  float fStack_24c;
  float fStack_248;
  float fStack_244;
  float fStack_240;
  float fStack_23c;
  float fStack_238;
  float fStack_230;
  int *piStack_22c;
  float fStack_228;
  uint uStack_220;
  float fStack_21c;
  float fStack_218;
  int *piStack_214;
  float *pfStack_210;
  float fStack_20c;
  float fStack_208;
  float fStack_204;
  float fStack_200;
  float fStack_1fc;
  float fStack_1f8;
  float fStack_1f4;
  float fStack_1f0;
  float fStack_1ec;
  float fStack_1e8;
  float *pfStack_1e4;
  float fStack_1e0;
  float fStack_1dc;
  float fStack_1d8;
  float fStack_1d4;
  float fStack_1d0;
  float fStack_1cc;
  float fStack_1c8;
  float fStack_1c4;
  float fStack_1c0;
  float fStack_1bc;
  float fStack_1b8;
  float fStack_1b4;
  float fStack_1b0;
  float fStack_1ac;
  float fStack_1a8;
  float fStack_1a4;
  float fStack_1a0;
  float fStack_19c;
  float fStack_198;
  float fStack_194;
  float fStack_190;
  float fStack_18c;
  float fStack_188;
  float fStack_184;
  float fStack_180;
  float fStack_178;
  float fStack_174;
  float fStack_170;
  float fStack_16c;
  float fStack_168;
  float fStack_164;
  float fStack_160;
  float fStack_15c;
  float fStack_158;
  float fStack_154;
  float fStack_14c;
  float fStack_144;
  float fStack_140;
  float fStack_13c;
  float fStack_134;
  float fStack_12c;
  float fStack_120;
  undefined1 auStack_114 [4];
  undefined1 auStack_110 [4];
  undefined1 auStack_10c [12];
  float afStack_100 [64];

  fVar17 = (float10)(**(code **)(*(int *)(*(int *)(*param_2 + 8) + 0x5c) + 0x24))
                              (*(int *)(*param_2 + 8));
  fVar18 = (float10)*(float *)(*(int *)(*param_2 + 8) + 0x4c);
  fStack_218 = (float)(fVar18 + fVar18 + fVar17);
  fVar17 = (float10)(**(code **)(*(int *)(*(int *)(param_2[1] + 8) + 0x5c) + 0x24))
                              (*(int *)(param_2[1] + 8));
  fVar18 = (float10)*(float *)(*(int *)(param_2[1] + 8) + 0x4c);
  fStack_218 = (float)((fVar18 + fVar18 + fVar17 + (float10)fStack_218) * (float10)param_1[10]);
  fVar1 = param_1[9];
  fVar2 = *param_1;
  if (param_3 == (float *)0x0) {
    FUN_0055bae0(*(undefined4 *)(*param_2 + 8),*(undefined4 *)(*param_2 + 0x10),&fStack_1d0);
    piStack_22c = (int *)0x0;
    pfStack_210 = (float *)0x0;
    fStack_23c = 0.0;
    fStack_230 = *param_4;
    pfVar14 = (float *)0x0;
  }
  else {
    uVar12 = (uint)*(ushort *)((int *)*param_2 + 1);
    iVar11 = *(int *)*param_2;
    piStack_22c = *(int **)(*(int *)(iVar11 + 0x10) + 4 + uVar12 * 0xc);
    pfVar14 = (float *)(piStack_22c[1] * 0x20 + *(int *)(*(int *)(*piStack_22c + 0x10) + 8));
    fStack_23c = SQRT(pfVar14[6] * pfVar14[6] + pfVar14[5] * pfVar14[5] + pfVar14[4] * pfVar14[4]);
    pfStack_210 = pfVar14;
    FUN_00561280(iVar11,uVar12,&fStack_1d0);
  }
  if (param_4 == (float *)0x0) {
    FUN_0055bae0(*(undefined4 *)(param_2[1] + 8),*(undefined4 *)(param_2[1] + 0x10),&fStack_1e0);
    fStack_248 = *pfVar14;
    fStack_230 = *param_3;
    fStack_244 = pfVar14[1];
    piVar13 = (int *)0x0;
    piStack_214 = (int *)0x0;
    pfStack_1e4 = (float *)0x0;
    fStack_240 = pfVar14[2];
    fStack_228 = 0.0;
  }
  else {
    uVar12 = (uint)*(ushort *)((int *)param_2[1] + 1);
    iVar11 = *(int *)param_2[1];
    piVar13 = *(int **)(*(int *)(iVar11 + 0x10) + 4 + uVar12 * 0xc);
    pfVar15 = (float *)(piVar13[1] * 0x20 + *(int *)(*(int *)(*piVar13 + 0x10) + 8));
    fStack_228 = SQRT(pfVar15[6] * pfVar15[6] + pfVar15[5] * pfVar15[5] + pfVar15[4] * pfVar15[4]);
    piStack_214 = piVar13;
    pfStack_1e4 = pfVar15;
    FUN_00561280(iVar11,uVar12,&fStack_1e0);
    if (param_3 == (float *)0x0) {
      fStack_248 = -*pfVar15;
      fStack_244 = -pfVar15[1];
      fStack_240 = -pfVar15[2];
    }
    else {
      if (*param_3 < *param_4 == (*param_3 == *param_4)) {
        fStack_230 = *param_4;
      }
      else {
        fStack_230 = *param_3;
      }
      fStack_248 = *pfVar14 - *pfVar15;
      fStack_244 = pfVar14[1] - pfVar15[1];
      fStack_240 = pfVar14[2] - pfVar15[2];
    }
  }
  fStack_1c0 = fStack_1d0 - fStack_1e0;
  fStack_1bc = fStack_1cc - fStack_1dc;
  fStack_1b8 = fStack_1c8 - fStack_1d8;
  fVar3 = fStack_1d4 + fStack_1c4;
  fVar17 = (float10)FUN_005667c0(auStack_10c,&fStack_1c0);
  fStack_250 = 0.0;
  fVar4 = fStack_240 * fStack_240 + fStack_248 * fStack_248 + fStack_244 * fStack_244;
  fVar18 = (float10)fStack_218 + (float10)fStack_218;
  fStack_1f4 = (float)fVar18;
  fVar18 = fVar18 + (float10)fVar3;
  if (fVar18 < fVar17) {
    fVar3 = fStack_1b8 * fStack_240 + fStack_1c0 * fStack_248 + fStack_1bc * fStack_244;
    if (_DAT_005e4568 < fVar3) {
      return 0;
    }
    fVar17 = (fVar17 + fVar18) * (fVar17 - fVar18) * (float10)fVar4;
    if ((float10)fVar3 * (float10)fVar3 < fVar17) {
      return 0;
    }
    fStack_250 = (float)((-(float10)fVar3 - SQRT((float10)fVar3 * (float10)fVar3 - fVar17)) /
                        (float10)fVar4);
  }
  if (fStack_250 < fVar1 * fVar2) {
    fStack_250 = fVar1 * fVar2;
  }
  if (fStack_228 + fStack_23c <= (float)PTR_DAT_005ceabc) {
    fStack_238 = 3.4028235e+38;
  }
  else {
    fStack_238 = _DAT_005cd07c / (fStack_228 + fStack_23c);
  }
  if (fStack_230 <= fStack_250) {
    return 0;
  }
  uStack_220 = 0;
  fVar1 = *(float *)(*(int *)(param_2[1] + 8) + 0x4c) + *(float *)(*(int *)(*param_2 + 8) + 0x4c);
  fStack_198 = *(float *)(*(int *)(param_2[1] + 8) + 0x4c) -
               *(float *)(*(int *)(*param_2 + 8) + 0x4c);
  fStack_194 = fStack_1c4 * fStack_23c + fStack_1d4 * fStack_228 + SQRT(fVar4);
  do {
    iVar11 = *param_2;
    if (piStack_22c != (int *)0x0) {
      pfVar15 = *(float **)(iVar11 + 0x10);
      iVar8 = piStack_22c[1];
      piVar13 = *(int **)(*piStack_22c + 0x10);
      pfVar16 = (float *)(iVar8 * 0x10 + piVar13[4]);
      pfVar14 = (float *)(iVar8 * 0x40 + 0x30 + *piVar13);
      fStack_190 = *pfVar16;
      fStack_18c = pfVar16[1];
      fStack_188 = pfVar16[2];
      fStack_184 = pfVar16[3];
      fStack_180 = *pfVar14;
      fVar2 = pfVar14[1];
      fStack_178 = pfVar14[2];
      if (DAT_005d757c < fStack_250) {
        FUN_0056be80(piVar13,iVar8,fStack_250);
      }
      fStack_1d0 = *pfVar14;
      fStack_1cc = pfVar14[1];
      fStack_1c8 = pfVar14[2];
      fVar9 = _DAT_005cc574 /
              (pfVar16[3] * pfVar16[3] +
              pfVar16[2] * pfVar16[2] + pfVar16[1] * pfVar16[1] + *pfVar16 * *pfVar16);
      fVar10 = fVar9 * *pfVar16;
      fStack_134 = pfVar16[1] * fVar9;
      fVar9 = pfVar16[2] * fVar9;
      fVar3 = pfVar16[3];
      fVar4 = pfVar16[3];
      fVar5 = pfVar16[3];
      fStack_20c = fVar10 * *pfVar16;
      fStack_208 = fStack_134 * pfVar16[1];
      fStack_204 = fVar9 * pfVar16[2];
      fStack_120 = fVar9 * pfVar16[1];
      fVar6 = pfVar16[2];
      fVar7 = *pfVar16;
      pfVar15[0xc] = 0.0;
      pfVar15[0xd] = 0.0;
      pfVar15[0xe] = 0.0;
      pfVar15[3] = 4.2039e-45;
      *pfVar15 = _DAT_005cc320 - (fStack_204 + fStack_208);
      pfVar15[1] = fVar5 * fVar9 + fStack_134 * fVar7;
      pfVar15[2] = fVar10 * fVar6 - fVar4 * fStack_134;
      pfVar15[4] = fStack_134 * fVar7 - fVar5 * fVar9;
      pfVar15[5] = _DAT_005cc320 - (fStack_204 + fStack_20c);
      pfVar15[6] = fStack_120 + fVar3 * fVar10;
      pfVar15[8] = fVar10 * fVar6 + fVar4 * fStack_134;
      pfVar15[9] = fStack_120 - fVar3 * fVar10;
      pfVar15[10] = _DAT_005cc320 - (fStack_208 + fStack_20c);
      pfVar15[0xc] = *pfVar14;
      pfVar15[0xd] = pfVar14[1];
      pfVar15[0xe] = pfVar14[2];
      fStack_144 = -(float)piStack_22c[2];
      fStack_140 = -(float)piStack_22c[3];
      fStack_13c = -(float)piStack_22c[4];
      FUN_004c51a0(pfVar15,&fStack_144,1);
      iVar11 = *(int *)(iVar11 + 8);
      if ((*(byte *)(*(int *)(iVar11 + 0x5c) + 0x40) & 2) == 0) {
        FUN_004c52f0(pfVar15,iVar11,1);
      }
      *pfVar16 = fStack_190;
      pfVar16[1] = fStack_18c;
      pfVar16[2] = fStack_188;
      pfVar16[3] = fStack_184;
      *pfVar14 = fStack_180;
      pfVar14[1] = fVar2;
      pfVar14[2] = fStack_178;
      piVar13 = piStack_214;
    }
    iVar11 = param_2[1];
    if (piVar13 != (int *)0x0) {
      pfVar15 = *(float **)(iVar11 + 0x10);
      iVar8 = piVar13[1];
      piVar13 = *(int **)(*piVar13 + 0x10);
      pfVar16 = (float *)(iVar8 * 0x10 + piVar13[4]);
      pfVar14 = (float *)(iVar8 * 0x40 + 0x30 + *piVar13);
      fStack_1b4 = *pfVar16;
      fStack_1b0 = pfVar16[1];
      fStack_1ac = pfVar16[2];
      fStack_1a8 = pfVar16[3];
      fStack_1a4 = *pfVar14;
      fStack_1a0 = pfVar14[1];
      fStack_19c = pfVar14[2];
      if (DAT_005d757c < fStack_250) {
        FUN_0056be80(piVar13,iVar8,fStack_250);
      }
      piVar13 = piStack_214;
      fStack_1e0 = *pfVar14;
      fStack_1dc = pfVar14[1];
      fStack_1d8 = pfVar14[2];
      fStack_12c = _DAT_005cc574 /
                   (pfVar16[3] * pfVar16[3] +
                   pfVar16[2] * pfVar16[2] + pfVar16[1] * pfVar16[1] + *pfVar16 * *pfVar16);
      fVar4 = fStack_12c * *pfVar16;
      fStack_14c = pfVar16[1] * fStack_12c;
      fStack_12c = pfVar16[2] * fStack_12c;
      fStack_1f0 = pfVar16[3] * fVar4;
      fStack_1ec = pfVar16[3] * fStack_14c;
      fStack_1e8 = pfVar16[3] * fStack_12c;
      fStack_200 = fVar4 * *pfVar16;
      fStack_1fc = fStack_14c * pfVar16[1];
      fStack_1f8 = fStack_12c * pfVar16[2];
      fStack_12c = fStack_12c * pfVar16[1];
      fVar2 = pfVar16[2];
      fVar3 = *pfVar16;
      pfVar15[0xc] = 0.0;
      pfVar15[0xd] = 0.0;
      pfVar15[0xe] = 0.0;
      pfVar15[3] = 4.2039e-45;
      *pfVar15 = _DAT_005cc320 - (fStack_1f8 + fStack_1fc);
      pfVar15[1] = fStack_1e8 + fStack_14c * fVar3;
      pfVar15[2] = fVar4 * fVar2 - fStack_1ec;
      pfVar15[4] = fStack_14c * fVar3 - fStack_1e8;
      pfVar15[5] = _DAT_005cc320 - (fStack_1f8 + fStack_200);
      pfVar15[6] = fStack_12c + fStack_1f0;
      pfVar15[8] = fVar4 * fVar2 + fStack_1ec;
      pfVar15[9] = fStack_12c - fStack_1f0;
      pfVar15[10] = _DAT_005cc320 - (fStack_1fc + fStack_200);
      pfVar15[0xc] = *pfVar14;
      pfVar15[0xd] = pfVar14[1];
      pfVar15[0xe] = pfVar14[2];
      fStack_15c = -(float)piStack_214[2];
      fStack_158 = -(float)piStack_214[3];
      fStack_154 = -(float)piStack_214[4];
      FUN_004c51a0(pfVar15,&fStack_15c,1);
      iVar11 = *(int *)(iVar11 + 8);
      if ((*(byte *)(*(int *)(iVar11 + 0x5c) + 0x40) & 2) == 0) {
        FUN_004c52f0(pfVar15,iVar11,1);
      }
      *pfVar16 = fStack_1b4;
      pfVar16[1] = fStack_1b0;
      pfVar16[2] = fStack_1ac;
      pfVar16[3] = fStack_1a8;
      *pfVar14 = fStack_1a4;
      pfVar14[1] = fStack_1a0;
      pfVar14[2] = fStack_19c;
    }
    param_1[0xeb] = 0.0;
    param_1[0xee] = 0.0;
    iVar11 = FUN_00575880(param_1 + 0xdf,param_1,param_2,(fStack_230 - fStack_250) * fStack_194);
    if (iVar11 == 0) {
      return 0;
    }
    pfVar15 = (float *)param_1[0xea];
    fStack_21c = fVar1 + pfVar15[0x44];
    pfVar14 = pfVar15 + 0x40;
    uVar12 = FUN_00576880(pfVar15[0x14],*(undefined2 *)(pfVar15 + 0x15),pfVar15[0x34],
                          *(undefined2 *)(pfVar15 + 0x35),fStack_198,pfVar15[0x43],pfVar14,
                          &fStack_21c,auStack_110,auStack_114,pfVar15,afStack_100);
    fVar2 = fStack_21c - fVar1;
    fStack_24c = fStack_230;
    fStack_228 = 0.0;
    if (uVar12 != 0) {
      pfVar16 = afStack_100;
      do {
        FUN_005735f0(pfStack_210,&fStack_1d0,pfVar16,&fStack_174);
        FUN_005735f0(pfStack_1e4,&fStack_1e0,pfVar16,&fStack_168);
        fVar3 = pfVar15[0x42] * (fStack_16c - fStack_160) +
                *pfVar14 * (fStack_174 - fStack_168) + pfVar15[0x41] * (fStack_170 - fStack_164);
        if (fVar3 < DAT_005d757c) {
          fStack_21c = (((pfVar16[2] * pfVar15[0x22] +
                         pfVar16[1] * pfVar15[0x21] + pfVar15[0x20] * *pfVar16) - pfVar15[0x23]) /
                        (pfVar15[0x42] * pfVar15[0x22] +
                        pfVar15[0x41] * pfVar15[0x21] + pfVar15[0x20] * *pfVar14) -
                       ((pfVar15[2] * pfVar16[2] + *pfVar16 * *pfVar15 + pfVar16[1] * pfVar15[1]) -
                       pfVar15[3]) /
                       (pfVar15[2] * pfVar15[0x42] +
                       *pfVar14 * *pfVar15 + pfVar15[0x41] * pfVar15[1])) - fVar1;
          if (fStack_21c < fVar2) {
            fStack_21c = fVar2;
          }
          if (fStack_21c < fStack_218 != (fStack_21c == fStack_218)) goto LAB_00573592;
          fStack_21c = fStack_21c - fStack_1f4;
          fVar3 = -(fStack_21c / fVar3);
          if (fVar3 < fStack_24c) {
            fStack_24c = fVar3;
          }
        }
        fStack_228 = (float)((int)fStack_228 + 1);
        pfVar16 = pfVar16 + 4;
      } while ((uint)fStack_228 < uVar12);
    }
    if (fStack_24c < DAT_005d757c != (fStack_24c == DAT_005d757c)) break;
    if (fStack_238 < fStack_24c) {
      fStack_24c = fStack_238;
    }
    fStack_250 = fStack_24c + fStack_250;
    if (fStack_230 < fStack_250) {
      return 0;
    }
    uStack_220 = uStack_220 + 1;
  } while (uStack_220 < 5);
LAB_00573592:
  if ((param_3 != (float *)0x0) && (fStack_250 < *param_3)) {
    *param_3 = fStack_250;
  }
  if ((param_4 != (float *)0x0) && (fStack_250 < *param_4)) {
    *param_4 = fStack_250;
  }
  return 1;
}

