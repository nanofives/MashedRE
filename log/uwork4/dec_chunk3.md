# DECOMP-bucket adjudication, chunk 3/4

## U-5025   row-fn 0x004cb490 (FUN_004cb490)

- STATEMENT: Name/purpose of the 16-dword block copied from DAT_007d4158 table not established; raw copy observed
- EVIDENCE MISSING (as written): Trace callers FUN_0049a2f0 and FUN_005422c0 to identify table semantics
- PATH (as written): decomp_function on callers

### BODY OF 0x005422c0 (`FUN_005422c0`)
signature: `undefined FUN_005422c0(void)`
callees: FUN_004c0ed0@0x004c0ed0, FUN_004c3b90@0x004c3b90, FUN_004c3d60@0x004c3d60, FUN_004c3dc0@0x004c3dc0, FUN_004c4dc0@0x004c4dc0, FUN_004cb490@0x004cb490, FUN_004f7dc0@0x004f7dc0, FUN_004f81b0@0x004f81b0
```c

/* [C1 2026-05-20] Very large two-arg renderer (int param_1, int param_2). Returns 1. */

undefined4 FUN_005422c0(int param_1,int param_2)

{
  char cVar1;
  ushort uVar2;
  ushort uVar3;
  int *piVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  int *piVar9;
  undefined4 *puVar10;
  uint uVar11;
  int iVar12;
  float *pfVar13;
  float unaff_EBX;
  float fVar14;
  float unaff_ESI;
  int iVar15;
  uint uVar16;
  float unaff_EDI;
  undefined4 *puVar17;
  uint uVar18;
  float fVar19;
  bool bVar20;
  float10 fVar21;
  float10 fVar22;
  float10 fVar23;
  float10 fVar24;
  int unaff_retaddr;
  uint uVar25;
  float fVar26;
  int iVar27;
  float fVar28;
  float local_1f0;
  float fStack_1ec;
  float fStack_1e8;
  float fStack_1e4;
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
  uint uStack_1b8;
  int iStack_1b4;
  float fStack_1b0;
  int iStack_1ac;
  undefined4 *puStack_1a8;
  float fStack_1a4;
  float fStack_1a0;
  float fStack_19c;
  float fStack_198;
  undefined4 *puStack_194;
  float local_190;
  float fStack_18c;
  float fStack_188;
  float fStack_184;
  int iStack_180;
  float fStack_17c;
  float fStack_178;
  float fStack_174;
  float local_170;
  float local_16c;
  float local_168;
  float fStack_164;
  float fStack_160;
  float fStack_15c;
  float fStack_158;
  float fStack_154;
  float fStack_150;
  float fStack_14c;
  float fStack_148;
  undefined4 uStack_144;
  float fStack_140;
  float fStack_13c;
  float fStack_138;
  float fStack_134;
  int local_130 [2];
  float fStack_128;
  float fStack_120;
  ushort auStack_11c [2];
  byte abStack_118 [8];
  undefined1 local_110 [144];
  undefined1 local_80 [64];
  undefined1 local_40 [36];
  int iStack_1c;
  int iStack_14;
  int iStack_8;
  float *pfStack_4;
  
  piVar4 = *(int **)(DAT_007dc5dc + *(int *)(param_2 + 8));
  local_190 = (float)piVar4[4] * (float)piVar4[3];
  iVar8 = *piVar4;
  if (iVar8 == 0) {
    iVar8 = *(int *)(*DAT_007d3ff8 + 4);
  }
  FUN_004cb490(0x100,local_40);
  FUN_004c4dc0(local_80,local_40);
  for (puVar10 = *(undefined4 **)(iVar8 + 0x90); puVar10 != (undefined4 *)(iVar8 + 0x90);
      puVar10 = (undefined4 *)*puVar10) {
    if (*(char *)(puVar10 + -2) == '\x03') {
      if ((puVar10 != (undefined4 *)&DAT_00000008) &&
         (((cVar1 = *(char *)((int)puVar10 + -7), cVar1 == '\x01' || (cVar1 == -0x7f)) ||
          (cVar1 == -0x7e)))) {
        iVar8 = FUN_004c0ed0(iVar8);
        local_170 = -*(float *)(iVar8 + 0x20);
        local_16c = -*(float *)(iVar8 + 0x24);
        local_168 = -*(float *)(iVar8 + 0x28);
        FUN_004c3dc0(local_130,&local_170,local_80);
        goto LAB_00542365;
      }
      break;
    }
  }
  iVar8 = FUN_004c0ed0(iVar8);
  FUN_004c3d60(local_130,iVar8 + 0x30,local_80);
LAB_00542365:
  (**(code **)(**(int **)(param_1 + 0x34) + 0x10))(*(int **)(param_1 + 0x34),local_110,&local_1f0);
  fVar26 = 0.0;
  do {
    piVar9 = (int *)(((int)fVar26 + 1) * 0x10 + param_1);
    local_130[(int)fVar26] = 0;
    piVar4 = (int *)*piVar9;
    if ((piVar4 != (int *)0x0) && ((*(ushort *)(piVar9 + 3) & 0xff6) != 0)) {
      (**(code **)(*piVar4 + 0x2c))
                (piVar4,(int)pfStack_4[1] * piVar9[2] + piVar9[1],(int)pfStack_4[6] * piVar9[2],
                 local_130 + (int)fVar26,0x800);
    }
    pfVar13 = pfStack_4;
    fVar26 = (float)((int)fVar26 + 1);
  } while ((int)fVar26 < 2);
  iVar8 = 0;
  while ((abStack_118[iVar8 * 8 + 2] != 0 || (abStack_118[iVar8 * 8 + 3] != 0))) {
    iVar8 = iVar8 + 1;
  }
  iStack_1ac = (uint)*(ushort *)(abStack_118 + iVar8 * 8 + -2) + local_130[auStack_11c[iVar8 * 4]];
  uStack_1b8 = (uint)abStack_118[iVar8 * 8];
  fStack_1d4 = *(float *)((uint)auStack_11c[iVar8 * 4] * 0x10 + 0x18 + param_1);
  for (iVar8 = 0;
      ((abStack_118[iVar8 * 8 + 2] != 3 || (abStack_118[iVar8 * 8 + 3] != 0)) &&
      (abStack_118[iVar8 * 8] != 0x11)); iVar8 = iVar8 + 1) {
  }
  if (abStack_118[iVar8 * 8] == 0x11) {
    fStack_1b0 = 0.0;
  }
  else {
    local_190 = (float)(uint)abStack_118[iVar8 * 8];
    fStack_1b0 = (float)((uint)*(ushort *)(abStack_118 + iVar8 * 8 + -2) +
                        local_130[auStack_11c[iVar8 * 4]]);
    fStack_198 = *(float *)((uint)auStack_11c[iVar8 * 4] * 0x10 + 0x18 + param_1);
  }
  fVar19 = fStack_1b0;
  for (iVar8 = 0;
      ((abStack_118[iVar8 * 8 + 2] != 10 || (abStack_118[iVar8 * 8 + 3] != 0)) &&
      (abStack_118[iVar8 * 8] != 0x11)); iVar8 = iVar8 + 1) {
  }
  if (abStack_118[iVar8 * 8] == 0x11) {
    local_170 = 0.0;
  }
  else {
    local_170 = (float)((uint)*(ushort *)(abStack_118 + iVar8 * 8 + -2) +
                       local_130[auStack_11c[iVar8 * 4]]);
    fStack_148 = *(float *)((uint)auStack_11c[iVar8 * 4] * 0x10 + 0x18 + param_1);
  }
  iVar8 = 0;
  while ((abStack_118[iVar8 * 8 + 2] != 5 || (abStack_118[iVar8 * 8 + 3] != 0))) {
    iVar8 = iVar8 + 1;
  }
  iVar12 = (uint)*(ushort *)(abStack_118 + iVar8 * 8 + -2) + local_130[auStack_11c[iVar8 * 4]];
  iStack_1b4 = *(int *)((uint)auStack_11c[iVar8 * 4] * 0x10 + 0x18 + param_1);
  fStack_1bc = (float)(uint)abStack_118[iVar8 * 8];
  for (iVar8 = 0;
      ((abStack_118[iVar8 * 8 + 2] != 6 || (abStack_118[iVar8 * 8 + 3] != 0)) &&
      (abStack_118[iVar8 * 8] != 0x11)); iVar8 = iVar8 + 1) {
  }
  iStack_180 = iVar12;
  if (abStack_118[iVar8 * 8] != 0x11) {
    puStack_194 = (undefined4 *)(uint)abStack_118[iVar8 * 8];
    iVar15 = (uint)*(ushort *)(abStack_118 + iVar8 * 8 + -2) + local_130[auStack_11c[iVar8 * 4]];
    iVar8 = *(int *)((uint)auStack_11c[iVar8 * 4] * 0x10 + 0x18 + param_1);
    if (iVar15 != 0) {
      iVar27 = 0;
      local_16c = pfStack_4[6];
      if (0 < (int)local_16c) {
        do {
          if (uStack_1b8 == 2) {
            pfVar13 = (float *)((int)fStack_1d4 * iVar27 + iStack_1ac);
            fStack_1e0 = *pfVar13;
            fStack_1dc = pfVar13[1];
            fStack_1d8 = pfVar13[2];
          }
          else {
            FUN_004f7dc0(uStack_1b8,&fStack_1e0,(int)fStack_1d4 * iVar27 + iStack_1ac);
          }
          if (fStack_1bc == 1.4013e-45) {
            fStack_1d0 = *(float *)(iStack_1b4 * iVar27 + iVar12);
            fStack_1cc = *(float *)(iStack_1b4 * iVar27 + 4 + iVar12);
          }
          else {
            FUN_004f81b0(fStack_1bc,&fStack_1d0,iStack_1b4 * iVar27 + iVar12);
          }
          if (fVar19 == 0.0) {
            unaff_EBX = 0.0;
            unaff_ESI = 0.0;
            local_1f0 = 0.0;
          }
          else if (local_190 == 2.8026e-45) {
            pfVar13 = (float *)((int)fStack_198 * iVar27 + (int)fVar19);
            unaff_EBX = *pfVar13;
            unaff_ESI = pfVar13[1];
            local_1f0 = pfVar13[2];
          }
          else {
            FUN_004f7dc0(local_190,&stack0xfffffe08,(int)fStack_198 * iVar27 + (int)fVar19);
          }
          if (puStack_194 == (undefined4 *)0x2) {
            pfVar13 = (float *)(iVar8 * iVar27 + iVar15);
            fStack_1ec = *pfVar13;
            fStack_1e8 = pfVar13[1];
            fStack_1e4 = pfVar13[2];
          }
          else {
            FUN_004f7dc0(puStack_194,&fStack_1ec,iVar8 * iVar27 + iVar15);
          }
          fStack_1c8 = fStack_13c - fStack_1e0;
          fStack_1c4 = fStack_138 - fStack_1dc;
          fStack_1c0 = fStack_134 - fStack_1d8;
          fVar21 = (float10)FUN_004c3b90(fStack_1c0 * fStack_1c0 +
                                         fStack_1c8 * fStack_1c8 + fStack_1c4 * fStack_1c4);
          fStack_120 = fStack_1ec * unaff_ESI - fStack_1e8 * unaff_EBX;
          fStack_1a4 = (float)((float10)fStack_1e4 * fVar21 * (float10)fStack_1c0 +
                              (float10)fStack_1ec * (float10)fStack_1c8 * fVar21 +
                              (float10)fStack_1e8 * fVar21 * (float10)fStack_1c4);
          pfVar13 = (float *)(unaff_retaddr + iVar27 * 0x2c);
          *pfVar13 = fStack_1e0;
          pfVar13[1] = fStack_1dc;
          pfVar13[2] = fStack_1d8;
          fStack_1a0 = (float)((float10)fStack_120 * fVar21 * (float10)fStack_1c0 +
                              ((float10)fStack_1e8 * (float10)local_1f0 -
                              (float10)fStack_1e4 * (float10)unaff_ESI) *
                              (float10)fStack_1c8 * fVar21 +
                              ((float10)fStack_1e4 * (float10)unaff_EBX -
                              (float10)fStack_1ec * (float10)local_1f0) *
                              fVar21 * (float10)fStack_1c4);
          if (fVar19 == 0.0) {
            fVar26 = 0.0;
            pfVar13[3] = 0.0;
            pfVar13[4] = 0.0;
          }
          else {
            pfVar13[3] = unaff_EBX;
            pfVar13[4] = unaff_ESI;
            fVar26 = local_1f0;
          }
          pfVar13[5] = fVar26;
          if (local_170 == 0.0) {
            pfVar13[6] = -NAN;
          }
          else {
            pfVar13[6] = *(float *)((int)fStack_148 * iVar27 + (int)local_170);
          }
          pfVar13[7] = fStack_1d0;
          pfVar13[8] = fStack_1cc;
          pfVar13[9] = fStack_1a4 * fStack_19c + fStack_1d0;
          pfVar13[10] = fStack_1a0 * fStack_19c + fStack_1cc;
          iVar27 = iVar27 + 1;
        } while (iVar27 < (int)local_16c);
      }
      goto LAB_005434b7;
    }
  }
  if (*(int *)(param_1 + 8) == 0) {
    fVar28 = 0.0;
    fVar14 = pfStack_4[6];
    fVar26 = fVar14;
    fVar19 = fVar14;
    fStack_1a4 = fVar14;
    if (0 < (int)fVar14) {
      do {
        if (*(int *)(iStack_8 + 0xc) == 4) {
          iVar8 = ((int)fVar28 / 3) * 3;
          fVar26 = (float)(((int)fVar28 + 1) % 3 + iVar8);
          fVar19 = (float)(((int)fVar28 + 2) % 3 + iVar8);
          fVar14 = fVar28;
          iVar12 = iStack_180;
        }
        else if (*(int *)(iStack_8 + 0xc) == 5) {
          if ((int)fVar28 < 2) {
            uVar11 = (uint)fVar28 & 0x80000001;
            bVar20 = uVar11 == 0;
            if ((int)uVar11 < 0) {
              bVar20 = (uVar11 - 1 | 0xfffffffe) == 0xffffffff;
            }
            if (bVar20) {
              fVar26 = (float)((int)fVar28 + 1);
              fVar19 = (float)((int)fVar28 + 2);
              fVar14 = fVar28;
            }
            else {
              fVar26 = (float)((int)fVar28 + 2);
              fVar19 = (float)((int)fVar28 + 1);
              fVar14 = fVar28;
            }
          }
          else {
            uVar11 = (uint)fVar28 & 0x80000001;
            bVar20 = uVar11 == 0;
            if ((int)uVar11 < 0) {
              bVar20 = (uVar11 - 1 | 0xfffffffe) == 0xffffffff;
            }
            if (bVar20) {
              fVar26 = (float)((int)fVar28 + -2);
              fVar19 = (float)((int)fVar28 + -1);
              fVar14 = fVar28;
            }
            else {
              fVar26 = (float)((int)fVar28 + -1);
              fVar19 = (float)((int)fVar28 + -2);
              fVar14 = fVar28;
            }
          }
        }
        if (uStack_1b8 == 2) {
          pfVar13 = (float *)((int)fVar14 * (int)fStack_1d4 + iStack_1ac);
          fStack_1e0 = *pfVar13;
          fStack_1dc = pfVar13[1];
          fStack_1d8 = pfVar13[2];
          pfVar13 = (float *)((int)fVar26 * (int)fStack_1d4 + iStack_1ac);
          local_168 = *pfVar13;
          fStack_164 = pfVar13[1];
          fStack_160 = pfVar13[2];
          pfVar13 = (float *)((int)fVar19 * (int)fStack_1d4 + iStack_1ac);
          fStack_15c = *pfVar13;
          fStack_158 = pfVar13[1];
          fStack_154 = pfVar13[2];
        }
        else {
          FUN_004f7dc0(uStack_1b8,&fStack_1e0,(int)fVar14 * (int)fStack_1d4 + iStack_1ac);
          FUN_004f7dc0(uStack_1b8,&local_168,(int)fVar26 * (int)fStack_1d4 + iStack_1ac);
          FUN_004f7dc0(uStack_1b8,&fStack_15c,(int)fVar19 * (int)fStack_1d4 + iStack_1ac);
        }
        if (fStack_1bc == 1.4013e-45) {
          fStack_1d0 = *(float *)((int)fVar14 * iStack_1b4 + iVar12);
          fStack_1cc = *(float *)((int)fVar14 * iStack_1b4 + 4 + iVar12);
          fStack_150 = *(float *)((int)fVar26 * iStack_1b4 + iVar12);
          fStack_14c = *(float *)((int)fVar26 * iStack_1b4 + 4 + iVar12);
          uStack_144 = *(undefined4 *)((int)fVar19 * iStack_1b4 + iVar12);
          fStack_140 = *(float *)((int)fVar19 * iStack_1b4 + 4 + iVar12);
        }
        else {
          FUN_004f81b0(fStack_1bc,&fStack_1d0,(int)fVar14 * iStack_1b4 + iVar12);
          FUN_004f81b0(fStack_1bc,&fStack_150,(int)fVar26 * iStack_1b4 + iVar12);
          FUN_004f81b0(fStack_1bc,&uStack_144,(int)fVar19 * iStack_1b4 + iVar12);
        }
        if (fStack_1b0 == 0.0) {
          fStack_1ec = 0.0;
          fStack_1e8 = 0.0;
          fStack_1e4 = 0.0;
        }
        else if (local_190 == 2.8026e-45) {
          pfVar13 = (float *)((int)fVar14 * (int)fStack_198 + (int)fStack_1b0);
          fStack_1ec = *pfVar13;
          fStack_1e8 = pfVar13[1];
          fStack_1e4 = pfVar13[2];
        }
        else {
          FUN_004f7dc0(local_190,&fStack_1ec,(int)fVar14 * (int)fStack_198 + (int)fStack_1b0);
        }
        fVar5 = fStack_13c - fStack_1e0;
        fVar6 = fStack_138 - fStack_1dc;
        local_1f0 = fStack_134 - fStack_1d8;
        fVar21 = (float10)FUN_004c3b90(local_1f0 * local_1f0 + fVar5 * fVar5 + fVar6 * fVar6);
        local_1f0 = (float)(fVar21 * (float10)local_1f0);
        fStack_120 = fStack_160 - fStack_1d8;
        fStack_18c = fStack_15c - fStack_1e0;
        puStack_1a8 = (undefined4 *)(fStack_140 - fStack_1cc);
        fStack_128 = (local_168 - fStack_1e0) * (float)puStack_1a8;
        fVar7 = fStack_14c - fStack_1cc;
        fStack_188 = (fStack_158 - fStack_1dc) * fVar7;
        fStack_184 = (fStack_154 - fStack_1d8) * fVar7;
        fStack_1c8 = fStack_128 - fStack_18c * fVar7;
        fStack_1c4 = (fStack_164 - fStack_1dc) * (float)puStack_1a8 - fStack_188;
        fStack_1c0 = fStack_120 * (float)puStack_1a8 - fStack_184;
        fVar24 = (float10)FUN_004c3b90(fStack_1c0 * fStack_1c0 +
                                       fStack_1c8 * fStack_1c8 + fStack_1c4 * fStack_1c4);
        fVar23 = (float10)fStack_1c8 * fVar24;
        fVar22 = (float10)fStack_1c4 * fVar24;
        fVar24 = (float10)fStack_1c0 * fVar24;
        fStack_17c = (float)(fVar22 * (float10)fStack_1e4 - fVar24 * (float10)fStack_1e8);
        fStack_178 = (float)((float10)fStack_1ec * fVar24 - fVar23 * (float10)fStack_1e4);
        fStack_174 = (float)(fVar23 * (float10)fStack_1e8 - fVar22 * (float10)fStack_1ec);
        pfVar13 = (float *)(unaff_retaddr + (int)fVar14 * 0x2c);
        *pfVar13 = fStack_1e0;
        pfVar13[1] = fStack_1dc;
        pfVar13[2] = fStack_1d8;
        if (fStack_1b0 == 0.0) {
          pfVar13[3] = 0.0;
          pfVar13[4] = 0.0;
          pfVar13[5] = 0.0;
        }
        else {
          pfVar13[3] = fStack_1ec;
          pfVar13[4] = fStack_1e8;
          pfVar13[5] = fStack_1e4;
        }
        if (local_170 == 0.0) {
          pfVar13[6] = -NAN;
        }
        else {
          pfVar13[6] = *(float *)((int)fVar14 * (int)fStack_148 + (int)local_170);
        }
        pfVar13[7] = fStack_1d0;
        pfVar13[8] = fStack_1cc;
        pfVar13[9] = (float)((float10)fStack_1d0 -
                            (fVar24 * (float10)local_1f0 +
                            fVar23 * (float10)(float)((float10)fVar5 * fVar21) +
                            fVar22 * (float10)(float)((float10)fVar6 * fVar21)) *
                            (float10)fStack_19c);
        pfVar13[10] = fStack_1cc -
                      (fStack_174 * local_1f0 +
                      fStack_17c * (float)((float10)fVar5 * fVar21) +
                      fStack_178 * (float)((float10)fVar6 * fVar21)) * fStack_19c;
        fVar28 = (float)((int)fVar28 + 1);
      } while ((int)fVar28 < (int)fStack_1a4);
    }
  }
  else {
    puVar10 = (undefined4 *)(*(code *)DAT_007d3ff8[0x42])(pfStack_4[6],0x30120);
    fVar19 = pfVar13[6];
    puVar17 = puVar10;
    for (uVar11 = (uint)fVar19 >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
      *puVar17 = 0;
      puVar17 = puVar17 + 1;
    }
    for (uVar11 = (uint)fVar19 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
      *(undefined1 *)puVar17 = 0;
      puVar17 = (undefined4 *)((int)puVar17 + 1);
    }
    fVar19 = *pfVar13;
    fStack_1a4 = fVar19;
    puStack_194 = puVar10;
    (**(code **)(**(int **)(param_1 + 8) + 0x2c))
              (*(int **)(param_1 + 8),(int)pfStack_4[7] * 2,(int)fVar19 * 2,&local_16c,0x800);
    uVar25 = 0;
    iVar8 = iStack_180;
    uVar11 = uStack_1b8;
    uVar16 = uStack_1b8;
    uVar18 = uStack_1b8;
    if (0 < (int)fVar19) {
      do {
        fVar19 = fStack_1c0;
        if (*(char *)((uint)*(ushort *)(iVar8 + uVar25 * 2) + (int)puStack_1a8) == '\0') {
          if (*(int *)(iStack_1c + 0xc) == 4) {
            uVar2 = *(ushort *)(iVar8 + uVar25 * 2);
            iVar15 = ((int)uVar25 / 3) * 3;
            uVar3 = *(ushort *)(iVar8 + ((int)(uVar25 + 1) % 3 + iVar15) * 2);
            iVar15 = (int)(uVar25 + 2) % 3 + iVar15;
LAB_00542a7b:
            uVar18 = (uint)uVar3;
            uVar16 = (uint)uVar2;
            uVar11 = (uint)*(ushort *)(iVar8 + iVar15 * 2);
          }
          else if (*(int *)(iStack_1c + 0xc) == 5) {
            if ((int)uVar25 < 2) {
              uVar11 = uVar25 & 0x80000001;
              bVar20 = uVar11 == 0;
              if ((int)uVar11 < 0) {
                bVar20 = (uVar11 - 1 | 0xfffffffe) == 0xffffffff;
              }
              if (bVar20) {
                iVar12 = uVar25 + 1;
                iVar15 = uVar25 + 2;
              }
              else {
                iVar12 = uVar25 + 2;
                iVar15 = uVar25 + 1;
              }
            }
            else {
              uVar11 = uVar25 & 0x80000001;
              bVar20 = uVar11 == 0;
              if ((int)uVar11 < 0) {
                bVar20 = (uVar11 - 1 | 0xfffffffe) == 0xffffffff;
              }
              if (bVar20) {
                iVar12 = uVar25 - 2;
                iVar15 = uVar25 - 1;
              }
              else {
                iVar12 = uVar25 - 1;
                iVar15 = uVar25 - 2;
              }
            }
            uVar2 = *(ushort *)(iVar8 + uVar25 * 2);
            uVar3 = *(ushort *)(iVar8 + iVar12 * 2);
            goto LAB_00542a7b;
          }
          if (fStack_1cc == 2.8026e-45) {
            pfVar13 = (float *)(uVar16 * (int)fStack_1e8 + (int)fStack_1c0);
            unaff_ESI = *pfVar13;
            local_1f0 = pfVar13[1];
            fStack_1ec = pfVar13[2];
            pfVar13 = (float *)(uVar18 * (int)fStack_1e8 + (int)fStack_1c0);
            fStack_17c = *pfVar13;
            fStack_178 = pfVar13[1];
            fStack_174 = pfVar13[2];
            pfVar13 = (float *)(uVar11 * (int)fStack_1e8 + (int)fStack_1c0);
            local_170 = *pfVar13;
            local_16c = pfVar13[1];
            local_168 = pfVar13[2];
          }
          else {
            FUN_004f7dc0(fStack_1cc,&stack0xfffffe0c,uVar16 * (int)fStack_1e8 + (int)fStack_1c0);
            FUN_004f7dc0(fStack_1cc,&fStack_17c,uVar18 * (int)fStack_1e8 + (int)fVar19);
            FUN_004f7dc0(fStack_1cc,&local_170,uVar11 * (int)fStack_1e8 + (int)fVar19);
          }
          puVar10 = puStack_194;
          if (fStack_1d0 == 1.4013e-45) {
            fStack_1e4 = *(float *)(uVar16 * (int)fStack_1c8 + (int)puStack_194);
            fStack_1e0 = *(float *)(uVar16 * (int)fStack_1c8 + 4 + (int)puStack_194);
            fStack_164 = *(float *)(uVar18 * (int)fStack_1c8 + (int)puStack_194);
            fStack_160 = *(float *)(uVar18 * (int)fStack_1c8 + 4 + (int)puStack_194);
            fStack_158 = *(float *)(uVar11 * (int)fStack_1c8 + (int)puStack_194);
            fStack_154 = *(float *)(uVar11 * (int)fStack_1c8 + 4 + (int)puStack_194);
          }
          else {
            FUN_004f81b0(fStack_1d0,&fStack_1e4,
                         (undefined4 *)(uVar16 * (int)fStack_1c8 + (int)puStack_194));
            FUN_004f81b0(fStack_1d0,&fStack_164,
                         (undefined4 *)(uVar18 * (int)fStack_1c8 + (int)puVar10));
            FUN_004f81b0(fStack_1d0,&fStack_158,
                         (undefined4 *)(uVar11 * (int)fStack_1c8 + (int)puVar10));
          }
          if (fStack_1c4 == 0.0) {
            unaff_EDI = 0.0;
            fVar26 = 0.0;
            unaff_EBX = 0.0;
          }
          else if (fStack_1a4 == 2.8026e-45) {
            pfVar13 = (float *)(uVar16 * iStack_1ac + (int)fStack_1c4);
            unaff_EDI = *pfVar13;
            fVar26 = pfVar13[1];
            unaff_EBX = pfVar13[2];
          }
          else {
            FUN_004f7dc0(fStack_1a4,&stack0xfffffe00,uVar16 * iStack_1ac + (int)fStack_1c4);
          }
          fVar19 = fStack_150 - unaff_ESI;
          fVar14 = fStack_14c - local_1f0;
          fVar28 = fStack_148 - fStack_1ec;
          fVar21 = (float10)FUN_004c3b90(fVar28 * fVar28 + fVar19 * fVar19 + fVar14 * fVar14);
          fVar19 = (float)((float10)fVar19 * fVar21);
          fVar14 = (float)((float10)fVar14 * fVar21);
          fVar28 = (float)(fVar21 * (float10)fVar28);
          fStack_134 = fStack_174 - fStack_1ec;
          fStack_1a0 = local_170 - unaff_ESI;
          fStack_1bc = fStack_154 - fStack_1e0;
          fStack_13c = (fStack_17c - unaff_ESI) * fStack_1bc;
          fVar5 = fStack_160 - fStack_1e0;
          fStack_19c = (local_16c - local_1f0) * fVar5;
          fStack_198 = (local_168 - fStack_1ec) * fVar5;
          fStack_1dc = fStack_13c - fStack_1a0 * fVar5;
          fStack_1d8 = (fStack_178 - local_1f0) * fStack_1bc - fStack_19c;
          fStack_1d4 = fStack_134 * fStack_1bc - fStack_198;
          fVar22 = (float10)FUN_004c3b90(fStack_1d4 * fStack_1d4 +
                                         fStack_1dc * fStack_1dc + fStack_1d8 * fStack_1d8);
          fVar21 = (float10)fStack_1dc * fVar22;
          fVar23 = (float10)fStack_1d8 * fVar22;
          fVar22 = (float10)fStack_1d4 * fVar22;
          local_190 = (float)(fVar23 * (float10)unaff_EBX - fVar22 * (float10)fVar26);
          fStack_18c = (float)((float10)unaff_EDI * fVar22 - fVar21 * (float10)unaff_EBX);
          fStack_188 = (float)(fVar21 * (float10)fVar26 - fVar23 * (float10)unaff_EDI);
          pfVar13 = (float *)(iStack_14 + uVar16 * 0x2c);
          *pfVar13 = unaff_ESI;
          pfVar13[1] = local_1f0;
          pfVar13[2] = fStack_1ec;
          if (fStack_1c4 == 0.0) {
            pfVar13[3] = 0.0;
            pfVar13[4] = 0.0;
            pfVar13[5] = 0.0;
          }
          else {
            pfVar13[3] = unaff_EDI;
            pfVar13[4] = fVar26;
            pfVar13[5] = unaff_EBX;
          }
          if (fStack_184 == 0.0) {
            pfVar13[6] = -NAN;
          }
          else {
            pfVar13[6] = *(float *)(uVar16 * (int)fStack_15c + (int)fStack_184);
          }
          pfVar13[7] = fStack_1e4;
          pfVar13[8] = fStack_1e0;
          pfVar13[9] = (float)((float10)fStack_1e4 -
                              (fVar22 * (float10)fVar28 +
                              fVar21 * (float10)fVar19 + fVar23 * (float10)fVar14) *
                              (float10)fStack_1b0);
          pfVar13[10] = fStack_1e0 -
                        (fStack_188 * fVar28 + local_190 * fVar19 + fStack_18c * fVar14) *
                        fStack_1b0;
          *(undefined1 *)(uVar16 + (int)puStack_1a8) = 1;
          iVar8 = iStack_180;
        }
        uVar25 = uVar25 + 1;
        puVar10 = puStack_1a8;
        param_1 = iStack_1c;
      } while ((int)uVar25 < (int)uStack_1b8);
    }
    (**(code **)(**(int **)(param_1 + 8) + 0x30))(*(int **)(param_1 + 8));
    (*(code *)DAT_007d3ff8[0x43])(puVar10);
  }
LAB_005434b7:
  iVar8 = 0;
  do {
    if (local_130[iVar8] != 0) {
      piVar4 = *(int **)((iVar8 + 1) * 0x10 + iStack_8);
      (**(code **)(*piVar4 + 0x30))(piVar4);
    }
    iVar8 = iVar8 + 1;
  } while (iVar8 < 2);
  return 1;
}


```

## U-5029   row-fn 0x004cb840 (FUN_004cb840)

- STATEMENT: Primitive type constant 8 does not match any standard D3DPRIMITIVETYPE (max valid = 6=D3DPT_TRIANGLEFAN); arg roles (param_1*2, literal 8, 0x65, 1, param_2, 0) unclear
- EVIDENCE MISSING (as written): Trace callers FUN_004f13d0 / FUN_004f1b00 to understand draw call context
- PATH (as written): decomp_function on callers

### BODY OF 0x004f13d0 (`FUN_004f13d0`)
signature: `undefined FUN_004f13d0(void)`
callees: FUN_004cb840@0x004cb840, FUN_004cb8a0@0x004cb8a0, FUN_004cbd30@0x004cbd30, FUN_004cc5e0@0x004cc5e0, FUN_004d7ff0@0x004d7ff0, FUN_004d8480@0x004d8480, FUN_004dc5b0@0x004dc5b0, FUN_004dcaa0@0x004dcaa0, FUN_004ebc30@0x004ebc30, FUN_004f3bd0@0x004f3bd0
```c

/* [C1 2026-05-20] Signature: `int FUN_004f13d0(u32 param_1, int param_2)`. */

int FUN_004f13d0(undefined4 param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  undefined4 uVar8;
  int local_cc;
  int iStack_c8;
  uint uStack_c4;
  int local_c0;
  int *piStack_bc;
  undefined1 auStack_b8 [4];
  uint local_b4;
  undefined4 local_b0;
  uint local_ac;
  int iStack_a8;
  int local_a4;
  undefined1 auStack_a0 [144];
  undefined4 uStack_10;
  
  iVar2 = FUN_004cc5e0(param_1,1,&local_cc,&local_ac);
  if (iVar2 != 0) {
    if ((local_ac < 0x35000) || (0x37002 < local_ac)) {
      local_b4 = 2;
      local_b0 = FUN_004d7ff0(0x80000004);
      FUN_004d8480(&local_b4);
    }
    else {
      iVar2 = FUN_004cbd30(param_1,&local_a4,4);
      if (((iVar2 == 4) && (local_cc = local_cc + -4, local_a4 == 9)) &&
         (iVar2 = FUN_004cbd30(param_1,&local_c0,4), iVar2 == 4)) {
        local_cc = local_cc + -4;
        puVar3 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(local_c0 + 0x18,0x3050d);
        piVar6 = (int *)(param_2 + 0x58);
        *piVar6 = (int)puVar3;
        *puVar3 = 0;
        *(undefined4 *)(*piVar6 + 4) = 0;
        *(int *)(*piVar6 + 0xc) = param_2;
        *(int *)(*piVar6 + 8) = local_c0;
        *(int **)(*piVar6 + 0x10) = piVar6;
        *(code **)(*piVar6 + 0x14) = FUN_004ebc30;
        piStack_bc = piVar6;
        iVar2 = FUN_004cbd30(param_1,*piVar6 + 0x18,local_c0);
        if (iVar2 == local_c0) {
          iVar2 = *piVar6;
          local_cc = local_cc - local_c0;
          iVar5 = 2;
          piVar7 = (int *)(iVar2 + 0x20);
          puVar3 = (undefined4 *)(iVar2 + 0x28);
          *piVar7 = 0;
          do {
            *puVar3 = 0;
            puVar3 = puVar3 + 4;
            iVar5 = iVar5 + -1;
          } while (iVar5 != 0);
          *(undefined4 *)(iVar2 + 0x4c) = 0;
          iVar5 = FUN_004cbd30(param_1,&iStack_a8,4);
          if (iVar5 == 4) {
            local_cc = local_cc + -4;
            iVar5 = FUN_004cbd30(param_1,auStack_a0,iStack_a8 * 8);
            if (iVar5 == iStack_a8 * 8) {
              local_cc = local_cc + iStack_a8 * -8;
              FUN_004cb8a0(auStack_a0,iVar2 + 0x4c);
              if ((*(int *)(iVar2 + 0x50) != 0) &&
                 (iVar5 = FUN_004cb840(*(int *)(iVar2 + 0x50),piVar7), iVar5 == 0)) {
LAB_004f1598:
                FUN_004ebc30(*piVar6);
                return 0;
              }
              piVar1 = (int *)*piVar7;
              if (piVar1 != (int *)0x0) {
                uStack_c4 = 0;
                iStack_c8 = 0;
                (**(code **)(*piVar1 + 0x2c))(piVar1,0,0,&uStack_c4,0);
                iVar5 = *(int *)(iVar2 + 0x50) * 2;
                if (uStack_c4 != 0) {
                  iStack_c8 = FUN_004cbd30(param_1,uStack_c4,iVar5);
                  (**(code **)(*(int *)*piVar7 + 0x30))((int *)*piVar7);
                }
                if (iStack_c8 != iVar5) goto LAB_004f1598;
                local_cc = local_cc - iStack_c8;
              }
              uStack_c4 = 0;
              local_b4 = 0;
              iStack_c8 = 1;
              piVar7 = (int *)(iVar2 + 0x28);
              do {
                iVar5 = FUN_004cbd30(param_1,piVar7,0x10);
                if (iVar5 != 0x10) goto LAB_004f1722;
                local_cc = local_cc + -0x10;
                if (*piVar7 != 0) {
                  uStack_c4 = uStack_c4 + 1;
                  iVar5 = *(int *)(iVar2 + 0x54) * piVar7[2];
                  if (*(int *)(param_2 + 0x18) < 2) {
                    *(undefined1 *)((int)piVar7 + 0xe) = 1;
                    iVar4 = FUN_004dc5b0(piVar7[2],iVar5,piVar7,piVar7 + 1);
                  }
                  else {
                    *(undefined1 *)((int)piVar7 + 0xe) = 0;
                    iVar4 = FUN_004dcaa0(iVar5,piVar7);
                  }
                  if ((iVar4 != 0) || (iStack_c8 == 0)) {
                    iStack_c8 = 1;
                    if (*(char *)((int)piVar7 + 0xe) == '\0') {
                      uVar8 = 0x2800;
                    }
                    else {
                      uVar8 = 0x800;
                    }
                    (**(code **)(*(int *)*piVar7 + 0x2c))
                              ((int *)*piVar7,piVar7[1],iVar5,auStack_b8,uVar8);
                    iVar4 = FUN_004cbd30(uStack_10,local_cc,iVar5);
                    (**(code **)(*(int *)*piVar7 + 0x30))((int *)*piVar7);
                    piVar6 = piStack_bc;
                    if (iVar4 == iVar5) {
                      iStack_c8 = 0;
                      local_cc = local_cc - iVar4;
                    }
                  }
                }
                piVar7 = piVar7 + 4;
                local_b4 = local_b4 + 1;
              } while (local_b4 < 2);
              if (iStack_c8 == 0) {
                if (uStack_c4 < 2) {
                  iVar5 = *(int *)(iVar2 + 0x1c);
                  *(undefined4 *)(iVar2 + 0x48) = 0;
                  piVar6 = (int *)(iVar2 + 0x6c);
                  do {
                    iVar5 = iVar5 + -1;
                    *piVar6 = *(uint *)(iVar2 + 0x2c) / *(uint *)(iVar2 + 0x30) + piVar6[-4];
                    piVar6 = piVar6 + 9;
                  } while (iVar5 != 0);
                }
                else {
                  if (((byte)DAT_00912074 & 1) != 0) {
                    *(undefined4 *)(iVar2 + 0x48) = 1;
                  }
                  iVar5 = *(int *)(iVar2 + 0x1c);
                  puVar3 = (undefined4 *)(iVar2 + 0x6c);
                  do {
                    *puVar3 = puVar3[-4];
                    puVar3 = puVar3 + 9;
                    iVar5 = iVar5 + -1;
                  } while (iVar5 != 0);
                }
                iVar5 = *(int *)(iVar2 + 0x1c);
                if (iVar5 != 0) {
                  puVar3 = (undefined4 *)(iVar2 + 0x60);
                  do {
                    uVar8 = FUN_004f3bd0(param_2 + 0x20,*puVar3);
                    *puVar3 = uVar8;
                    puVar3 = puVar3 + 9;
                    iVar5 = iVar5 + -1;
                  } while (iVar5 != 0);
                }
                return param_2;
              }
LAB_004f1722:
              FUN_004ebc30(*piVar6);
              return 0;
            }
          }
        }
      }
    }
  }
  return 0;
}


```

### BODY OF 0x004f1b00 (`FUN_004f1b00`)
signature: `undefined FUN_004f1b00(void)`
callees: FUN_004cb840@0x004cb840, FUN_004cb8a0@0x004cb8a0, FUN_004cbd30@0x004cbd30, FUN_004cc5e0@0x004cc5e0, FUN_004d7ff0@0x004d7ff0, FUN_004d8480@0x004d8480, FUN_004dc5b0@0x004dc5b0, FUN_004e55d0@0x004e55d0, FUN_004ebc30@0x004ebc30, FUN_004f3bd0@0x004f3bd0
```c

/* [C1 2026-05-20] Signature: `int FUN_004f1b00(u32 param_1, int param_2)`. */

int FUN_004f1b00(undefined4 param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  int iVar8;
  int local_d0;
  int iStack_cc;
  uint uStack_c8;
  uint uStack_c4;
  int local_c0;
  int *piStack_bc;
  int local_b8;
  undefined4 local_b4;
  int local_b0;
  undefined1 auStack_ac [4];
  uint local_a8;
  int iStack_a4;
  undefined1 auStack_a0 [144];
  undefined4 uStack_10;
  
  local_b8 = FUN_004e55d0(param_2);
  iVar2 = FUN_004cc5e0(param_1,1,&local_d0,&local_a8);
  if (iVar2 != 0) {
    if ((local_a8 < 0x35000) || (0x37002 < local_a8)) {
      local_b8 = 2;
      local_b4 = FUN_004d7ff0(0x80000004);
      FUN_004d8480(&local_b8);
    }
    else {
      iVar2 = FUN_004cbd30(param_1,&local_b0,4);
      if (((iVar2 == 4) && (local_d0 = local_d0 + -4, local_b0 == 9)) &&
         (iVar2 = FUN_004cbd30(param_1,&local_c0,4), iVar2 == 4)) {
        local_d0 = local_d0 + -4;
        puVar3 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(local_c0 + 0x18,0x3050d);
        piVar6 = (int *)(param_2 + 0x34);
        *piVar6 = (int)puVar3;
        *puVar3 = 0;
        *(undefined4 *)(*piVar6 + 4) = 0;
        *(int *)(*piVar6 + 0xc) = param_2;
        *(int *)(*piVar6 + 8) = local_c0;
        *(int **)(*piVar6 + 0x10) = piVar6;
        *(code **)(*piVar6 + 0x14) = FUN_004ebc30;
        piStack_bc = piVar6;
        iVar2 = FUN_004cbd30(param_1,*piVar6 + 0x18,local_c0);
        if (iVar2 == local_c0) {
          iVar2 = *piVar6;
          local_d0 = local_d0 - local_c0;
          iVar5 = 2;
          piVar7 = (int *)(iVar2 + 0x20);
          puVar3 = (undefined4 *)(iVar2 + 0x28);
          *piVar7 = 0;
          do {
            *puVar3 = 0;
            puVar3 = puVar3 + 4;
            iVar5 = iVar5 + -1;
          } while (iVar5 != 0);
          *(undefined4 *)(iVar2 + 0x4c) = 0;
          iVar5 = FUN_004cbd30(param_1,&iStack_a4,4);
          if (iVar5 == 4) {
            local_d0 = local_d0 + -4;
            uStack_c4 = iStack_a4 * 8;
            iVar5 = FUN_004cbd30(param_1,auStack_a0,iStack_a4 * 8);
            if (iVar5 == uStack_c4) {
              local_d0 = local_d0 - uStack_c4;
              FUN_004cb8a0(auStack_a0,iVar2 + 0x4c);
              if ((*(int *)(iVar2 + 0x50) != 0) &&
                 (iVar5 = FUN_004cb840(*(int *)(iVar2 + 0x50),piVar7), iVar5 == 0)) {
LAB_004f1cd3:
                FUN_004ebc30(*piVar6);
                return 0;
              }
              piVar1 = (int *)*piVar7;
              if (piVar1 != (int *)0x0) {
                uStack_c8 = 0;
                iStack_cc = 0;
                (**(code **)(*piVar1 + 0x2c))(piVar1,0,0,&uStack_c8,0);
                iVar5 = *(int *)(iVar2 + 0x50) * 2;
                if (uStack_c8 != 0) {
                  iStack_cc = FUN_004cbd30(param_1,uStack_c8,iVar5);
                  (**(code **)(*(int *)*piVar7 + 0x30))((int *)*piVar7);
                }
                if (iStack_cc != iVar5) goto LAB_004f1cd3;
                local_d0 = local_d0 - iStack_cc;
              }
              uStack_c8 = 0;
              uStack_c4 = 0;
              iStack_cc = 1;
              piVar7 = (int *)(iVar2 + 0x28);
              do {
                iVar5 = FUN_004cbd30(param_1,piVar7,0x10);
                if (iVar5 != 0x10) goto LAB_004f1e25;
                if (*piVar7 != 0) {
                  uStack_c8 = uStack_c8 + 1;
                  iVar8 = *(int *)(iVar2 + 0x54) * piVar7[2];
                  *(undefined1 *)((int)piVar7 + 0xe) = 1;
                  iVar5 = FUN_004dc5b0(piVar7[2],iVar8,piVar7,piVar7 + 1);
                  if ((iVar5 != 0) || (iStack_cc == 0)) {
                    iStack_cc = 1;
                    (**(code **)(*(int *)*piVar7 + 0x2c))
                              ((int *)*piVar7,piVar7[1],iVar8,auStack_ac,0x800);
                    iVar5 = FUN_004cbd30(uStack_10,local_c0,iVar8);
                    (**(code **)(*(int *)*piVar7 + 0x30))((int *)*piVar7);
                    piVar6 = piStack_bc;
                    if (iVar5 == iVar8) {
                      iStack_cc = 0;
                      local_d0 = local_d0 - iVar5;
                    }
                  }
                }
                piVar7 = piVar7 + 4;
                uStack_c4 = uStack_c4 + 1;
              } while (uStack_c4 < 2);
              if (iStack_cc == 0) {
                if (uStack_c8 < 2) {
                  iVar5 = *(int *)(iVar2 + 0x1c);
                  *(undefined4 *)(iVar2 + 0x48) = 0;
                  piVar6 = (int *)(iVar2 + 0x6c);
                  do {
                    iVar5 = iVar5 + -1;
                    *piVar6 = *(uint *)(iVar2 + 0x2c) / *(uint *)(iVar2 + 0x30) + piVar6[-4];
                    piVar6 = piVar6 + 9;
                  } while (iVar5 != 0);
                }
                else {
                  if (((byte)DAT_00912074 & 1) != 0) {
                    *(undefined4 *)(iVar2 + 0x48) = 1;
                  }
                  iVar5 = *(int *)(iVar2 + 0x1c);
                  puVar3 = (undefined4 *)(iVar2 + 0x6c);
                  do {
                    *puVar3 = puVar3[-4];
                    puVar3 = puVar3 + 9;
                    iVar5 = iVar5 + -1;
                  } while (iVar5 != 0);
                }
                iVar5 = *(int *)(iVar2 + 0x1c);
                if (iVar5 != 0) {
                  puVar3 = (undefined4 *)(iVar2 + 0x60);
                  iVar2 = local_b8 + 0x10;
                  do {
                    uVar4 = FUN_004f3bd0(iVar2,*puVar3);
                    *puVar3 = uVar4;
                    puVar3 = puVar3 + 9;
                    iVar5 = iVar5 + -1;
                  } while (iVar5 != 0);
                }
                return param_2;
              }
LAB_004f1e25:
              FUN_004ebc30(*piVar6);
              return 0;
            }
          }
        }
      }
    }
  }
  return 0;
}


```

## U-5033   row-fn 0x004cbb00 (FUN_004cbb00)

- STATEMENT: 3 args passed (stage, param_2, param_3) but standard D3D9 SetTexture takes (stage, texture); third arg role unclear
- EVIDENCE MISSING (as written): Trace caller FUN_0049a750 to see what is passed as param_3
- PATH (as written): decomp_function on caller

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5034   row-fn 0x004cbb20 (FUN_004cbb20)

- STATEMENT: Only 2 args passed to vtable+0x1a8; standard SetTextureStageState takes (stage, type, value) — decompiler may have folded args or signature is non-standard
- EVIDENCE MISSING (as written): Trace callers FUN_0049aae0 / FUN_00543710 to see full arg list
- PATH (as written): decomp_function on callers

### BODY OF 0x0049aae0 (`FUN_0049aae0`)
signature: `undefined FUN_0049aae0(void)`
callees: FUN_004cbad0@0x004cbad0, FUN_004cbb20@0x004cbb20
```c

/* [C1 2026-05-18] Initialiser pair (mode-1 init): */

void FUN_0049aae0(void)

{
  FUN_004cbad0(&DAT_005d0438,&DAT_00773974);
  FUN_004cbb20(&DAT_005d0518,&DAT_0077397c);
  return;
}


```

### BODY OF 0x00543710 (`FUN_00543710`)
signature: `undefined FUN_00543710(void)`
callees: FUN_004cbb20@0x004cbb20, FUN_004cbb60@0x004cbb60, FUN_004d4170@0x004d4170, FUN_004d41e0@0x004d41e0, FUN_004d4380@0x004d4380, FUN_004d4dd0@0x004d4dd0, FUN_004d4f00@0x004d4f00, FUN_004d4f90@0x004d4f90, FUN_004eb3c0@0x004eb3c0, FUN_004eb9d0@0x004eb9d0, FUN_004ec720@0x004ec720, FUN_004ec730@0x004ec730
```c

/* [C1 2026-05-20] Zero-arg subsystem-init function. Returns 1. */

undefined4 FUN_00543710(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  
  iVar1 = FUN_004d4170();
  if (iVar1 == 0) {
LAB_00543758:
    iVar1 = 0;
  }
  else {
    *(undefined4 *)(iVar1 + 0x2c) = 0x120;
    iVar2 = FUN_004d4dd0(iVar1);
    if (iVar2 == 0) {
      FUN_004d41e0(iVar1);
      goto LAB_00543758;
    }
    uVar3 = FUN_004eb3c0();
    uVar3 = FUN_004d4f90(iVar2,0,uVar3,0);
    FUN_004d4380(uVar3);
  }
  DAT_007dc5f4 = iVar1;
  puVar4 = (undefined4 *)FUN_004eb3c0();
  uVar3 = FUN_004d4f00(DAT_007dc5f4,*puVar4,0,0);
  DAT_007dc62c = FUN_004ec730(uVar3);
  FUN_004ec720(uVar3,&LAB_00543950);
  FUN_004ec740(uVar3,&LAB_00543500);
  iVar1 = FUN_004d4170();
  if (iVar1 != 0) {
    *(undefined4 *)(iVar1 + 0x2c) = 0x120;
    iVar2 = FUN_004d4dd0(iVar1);
    if (iVar2 != 0) {
      uVar3 = FUN_004eb9d0();
      uVar3 = FUN_004d4f90(iVar2,0,uVar3,0);
      FUN_004d4380(uVar3);
      goto LAB_005437e1;
    }
    FUN_004d41e0(iVar1);
  }
  iVar1 = 0;
LAB_005437e1:
  DAT_007dc5f8 = iVar1;
  puVar4 = (undefined4 *)FUN_004eb9d0();
  uVar3 = FUN_004d4f00(DAT_007dc5f8,*puVar4,0,0);
  DAT_007dc630 = FUN_004ec730(uVar3);
  FUN_004ec720(uVar3,&LAB_005439d0);
  FUN_004ec740(uVar3,&LAB_00543500);
  iVar1 = FUN_004cbb60();
  DAT_007dc624 = *(uint *)(iVar1 + 0x94);
  if (DAT_007dc624 < 2) {
    DAT_007dc614 = 0;
    DAT_007dc618 = 0;
    DAT_007dc61c = 0;
    DAT_007dc620 = 0;
    DAT_00623d30 = 0;
    DAT_00623d34 = 0;
  }
  else {
    DAT_007dc614 = (uint)(1 < *(uint *)(iVar1 + 0x98));
    DAT_007dc618 = *(uint *)(iVar1 + 0x90) & 0x1000000;
    DAT_007dc61c = *(uint *)(iVar1 + 0x90) & 0x10;
  }
  if (0x100 < (*(uint *)(iVar1 + 0xcc) & 0xffff)) {
    DAT_007dc628 = 1;
    FUN_004cbb20(&DAT_005e4580,&DAT_007dc5fc);
    FUN_004cbb20(&DAT_005e45e0,&DAT_007dc600);
    FUN_004cbb20(&DAT_005e4630,&DAT_007dc604);
    FUN_004cbb20(&DAT_005e4670,&DAT_007dc608);
    FUN_004cbb20(&DAT_005e46f8,&DAT_007dc60c);
    FUN_004cbb20(&DAT_005e47a8,&DAT_007dc610);
    return 1;
  }
  DAT_007dc628 = 0;
  DAT_007dc5fc = 0;
  DAT_007dc600 = 0;
  DAT_007dc604 = 0;
  DAT_007dc608 = 0;
  DAT_007dc60c = 0;
  DAT_007dc610 = 0;
  return 1;
}


```

## U-5038   row-fn 0x004cbbd0 (FUN_004cbbd0)

- STATEMENT: Sign-byte extraction at raw pfVar1+0x11/+0x12 for n-vertex AABB selection and AABB layout (stride -0xc, base +0x14) are non-obvious; exact byte layout of D3DPLANE struct and AABB layout not confirmed
- EVIDENCE MISSING (as written): Verify plane struct layout (5 floats = [nx,ny,nz,d,signBytes]) and AABB layout at call site
- PATH (as written): decomp_function on caller FUN_004ec130

### BODY OF 0x004ec130 (`FUN_004ec130`)
signature: `undefined FUN_004ec130(void)`
callees: FUN_004cbb70@0x004cbb70, FUN_004cbbd0@0x004cbbd0, FUN_004d5480@0x004d5480, RpAtomicGetWorldBoundingSphere@0x004e6100
```c

/* [C1 2026-05-19] Camera/frustum-cull-around-atomic routine. */

void FUN_004ec130(int param_1,int param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = *DAT_007d3ff8;
  if (param_2 == 1) {
    uVar2 = RpAtomicGetWorldBoundingSphere(param_1);
    FUN_004cbb70(uVar1,uVar2);
  }
  else {
    FUN_004cbbd0(uVar1,param_1 + 0x60);
  }
  FUN_004d5480();
  return;
}


```

## U-5105   row-fn 0x004cce20 (FUN_004cce20)

- STATEMENT: Names of LAB_004ccef0 / LAB_005aead0 / LAB_004ccf00 — Ghidra shows LAB_ labels not FUN_ symbols; implementations not confirmed
- EVIDENCE MISSING (as written): Disassemble those addresses to verify they are malloc/calloc/realloc stubs
- PATH (as written): listing_disassemble_seed on each address

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5106   row-fn 0x004ccf20 (FUN_004ccf20)

- STATEMENT: Ownership-flag byte at pool_base-1 (bit 0) — controls vtable+0x10c vs vtable+0x11c free path; exact semantics vs FUN_004cc820 param_5[6] (writes 2 or 3) unclear
- EVIDENCE MISSING (as written): Cross-ref FUN_004cc820 allocation path and pool header layout
- PATH (as written): decomp_function on FUN_004cc820

### BODY OF 0x004cc820 (`FUN_004cc820`)
signature: `undefined FUN_004cc820(void)`
```c

/* [C1 2026-05-02] Allocates and initialises a RenderWare free-list pool (`RwFreeList` equivalent).
   Called by… */

uint * FUN_004cc820(int param_1,uint param_2,uint param_3,int param_4,uint *param_5,uint param_6)

{
  uint *puVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  
  if (DAT_006182b0 == 0) {
    param_4 = 0;
  }
  if (param_3 == 0) {
    param_3 = 4;
  }
  if (param_5 == (uint *)0x0) {
    if (DAT_007d45fc == (uint *)0x0) {
      param_5 = (uint *)(**(code **)(DAT_007d3ff8 + 0x108))(0x24,param_6 & 0xff0000);
    }
    else {
      param_5 = (uint *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d45fc,param_6 & 0xff0000);
    }
    if (param_5 == (uint *)0x0) {
      return (uint *)0x0;
    }
    param_5[6] = 2;
  }
  else {
    param_5[6] = 3;
  }
  uVar2 = param_1 + -1 + param_3 & ~(param_3 - 1);
  puVar1 = param_5 + 4;
  *param_5 = uVar2;
  param_5[1] = param_2;
  uVar5 = param_2 + 7 >> 3;
  param_5[3] = param_3;
  param_5[2] = uVar5;
  *puVar1 = (uint)puVar1;
  param_5[5] = (uint)puVar1;
  if (param_4 != 0) {
    do {
      puVar3 = (uint *)(**(code **)(DAT_007d3ff8 + 0x108))
                                 (uVar2 * param_2 + uVar5 + 7 + param_3,param_6);
      if (puVar3 == (uint *)0x0) {
        puVar3 = (uint *)*puVar1;
        while (puVar3 != puVar1) {
          *(uint *)puVar3[1] = *puVar3;
          *(uint *)(*puVar3 + 4) = puVar3[1];
          (**(code **)(DAT_007d3ff8 + 0x10c))(puVar3);
          puVar3 = (uint *)*puVar1;
        }
        if ((param_5[6] & 1) == 0) {
          if ((DAT_007d45fc != param_5) && (DAT_007d45fc != (uint *)0x0)) {
            (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d45fc,param_5);
            return (uint *)0x0;
          }
          (**(code **)(DAT_007d3ff8 + 0x10c))(param_5);
        }
        return (uint *)0x0;
      }
      puVar3[1] = 0;
      *puVar3 = 0;
      uVar4 = *puVar1;
      puVar3[1] = (uint)puVar1;
      *puVar3 = uVar4;
      *(uint **)(*puVar1 + 4) = puVar3;
      *puVar1 = (uint)puVar3;
      puVar3 = puVar3 + 2;
      for (uVar4 = param_2 + 7 >> 5; uVar4 != 0; uVar4 = uVar4 - 1) {
        *puVar3 = 0;
        puVar3 = puVar3 + 1;
      }
      for (uVar4 = uVar5 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
        *(undefined1 *)puVar3 = 0;
        puVar3 = (uint *)((int)puVar3 + 1);
      }
      param_4 = param_4 + -1;
    } while (param_4 != 0);
  }
  uVar2 = (uint)DAT_007d45cc;
  puVar1 = param_5 + 7;
  param_5[8] = (uint)&DAT_007d45cc;
  *puVar1 = uVar2;
  *(uint **)((int)DAT_007d45cc + 4) = puVar1;
  DAT_007d45cc = puVar1;
  return param_5;
}


```

## U-5125   row-fn 0x004cd550 (FUN_004cd550)

- STATEMENT: PrimType=6 identity — RW has 6 prim types but D3DPRIMITIVETYPE stops at D3DPT_TRIANGLEFAN=6; may be PointList or Mashed extension
- EVIDENCE MISSING (as written): Trace callers; cross-ref RW IM3D prim type enum
- PATH (as written): decomp_function on caller

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5127   row-fn 0x004cdf20 (FUN_004cdf20)

- STATEMENT: Flag bit 0x02 meaning — preserved across cross-depth copy, never cleared; may be "palette-owned-by-image" or external flag
- EVIDENCE MISSING (as written): Trace callers of RwImageCopy; cross-ref RW image flags enum
- PATH (as written): decomp_function on callers

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5128   row-fn 0x004ce390 (FUN_004ce390)

- STATEMENT: FUN_004ce690 callback identity — called with (filename, &handler); may be RwImageReadCallback or RwImageFindFileTypeCallback
- EVIDENCE MISSING (as written): Decomp 0x004ce690
- PATH (as written): decomp_function

### BODY OF 0x004ce690 (`FUN_004ce690`)
signature: `undefined FUN_004ce690(void)`
callees: FUN_00550b00@0x00550b00
```c

/* [C1 2026-05-19] Signature `int f(const char* candidate_path, ImageHandlerPair* user_ctx)`. */

undefined4 FUN_004ce690(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  
  iVar1 = FUN_00550b00(param_1);
  if (iVar1 != 0) {
    iVar1 = (*(code *)*param_2)(param_1);
    param_2[1] = iVar1;
    if (iVar1 != 0) {
      return 0;
    }
  }
  return param_1;
}


```

## U-5151   row-fn 0x004ce6c0 (FUN_004ce6c0)

- STATEMENT: LAB_004ce6f0 is a label reference not a named function; full behavior unconfirmed
- EVIDENCE MISSING (as written): function_at 0x004ce6f0 to check Ghidra split
- PATH (as written): function_at + decomp_function 0x004ce6f0

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5152   row-fn 0x004ce790 (FUN_004ce790)

- STATEMENT: **OBJECT IDENTIFIED 2026-09-11, slot NAME still needs an external reference.** `DAT_007d3ff8` is a pointer to the structure at `DAT_007d3ec8`: two of its six writes store the literal address, `0x004c31e9 MOV dword ptr [0x007d3ff8],0x7d3ec8` and `0x004c32b7`, and the block itself is filled by `MOVSD.REP` at `0x004c307a` / `0x004c31cd` (see [[U-0091]]). So the object behind every `DAT_007d3ff8 + N` dispatch in the tree is that 300-byte block. **What this does NOT give is the RenderWare API name for the slot** - that needs RW headers or librw, not the binary, and guessing it from behaviour is the inference NO-GUESSING forbids (the same refusal as [[U-4707]]). Keeps gating; this is now the NEEDS
- EVIDENCE MISSING (as written): decomp callers of vtable[0xfc]/[0xf8] slots
- PATH (as written): cross-check vtable slot usage in other register paths

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5154   row-fn 0x004ce8e0 (FUN_004ce8e0)

- STATEMENT: node[+0x2c] is write callback vs. some other secondary callback; unconfirmed
- EVIDENCE MISSING (as written): decomp FUN_00498810 (single caller) for context
- PATH (as written): decomp_function 0x00498810

### BODY OF 0x00498810 (`FUN_00498810`)
signature: `undefined FUN_00498810(void)`
callees: FUN_004cdca0@0x004cdca0, FUN_004cdd00@0x004cdd00, FUN_004cdd60@0x004cdd60, FUN_004ce8e0@0x004ce8e0, FUN_004d52d0@0x004d52d0
```c

/* [C1 2026-06-01] `void FUN_00498810(undefined4 param_1)` — implicit `this` arrives in EAX (`int
   in_EAX`). */

void FUN_00498810(undefined4 param_1)

{
  int iVar1;
  int in_EAX;
  int iVar2;
  
  iVar1 = *(int *)(in_EAX + 0x60);
  if ((iVar1 != 0) &&
     (iVar2 = FUN_004cdca0(*(undefined4 *)(iVar1 + 0xc),*(undefined4 *)(iVar1 + 0x10),0x20),
     iVar2 != 0)) {
    FUN_004cdd60(iVar2);
    FUN_004d52d0(iVar2,iVar1);
    FUN_004ce8e0(iVar2,param_1);
    FUN_004cdd00(iVar2);
  }
  return;
}


```

## U-5156   row-fn 0x004cecb0 (FUN_004cecb0)

- STATEMENT: Flag bit 0x02 on RwImage — "palette is shared" vs. other pixel-ownership flag unconfirmed
- EVIDENCE MISSING (as written): cross-check all sites writing/reading bit 0x02 on RwImage
- PATH (as written): search_pcode / decomp_function at write sites

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5190   row-fn 0x004d1e30 (FUN_004d1e30)

- STATEMENT: Vtable offsets CreateDepthStencilSurface=+0x74 CreateTexture=+0x5c GetBackBuffer=+0x14 CreateAdditionalSwapChain=+0x34 inferred from D3D9 vtable layout not independently symbol-confirmed
- EVIDENCE MISSING (as written): Verify against D3D9 COM vtable listing or RW D3D9 plugin source
- PATH (as written): list_modules / get_module_exports on d3d9.dll

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5375   row-fn 0x004d84e0 (RwResourcesEmptyArena_PopSlot)

- STATEMENT: No caller observed in Ghidra or prior sessions — cannot confirm call-site context
- EVIDENCE MISSING (as written): search hooks.csv for resource-arena callee references
- PATH (as written): decomp_function all resource-arena callers

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5377   row-fn 0x004d87a0 (RwStringStreamWrite)

- STATEMENT: 0x37002 arg to FUN_004cc580 — exact chunk-type semantic unconfirmed
- EVIDENCE MISSING (as written): FUN_004cc580 not yet decomped
- PATH (as written): decomp_function 0x004cc580

### BODY OF 0x004cc580 (`FUN_004cc580`)
signature: `undefined FUN_004cc580(void)`
callees: FUN_004cbe80@0x004cbe80
```c

/* [C1 2026-05-19] Signature `void f(int chunk_writer, u32 a, u32 b, u32 p4, u32 p5)`. */

void FUN_004cc580(undefined4 param_1,undefined4 param_2,undefined4 param_3,uint param_4,uint param_5
                 )

{
  undefined4 local_c;
  undefined4 local_8;
  uint local_4;
  
  local_c = param_2;
  local_8 = param_3;
  local_4 = (param_4 + 0x10000 & 0x3ff00) << 0xe | (param_4 & 0x3f) << 0x10 | param_5 & 0xffff;
  FUN_004cbe80(param_1,&local_c,0xc);
  return;
}


```

## U-5380   row-fn 0x004d8430 (rwID_ERRORMODULE_ctor)

- STATEMENT: Ghidra does not recognize function boundary (LAB_ not FUN_)
- EVIDENCE MISSING (as written): Ghidra auto-analysis missed function start
- PATH (as written): D-8560 librw_plugin_compat-cont1 decompile session

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5403   row-fn 0x004d9530 (FUN_004d9530)

- STATEMENT: Byte at `puVar[6]` cleared during partition reset — role (in-use flag vs. other sentinel) unconfirmed; mechanical description complete without knowing it
- EVIDENCE MISSING (as written): Trace partition-reset site; check if any reader tests this byte
- PATH (as written): decomp_function FUN_004d9530 + search_pcode for reads at offset +6*4

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5425   row-fn 0x004db2e0 (FUN_004db2e0)

- STATEMENT: FUN_004cdca0/FUN_004cdd60/FUN_004cdd00/FUN_004cecb0 shapes match RtImageNew/RtImageCopy pattern but not verified against exported names
- EVIDENCE MISSING (as written): Decomp each callee; cross-check against RenderWare RT image API
- PATH (as written): decomp_function on each callee

### BODY OF 0x004cdca0 (`FUN_004cdca0`)
signature: `undefined FUN_004cdca0(void)`
callees: FUN_004d8000@0x004d8000
```c

/* [C1 2026-05-19] Signature `RwImage* f(int width, int height, int depth)`. */

undefined4 * FUN_004cdca0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)
           (**(code **)(DAT_007d3ff8 + 0x118))(*(undefined4 *)(DAT_007d4628 + DAT_007d3ff8),0x30018)
  ;
  if (puVar1 == (undefined4 *)0x0) {
    return (undefined4 *)0x0;
  }
  puVar1[1] = param_1;
  puVar1[2] = param_2;
  puVar1[3] = param_3;
  puVar1[5] = 0;
  puVar1[6] = 0;
  *puVar1 = 0;
  FUN_004d8000(&DAT_006182b4,puVar1);
  return puVar1;
}


```

### BODY OF 0x004cdd60 (`FUN_004cdd60`)
signature: `undefined FUN_004cdd60(void)`
callees: FUN_004d7ff0@0x004d7ff0, FUN_004d8480@0x004d8480
```c

/* [C1 2026-05-07] FUN_004cdd60 (RwImageAllocatePixels candidate) */

uint * FUN_004cdd60(uint *param_1)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar2 = param_1[3];
  if ((uVar2 == 4) || (uVar2 == 8)) {
    bVar1 = true;
    iVar3 = (1 << ((byte)uVar2 & 0x1f)) << 2;
  }
  else {
    bVar1 = false;
    iVar3 = 0;
  }
  uVar2 = ((int)(uVar2 + 7) >> 3) * param_1[1] + 3 & 0xfffffffc;
  iVar4 = param_1[2] * uVar2;
  param_1[4] = uVar2;
  iVar3 = iVar4 + iVar3;
  uVar2 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar3,0x30018);
  param_1[5] = uVar2;
  if (uVar2 == 0) {
    uStack_8 = 1;
    uStack_4 = FUN_004d7ff0(0x80000013,iVar3);
    FUN_004d8480(&uStack_8);
    return (uint *)0x0;
  }
  if (bVar1) {
    param_1[6] = uVar2 + iVar4;
    *param_1 = *param_1 | 1;
    return param_1;
  }
  param_1[6] = 0;
  *param_1 = *param_1 | 1;
  return param_1;
}


```

### BODY OF 0x004cdd00 (`FUN_004cdd00`)
signature: `undefined FUN_004cdd00(void)`
callees: FUN_004d8060@0x004d8060
```c

/* [C1 2026-05-19] Signature `bool f(RwImage* image)`. */

undefined4 FUN_004cdd00(uint *param_1)

{
  if ((*param_1 & 1) != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(param_1[5]);
    param_1[5] = 0;
    param_1[6] = 0;
    *param_1 = *param_1 & 0xfffffffe;
  }
  FUN_004d8060(&DAT_006182b4,param_1);
  (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d4628 + DAT_007d3ff8),param_1);
  return 1;
}


```

### BODY OF 0x004cecb0 (`FUN_004cecb0`)
signature: `undefined FUN_004cecb0(void)`
callees: FUN_004d7ff0@0x004d7ff0, FUN_004d8480@0x004d8480
```c

/* [C1 2026-05-19] Signature `void f(RwImage* dst, RwImage* src)`. */

void FUN_004cecb0(uint *param_1,uint *param_2)

{
  uint *puVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  undefined4 local_8;
  undefined4 local_4;
  
  puVar3 = param_1;
  uVar4 = param_2[3];
  if (param_1[3] == uVar4) {
    if ((((undefined4 *)param_1[6] != (undefined4 *)0x0) &&
        ((undefined4 *)param_2[6] != (undefined4 *)0x0)) && ((int)uVar4 < 9)) {
      puVar9 = (undefined4 *)param_2[6];
      puVar8 = (undefined4 *)param_1[6];
      for (uVar4 = 1 << ((byte)uVar4 & 0x1f) & 0x3fffffff; uVar4 != 0; uVar4 = uVar4 - 1) {
        *puVar8 = *puVar9;
        puVar9 = puVar9 + 1;
        puVar8 = puVar8 + 1;
      }
      for (iVar5 = 0; iVar5 != 0; iVar5 = iVar5 + -1) {
        *(undefined1 *)puVar8 = *(undefined1 *)puVar9;
        puVar9 = (undefined4 *)((int)puVar9 + 1);
        puVar8 = (undefined4 *)((int)puVar8 + 1);
      }
    }
    puVar1 = param_1 + 2;
    puVar9 = (undefined4 *)param_2[5];
    uVar4 = ((int)(param_1[3] + 7) >> 3) * param_1[1];
    puVar8 = (undefined4 *)param_1[5];
    param_1 = (uint *)0x0;
    if (0 < (int)*puVar1) {
      do {
        puVar10 = puVar9;
        puVar11 = puVar8;
        for (uVar6 = uVar4 >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
          *puVar11 = *puVar10;
          puVar10 = puVar10 + 1;
          puVar11 = puVar11 + 1;
        }
        for (uVar6 = uVar4 & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
          *(undefined1 *)puVar11 = *(undefined1 *)puVar10;
          puVar10 = (undefined4 *)((int)puVar10 + 1);
          puVar11 = (undefined4 *)((int)puVar11 + 1);
        }
        puVar8 = (undefined4 *)((int)puVar8 + puVar3[4]);
        puVar9 = (undefined4 *)((int)puVar9 + param_2[4]);
        param_1 = (uint *)((int)param_1 + 1);
      } while ((int)param_1 < (int)puVar3[2]);
    }
    goto LAB_004cee6a;
  }
  uVar6 = param_2[6];
  uVar2 = param_1[1];
  puVar9 = (undefined4 *)param_2[5];
  puVar8 = (undefined4 *)param_1[5];
  uVar4 = uVar4 << 8 | param_1[3];
  if (uVar4 < 0x809) {
    if ((uVar4 == 0x808) || (uVar4 == 0x404)) goto LAB_004cee6a;
    if (uVar4 == 0x408) {
      param_1 = (uint *)param_1[2];
      if (0 < (int)param_1) {
        do {
          puVar10 = puVar9;
          puVar11 = puVar8;
          for (uVar4 = uVar2 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
            *puVar11 = *puVar10;
            puVar10 = puVar10 + 1;
            puVar11 = puVar11 + 1;
          }
          for (uVar4 = uVar2 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
            *(undefined1 *)puVar11 = *(undefined1 *)puVar10;
            puVar10 = (undefined4 *)((int)puVar10 + 1);
            puVar11 = (undefined4 *)((int)puVar11 + 1);
          }
          puVar9 = (undefined4 *)((int)puVar9 + param_2[4]);
          puVar8 = (undefined4 *)((int)puVar8 + puVar3[4]);
          param_1 = (uint *)((int)param_1 - 1);
        } while (param_1 != (uint *)0x0);
      }
      goto LAB_004cee6a;
    }
    if (uVar4 != 0x420) goto LAB_004cee45;
  }
  else if (uVar4 != 0x820) {
    if (uVar4 == 0x2020) goto LAB_004cee6a;
LAB_004cee45:
    local_8 = 1;
    local_4 = FUN_004d7ff0(0x80000009);
    FUN_004d8480(&local_8);
    puVar3 = (uint *)0x0;
    goto LAB_004cee6a;
  }
  param_1 = (uint *)param_1[2];
  if (0 < (int)param_1) {
    do {
      iVar5 = 0;
      if (0 < (int)uVar2) {
        do {
          iVar7 = iVar5 + 1;
          puVar8[iVar5] = *(undefined4 *)(uVar6 + (uint)*(byte *)(iVar5 + (int)puVar9) * 4);
          iVar5 = iVar7;
        } while (iVar7 < (int)uVar2);
      }
      puVar9 = (undefined4 *)((int)puVar9 + param_2[4]);
      puVar8 = (undefined4 *)((int)puVar8 + puVar3[4]);
      param_1 = (uint *)((int)param_1 - 1);
    } while (param_1 != (uint *)0x0);
  }
LAB_004cee6a:
  uVar4 = *puVar3;
  *puVar3 = uVar4 & 0xfffffffd;
  *puVar3 = *param_2 & 2 | uVar4 & 0xfffffffd;
  return;
}


```

## U-5428   row-fn 0x004dc160 (FUN_004dc160)

- STATEMENT: Dual identical aligned/unaligned branches — defensive SSE alignment-fastpath not diverged in this build; whether SSE was used originally unconfirmed
- EVIDENCE MISSING (as written): Compare against RW3 matrix transform reference
- PATH (as written): decomp surrounding matrix functions; check for SIMD patterns

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.

## U-5585   row-fn 0x0041d410 (FUN_0041d410)

- STATEMENT: Vtable slot 0x48 (index 18) semantics unknown — virtual method purpose not named
- EVIDENCE MISSING (as written): Trace vtable layout of child object type; decomp one child object ctor or cross-ref
- PATH (as written): decomp_function one of the child-object constructors

### NO BODY PROVIDED
Answer NO-NEW-EVIDENCE unless the statement already settles it.
