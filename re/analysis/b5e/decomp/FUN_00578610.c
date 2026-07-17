
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `int FUN_00578610(undefined4 world, int *body_pair, float
   depth_tol, float… */

undefined4
FUN_00578610(undefined4 param_1,int *param_2,float param_3,float *param_4,float *param_5,
            float *param_6)

{
  ushort uVar1;
  ushort uVar2;
  ushort uVar3;
  ushort uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  float fVar9;
  float *pfVar10;
  uint uVar11;
  float *pfVar12;
  float10 fVar13;
  float10 fVar14;
  float10 fVar15;
  float10 fVar16;
  undefined1 *puStack_144;
  float local_140;
  float fStack_13c;
  float fStack_138;
  float fStack_134;
  float fStack_130;
  float fStack_12c;
  uint uStack_128;
  float fStack_124;
  float fStack_120;
  float fStack_11c;
  undefined4 local_118;
  undefined4 local_114;
  undefined4 local_110;
  undefined4 local_10c;
  undefined4 local_108;
  int local_104;
  float fStack_100;
  float afStack_fc [11];
  float local_d0 [12];
  undefined1 local_a0 [80];
  undefined1 auStack_50 [80];

  iVar5 = *param_2;
  iVar6 = param_2[1];
  iVar7 = *(int *)(*(int *)(iVar5 + 8) + 0x5c);
  iVar8 = *(int *)(*(int *)(iVar6 + 8) + 0x5c);
  if ((uint)*(byte *)(iVar8 + 0x39) + (uint)*(byte *)(iVar7 + 0x39) == 0) {
    *param_4 = *(float *)(*(int *)(iVar5 + 0x10) + 0x30) - *(float *)(*(int *)(iVar6 + 0x10) + 0x30)
    ;
    param_4[1] = *(float *)(*(int *)(*param_2 + 0x10) + 0x34) -
                 *(float *)(*(int *)(param_2[1] + 0x10) + 0x34);
    param_4[2] = *(float *)(*(int *)(*param_2 + 0x10) + 0x38) -
                 *(float *)(*(int *)(param_2[1] + 0x10) + 0x38);
    fVar13 = (float10)FUN_005667c0(param_4,param_4);
    iVar5 = *(int *)(param_2[1] + 0x10);
    local_140 = *(float *)(iVar5 + 0x38) * param_4[2] +
                *(float *)(iVar5 + 0x30) * *param_4 + *(float *)(iVar5 + 0x34) * param_4[1];
  }
  else if (*(char *)(iVar7 + 0x38) == '\x02') {
    iVar5 = *(int *)(iVar5 + 0x10);
    local_118 = *(undefined4 *)(iVar5 + 0x20);
    local_114 = *(undefined4 *)(iVar5 + 0x24);
    local_110 = *(undefined4 *)(iVar5 + 0x28);
    local_10c = 0xff7fffff;
    local_108 = 0;
    fVar13 = (float10)FUN_00578bd0(param_2,&local_118,param_4,&local_140);
  }
  else if (*(char *)(iVar8 + 0x38) == '\x02') {
    iVar5 = *(int *)(iVar6 + 0x10);
    local_118 = *(undefined4 *)(iVar5 + 0x20);
    local_114 = *(undefined4 *)(iVar5 + 0x24);
    local_110 = *(undefined4 *)(iVar5 + 0x28);
    local_10c = 0xff7fffff;
    local_108 = 0;
    fVar13 = (float10)FUN_00578cb0(param_2,&local_118,param_4,&local_140);
  }
  else {
    iVar6 = *(int *)(*(int *)(iVar6 + 8) + 0x5c);
    iVar7 = *(int *)(*(int *)(iVar5 + 8) + 0x5c);
    uVar1 = *(ushort *)(iVar7 + 0x3a);
    uVar2 = *(ushort *)(iVar6 + 0x3a);
    uVar3 = *(ushort *)(iVar6 + 0x3c);
    uVar4 = *(ushort *)(iVar7 + 0x3c);
    local_104 = iVar7;
    (**(code **)(iVar7 + 0x1c))(*(int *)(iVar5 + 8),*(undefined4 *)(iVar5 + 0x10),local_a0,local_d0)
    ;
    uStack_128 = *(int *)(param_2[1] + 8);
    (**(code **)(*(int *)(uStack_128 + 0x5c) + 0x1c))
              (uStack_128,*(undefined4 *)(param_2[1] + 0x10),auStack_50,&fStack_100);
    if ((uint)uVar2 * (uint)uVar1 + (uint)uVar3 + (uint)uVar4 == 0) {
      if (*(short *)(iVar7 + 0x3a) == 0) {
        pfVar10 = &fStack_100;
        iVar5 = *(int *)(param_2[1] + 0x10);
        iVar6 = *(int *)(*param_2 + 0x10);
      }
      else {
        pfVar10 = local_d0;
        iVar5 = *(int *)(*param_2 + 0x10);
        iVar6 = *(int *)(param_2[1] + 0x10);
      }
      FUN_00578d90(iVar6 + 0x30,iVar5 + 0x30,pfVar10,param_4);
      fVar13 = (float10)FUN_00578b20(param_2,param_4,&local_140);
    }
    else {
      uVar11 = 0;
      local_140 = 0.0;
      fStack_13c = -3.4028235e+38;
      if (*(short *)(iVar7 + 0x3c) != 0) {
        puStack_144 = local_a0;
        do {
          fVar13 = (float10)FUN_00578bd0(param_2,puStack_144,&fStack_124,&fStack_12c);
          if ((float10)fStack_13c < fVar13) {
            local_140 = fStack_12c;
            *param_4 = fStack_124;
            fStack_13c = (float)fVar13;
            param_4[1] = fStack_120;
            param_4[2] = fStack_11c;
          }
          uVar11 = uVar11 + 1;
          puStack_144 = puStack_144 + 0x14;
        } while (uVar11 < *(ushort *)(iVar7 + 0x3c));
      }
      uVar11 = 0;
      if (*(short *)(iVar6 + 0x3c) != 0) {
        puStack_144 = auStack_50;
        do {
          fVar13 = (float10)FUN_00578cb0(param_2,puStack_144,&fStack_124,&fStack_12c);
          if ((float10)fStack_13c < fVar13) {
            local_140 = fStack_12c;
            *param_4 = fStack_124;
            fStack_13c = (float)fVar13;
            param_4[1] = fStack_120;
            param_4[2] = fStack_11c;
          }
          uVar11 = uVar11 + 1;
          puStack_144 = puStack_144 + 0x14;
        } while (uVar11 < *(ushort *)(iVar6 + 0x3c));
      }
      uStack_128 = 0;
      if (*(short *)(iVar7 + 0x3a) != 0) {
        pfVar10 = local_d0;
        do {
          puStack_144 = (undefined1 *)0x0;
          if (*(short *)(iVar6 + 0x3a) != 0) {
            pfVar12 = afStack_fc;
            do {
              fStack_138 = pfVar12[1] * pfVar10[1] - *pfVar12 * pfVar10[2];
              fStack_134 = pfVar12[-1] * pfVar10[2] - *pfVar10 * pfVar12[1];
              fStack_130 = *pfVar12 * *pfVar10 - pfVar12[-1] * pfVar10[1];
              fVar9 = fStack_130 * fStack_130 + fStack_138 * fStack_138 + fStack_134 * fStack_134;
              if (_DAT_005ce54c < fVar9) {
                fVar9 = _DAT_005cc320 / SQRT(fVar9);
                fStack_138 = fStack_138 * fVar9;
                fStack_134 = fStack_134 * fVar9;
                fStack_130 = fVar9 * fStack_130;
                fVar13 = (float10)FUN_00578b20(param_2,&fStack_138,&fStack_12c);
                if ((float10)fStack_13c < fVar13) {
                  local_140 = fStack_12c;
                  *param_4 = fStack_138;
                  fStack_13c = (float)fVar13;
                  param_4[1] = fStack_134;
                  param_4[2] = fStack_130;
                }
              }
              puStack_144 = (undefined1 *)((int)puStack_144 + 1);
              pfVar12 = pfVar12 + 3;
            } while (puStack_144 < (uint)*(ushort *)(iVar6 + 0x3a));
          }
          uStack_128 = uStack_128 + 1;
          pfVar10 = pfVar10 + 3;
        } while (uStack_128 < *(ushort *)(local_104 + 0x3a));
      }
      if (fStack_13c <= _DAT_005e5050) {
        FUN_00578d90(*(int *)(*param_2 + 0x10) + 0x30,*(int *)(param_2[1] + 0x10) + 0x30,&fStack_100
                     ,param_4);
        fVar13 = (float10)FUN_00578b20(param_2,param_4,&local_140);
      }
      else {
        fVar13 = (float10)fStack_13c;
      }
    }
  }
  fVar14 = (float10)*(float *)(*(int *)(*param_2 + 8) + 0x4c);
  fVar15 = (float10)*(float *)(*(int *)(param_2[1] + 8) + 0x4c);
  fVar16 = (fVar13 - fVar14) - fVar15;
  *param_6 = (float)fVar16;
  if (fVar16 <= (float10)param_3) {
    *param_5 = (float)(((fVar15 - fVar14) + fVar13) * (float10)_DAT_005cc32c + (float10)local_140);
    return 1;
  }
  return 1;
}

