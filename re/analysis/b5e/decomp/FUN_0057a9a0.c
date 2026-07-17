
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `int FUN_0057a9a0(undefined4 unused, int *body_pair, float
   merge_depth, float… */

undefined4
FUN_0057a9a0(undefined4 param_1,int *param_2,float param_3,float *param_4,float *param_5,
            float *param_6,float param_7)

{
  int iVar1;
  bool bVar2;
  float fVar3;
  int iVar4;
  float10 fVar5;
  float local_200;
  float local_1fc;
  float local_1f8;
  float local_1f4;
  float local_1f0;
  float local_1ec;
  float local_1e8;
  int local_1e4;
  float local_1e0;
  float local_1dc;
  float local_1d8;
  float local_1d4;
  float local_1d0;
  float local_1cc;
  uint local_1c8;
  float local_1c4;
  float local_1c0;
  float local_1bc;
  float local_1b8;
  float local_1b4;
  undefined1 local_1b0 [432];

  iVar4 = *(int *)(*param_2 + 8);
  iVar1 = *(int *)(param_2[1] + 8);
  local_1f0 = *(float *)(iVar4 + 0x4c);
  local_1e8 = *(float *)(iVar1 + 0x4c);
  if (**(short **)(iVar4 + 0x5c) == 1) {
    *param_4 = -*(float *)(*(int *)(*param_2 + 0x10) + 0x20);
    param_4[1] = -*(float *)(*(int *)(*param_2 + 0x10) + 0x24);
    param_4[2] = -*(float *)(*(int *)(*param_2 + 0x10) + 0x28);
    FUN_0055c000(*(undefined4 *)(param_2[1] + 8),*(undefined4 *)(param_2[1] + 0x10),param_4,
                 &local_1e0);
    iVar4 = *(int *)(*param_2 + 0x10);
    fVar3 = param_4[2] * local_1d8 + local_1e0 * *param_4 + param_4[1] * local_1dc + local_1e8;
    local_1e8 = (*(float *)(iVar4 + 0x38) * param_4[2] +
                *(float *)(iVar4 + 0x30) * *param_4 + *(float *)(iVar4 + 0x34) * param_4[1]) -
                local_1f0;
  }
  else {
    if (**(short **)(iVar1 + 0x5c) != 1) {
      fVar5 = (float10)FUN_0057a660(param_2,&local_1d4);
      local_1f8 = (float)fVar5;
      local_1fc = param_7 * _DAT_005cc328;
      if (local_1fc < local_1f8) {
        local_1fc = local_1fc + local_1f8;
      }
      local_1c4 = local_1e8 + local_1f0;
      local_1ec = 0.0;
      local_200 = 0.0;
      local_1c0 = local_1c4 + param_3;
      local_1e4 = 0;
      local_1c8 = 0;
      do {
        local_1e0 = local_1d4 * local_200;
        local_1dc = local_1d0 * local_200;
        local_1d8 = local_1cc * local_200;
        iVar4 = FUN_0057a250(param_2,&local_1e0,local_1c0 + local_200,local_1b0,&local_1bc,
                             &local_1f8,&local_1f4,param_7);
        if ((iVar4 == 0) || (local_1f4 <= DAT_005d757c)) {
          local_1ec = local_200;
          if (local_1e4 == 0) {
            if (local_200 == local_1fc) {
              local_1fc = local_1fc + param_7;
            }
            local_200 = local_1fc;
          }
          else {
            local_200 = (local_200 + local_1fc) * _DAT_005cc32c;
          }
        }
        else {
          local_1fc = local_200;
          local_1e4 = 1;
          bVar2 = local_200 == DAT_005d757c;
          *param_4 = local_1bc;
          param_4[1] = local_1b8;
          param_4[2] = local_1b4;
          *param_5 = local_1f8;
          *param_6 = local_1f4;
          if (bVar2) goto LAB_0057ad7f;
          if (local_1f4 < param_3) goto LAB_0057ad3c;
          local_1f8 = param_4[2] * local_1cc + local_1d4 * *param_4 + param_4[1] * local_1d0;
          if (local_200 <= local_1f4 / local_1f8) {
            local_200 = local_1ec * _DAT_005cc9dc + local_200 * _DAT_005cc9a0;
          }
          else {
            local_200 = local_200 - (local_1f4 / local_1f8) * _DAT_005cc9dc;
          }
          if (local_200 < local_1ec) {
            local_1ec = local_200;
          }
        }
        local_1c8 = local_1c8 + 1;
      } while (local_1c8 < 4);
      if (local_1e4 == 0) {
        return 0;
      }
LAB_0057ad3c:
      if (local_1fc != DAT_005d757c) {
        local_1fc = (param_4[2] * local_1cc + local_1d4 * *param_4 + param_4[1] * local_1d0) *
                    local_1fc;
        *param_5 = *param_5 - _DAT_005cc32c * local_1fc;
        *param_6 = *param_6 - local_1fc;
      }
LAB_0057ad7f:
      *param_5 = *param_5 - (local_1f0 - local_1e8) * _DAT_005cc32c;
      *param_6 = *param_6 - local_1c4;
      return 1;
    }
    iVar4 = *(int *)(param_2[1] + 0x10);
    *param_4 = *(float *)(iVar4 + 0x20);
    param_4[1] = *(float *)(iVar4 + 0x24);
    param_4[2] = *(float *)(iVar4 + 0x28);
    FUN_0055c000(*(undefined4 *)(*param_2 + 8),*(undefined4 *)(*param_2 + 0x10),param_4,&local_1e0);
    iVar4 = *(int *)(param_2[1] + 0x10);
    fVar3 = param_4[2] * local_1d8 + local_1e0 * *param_4 + param_4[1] * local_1dc + local_1f0;
    local_1e8 = (*(float *)(iVar4 + 0x38) * param_4[2] +
                *(float *)(iVar4 + 0x30) * *param_4 + *(float *)(iVar4 + 0x34) * param_4[1]) -
                local_1e8;
  }
  *param_5 = (local_1e8 + fVar3) * _DAT_005cc32c;
  *param_6 = local_1e8 - fVar3;
  return 1;
}

