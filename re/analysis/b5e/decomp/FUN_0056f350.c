
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-19] Signature: `void FUN_0056f350(int param_1, float *param_2, float param_3)`. */

void FUN_0056f350(int param_1,float *param_2,float param_3)

{
  float *pfVar1;
  float fVar2;
  undefined4 *puVar3;
  float fVar4;
  float fVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  float *pfVar8;
  float *pfVar9;
  int iVar10;
  float local_f0;
  float local_ec;
  int *local_e4;
  int *local_e0;
  float local_dc;
  float local_d8;
  float local_d4;
  float *local_d0;
  float local_cc;
  float local_c8;
  float local_c4;
  float local_c0;
  float local_bc;
  float local_b8;
  float *local_b4;
  float local_b0;
  float local_ac;
  float local_a8;
  float local_a4;
  float local_a0;
  float local_9c;
  float local_98;
  float local_94;
  float local_90;
  float local_8c;
  float local_88;
  float local_84;
  float local_80;
  float local_7c;
  undefined1 local_78 [64];
  float local_38;
  float local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  float local_20;
  float local_1c;
  undefined4 local_18;
  float local_4;

  local_b4 = (float *)&DAT_005e57a4;
  local_d0 = (float *)&DAT_005e57a4;
  local_7c = *(float *)(&DAT_005e57b0 + (int)param_2[0x2b] * 4) * param_2[0x34];
  if (*(ushort *)(param_2 + 0x2e) == 0xffff) {
    local_e4 = (int *)0x0;
  }
  else {
    local_e4 = *(int **)(*(int *)((int)param_2[0x2d] + 0x10) + 4 +
                        (uint)*(ushort *)(param_2 + 0x2e) * 0xc);
  }
  if (*(ushort *)(param_2 + 0x30) == 0xffff) {
    local_e0 = (int *)0x0;
  }
  else {
    local_e0 = *(int **)(*(int *)((int)param_2[0x2f] + 0x10) + 4 +
                        (uint)*(ushort *)(param_2 + 0x30) * 0xc);
  }
  if (local_e4 != (int *)0x0) {
    local_b4 = (float *)(local_e4[1] * 0x40 + 0x30 + **(int **)(*local_e4 + 0x10));
  }
  if (local_e0 != (int *)0x0) {
    local_d0 = (float *)(**(int **)(*local_e0 + 0x10) + 0x30 + local_e0[1] * 0x40);
  }
  *(undefined4 *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4) = 0;
  *(int *)(param_1 + 0xbc) = *(int *)(param_1 + 0xbc) + 1;
  if (local_e4 == (int *)0x0) {
    iVar10 = -1;
  }
  else {
    iVar10 = (int)(short)local_e4[8];
  }
  *(int *)(*(int *)(param_1 + 0xac) + *(int *)(param_1 + 0xf8) * 8) = iVar10;
  if (local_e0 == (int *)0x0) {
    iVar10 = -1;
  }
  else {
    iVar10 = (int)(short)local_e0[8];
  }
  local_b0 = 0.0;
  *(int *)(*(int *)(param_1 + 0xac) + 4 + *(int *)(param_1 + 0xf8) * 8) = iVar10;
  if (param_2[0x2b] != 0.0) {
    pfVar9 = param_2 + 4;
    pfVar8 = local_d0;
    do {
      puVar3 = (undefined4 *)pfVar9[3];
      local_a0 = 0.0;
      local_9c = 0.0;
      local_98 = 0.0;
      local_ac = 0.0;
      local_a8 = 0.0;
      local_a4 = 0.0;
      if ((puVar3 == (undefined4 *)0x0) || ((*(byte *)(puVar3 + 7) & 8) == 0)) {
        pfVar1 = pfVar9 + -1;
        local_88 = pfVar9[-1] - *local_b4;
        local_84 = *pfVar9 - local_b4[1];
        local_80 = pfVar9[1] - local_b4[2];
        local_94 = *pfVar1 - *pfVar8;
        local_90 = *pfVar9 - pfVar8[1];
        local_8c = pfVar9[1] - pfVar8[2];
        if (local_e4 != (int *)0x0) {
          FUN_0055b750(local_e4,pfVar1,&local_a0);
        }
        if (local_e0 != (int *)0x0) {
          FUN_0055b750(local_e0,pfVar1,&local_ac);
        }
        local_cc = pfVar9[4] + (local_a0 - local_ac);
        local_c8 = pfVar9[5] + (local_9c - local_a8);
        local_c4 = pfVar9[6] + (local_98 - local_a4);
        FUN_0056fad0(local_78,param_2,&local_88,&local_94,local_e4,local_e0);
        if (pfVar9[2] < DAT_005d757c == (pfVar9[2] == DAT_005d757c)) {
          local_38 = 0.0;
        }
        else {
          local_38 = pfVar9[2];
        }
        if ((puVar3 == (undefined4 *)0x0) || ((*(byte *)(puVar3 + 7) & 4) == 0)) {
          fVar2 = param_2[0x32];
          local_30 = 0;
          local_2c = 0;
          local_18 = 0x3f4ccccd;
        }
        else {
          local_30 = *puVar3;
          local_18 = puVar3[3];
          local_2c = puVar3[1];
          fVar2 = (float)puVar3[2];
        }
        local_1c = 3.4028235e+38;
        local_20 = 0.0;
        local_24 = 0x7f7fffff;
        local_28 = 0;
        fVar4 = param_2[2] * local_c4 + local_cc * *param_2 + param_2[1] * local_c8;
        if (param_3 * _DAT_005e456c <= fVar4) {
          local_34 = 0.0;
        }
        else {
          local_34 = -(fVar4 * fVar2);
        }
        FUN_0056f1f0(param_1,local_78);
        pfVar8 = local_d0;
        if ((*(byte *)((int)param_2 + 0xce) & 1) == 0) {
          local_38 = 0.0;
          local_18 = 0;
          local_34 = 0.0;
          local_2c = 0;
          local_24 = 0;
          local_28 = 0;
          if ((puVar3 == (undefined4 *)0x0) || ((*(byte *)(puVar3 + 7) & 1) == 0)) {
            local_f0 = param_2[1] * local_c4 - param_2[2] * local_c8;
            local_ec = param_2[2] * local_cc - local_c4 * *param_2;
            fVar2 = local_c8 * *param_2 - param_2[1] * local_cc;
            if (local_f0 * local_f0 + local_ec * local_ec + fVar2 * fVar2 < _DAT_005cd03c) {
              fVar2 = ABS(*param_2);
              puVar7 = &DAT_005e57c4;
              if (_DAT_005cc9b4 <= fVar2) {
                puVar7 = &DAT_005e57d0;
              }
              puVar6 = &DAT_005e57c4;
              if (_DAT_005cc9b4 <= fVar2) {
                puVar6 = &DAT_005e57d0;
              }
              pfVar8 = (float *)&DAT_005e57c4;
              local_f0 = (float)puVar7[2] * param_2[1] - (float)puVar6[1] * param_2[2];
              if (_DAT_005cc9b4 <= fVar2) {
                pfVar8 = (float *)&DAT_005e57d0;
              }
              puVar7 = &DAT_005e57c4;
              if (_DAT_005cc9b4 <= fVar2) {
                puVar7 = &DAT_005e57d0;
              }
              puVar6 = &DAT_005e57c4;
              local_ec = param_2[2] * *pfVar8 - (float)puVar7[2] * *param_2;
              if (_DAT_005cc9b4 <= fVar2) {
                puVar6 = &DAT_005e57d0;
              }
              pfVar8 = (float *)&DAT_005e57c4;
              if (_DAT_005cc9b4 <= fVar2) {
                pfVar8 = (float *)&DAT_005e57d0;
              }
              fVar2 = (float)puVar6[1] * *param_2 - param_2[1] * *pfVar8;
            }
            fVar4 = fVar2 * fVar2;
            local_f0 = (_DAT_005cc320 / SQRT(local_f0 * local_f0 + local_ec * local_ec + fVar4)) *
                       local_f0;
            local_ec = (_DAT_005cc320 / SQRT(local_f0 * local_f0 + local_ec * local_ec + fVar4)) *
                       local_ec;
            fVar2 = (_DAT_005cc320 / SQRT(local_ec * local_ec + local_f0 * local_f0 + fVar4)) *
                    fVar2;
            fVar5 = param_2[2] * local_ec - param_2[1] * fVar2;
            fVar4 = fVar2 * *param_2 - param_2[2] * local_f0;
            local_4 = param_2[1] * local_f0 - local_ec * *param_2;
            local_dc = (fVar5 + local_f0) * _DAT_005e520c;
            local_d8 = (fVar4 + local_ec) * _DAT_005e520c;
            local_d4 = (local_4 + fVar2) * _DAT_005e520c;
            local_c0 = (local_f0 - fVar5) * _DAT_005e520c;
            local_bc = (local_ec - fVar4) * _DAT_005e520c;
            local_b8 = (fVar2 - local_4) * _DAT_005e520c;
          }
          else {
            local_dc = (float)puVar3[4];
            local_d8 = (float)puVar3[5];
            local_d4 = (float)puVar3[6];
            local_c0 = param_2[2] * local_d8 - param_2[1] * local_d4;
            local_bc = local_d4 * *param_2 - param_2[2] * local_dc;
            local_b8 = param_2[1] * local_dc - local_d8 * *param_2;
          }
          if ((puVar3 == (undefined4 *)0x0) || ((*(byte *)(puVar3 + 7) & 2) == 0)) {
            local_30 = 0;
            local_1c = local_7c * param_2[0x31] * _DAT_005e520c;
            local_20 = local_1c;
          }
          else {
            local_30 = puVar3[9];
            local_1c = (float)puVar3[8];
            local_20 = (float)puVar3[8];
          }
          local_20 = -local_20;
          FUN_0056fad0(local_78,&local_dc,&local_88,&local_94,local_e4,local_e0);
          FUN_0056f1f0(param_1,local_78);
          if ((puVar3 != (undefined4 *)0x0) && ((*(byte *)(puVar3 + 7) & 2) != 0)) {
            local_30 = puVar3[0xb];
            local_1c = (float)puVar3[10];
            local_20 = -(float)puVar3[10];
          }
          FUN_0056fad0(local_78,&local_c0,&local_88,&local_94,local_e4,local_e0);
          FUN_0056f1f0(param_1,local_78);
          pfVar8 = local_d0;
        }
      }
      local_b0 = (float)((int)local_b0 + 1);
      pfVar9 = pfVar9 + 10;
    } while ((uint)local_b0 < (uint)param_2[0x2b]);
  }
  FUN_0056f0a0(param_1);
  return;
}

