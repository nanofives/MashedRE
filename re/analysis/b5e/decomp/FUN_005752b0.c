
/* [C1 2026-05-20] Signature recovered: `int FUN_005752b0(int pair, float depth_tolerance, float
   *out_manifold_record)` —… */

undefined4 FUN_005752b0(int param_1,float param_2,float *param_3)

{
  short sVar1;
  short sVar2;
  ushort uVar3;
  int iVar4;
  float *pfVar5;
  float fVar6;
  int local_114;
  int local_110;
  float local_10c;
  float local_108;
  float local_104;
  undefined1 local_100 [256];

  local_10c = *(float *)(param_1 + 0x78) + *(float *)(param_1 + 0xf8);
  sVar1 = *(short *)(param_1 + 0x54);
  sVar2 = *(short *)(param_1 + 0xd4);
  local_104 = *(float *)(param_1 + 0xf8) - *(float *)(param_1 + 0x78);
  local_108 = *(float *)(param_1 + 0x110) + local_10c;
  if (sVar1 == 1) {
    local_114 = 0;
  }
  else {
    local_114 = (sVar1 != 2) + 1;
  }
  if (sVar2 == 1) {
    local_110 = 0;
  }
  else {
    local_110 = (sVar2 != 2) + 1;
  }
  iVar4 = FUN_00576880(*(undefined4 *)(param_1 + 0x50),sVar1,*(undefined4 *)(param_1 + 0xd0),sVar2,
                       local_104,*(undefined4 *)(param_1 + 0x10c),(float *)(param_1 + 0x100),
                       &local_108,&local_114,&local_110,param_1,local_100);
  if ((local_108 - local_10c <= param_2) && (iVar4 != 0)) {
    *param_3 = *(float *)(param_1 + 0x100);
    param_3[1] = *(float *)(param_1 + 0x104);
    param_3[2] = *(float *)(param_1 + 0x108);
    param_3[0x2d] = *(float *)(param_1 + 0x58);
    param_3[0x2e] = *(float *)(param_1 + 0x5c);
    param_3[0x2f] = *(float *)(param_1 + 0xd8);
    param_3[0x30] = *(float *)(param_1 + 0xdc);
    if (*(float *)(param_1 + 0xf0) < *(float *)(param_1 + 0x70)) {
      fVar6 = *(float *)(param_1 + 0xf0);
    }
    else {
      fVar6 = *(float *)(param_1 + 0x70);
    }
    param_3[0x31] = fVar6;
    if (*(float *)(param_1 + 0xf4) < *(float *)(param_1 + 0x74)) {
      fVar6 = *(float *)(param_1 + 0xf4);
    }
    else {
      fVar6 = *(float *)(param_1 + 0x74);
    }
    *(undefined1 *)(param_3 + 0x33) = (undefined1)local_114;
    *(undefined1 *)((int)param_3 + 0xcd) = (undefined1)local_110;
    param_3[0x32] = fVar6;
    *(undefined2 *)((int)param_3 + 0xce) = 0;
    param_3[0x2b] = 0.0;
    FUN_00574ad0(param_1,local_100,iVar4,param_2,local_10c,param_3);
    if (param_3[0x2b] != 0.0) {
      fVar6 = 0.0;
      if (param_3[0x2b] != 0.0) {
        pfVar5 = param_3 + 0xc;
        do {
          fVar6 = (float)((int)fVar6 + 1);
          pfVar5[-1] = *(float *)(param_1 + 0x6c);
          *pfVar5 = *(float *)(param_1 + 0xec);
          pfVar5[-2] = 0.0;
          pfVar5[-3] = 0.0;
          pfVar5[-4] = 0.0;
          pfVar5 = pfVar5 + 10;
        } while ((uint)fVar6 < (uint)param_3[0x2b]);
      }
      if (((*(byte *)(param_1 + 0x7c) & 0x40) == 0) ||
         (uVar3 = *(ushort *)(param_1 + 0x7e), uVar3 == 0xe)) {
        if ((((*(byte *)(param_1 + 0xfc) & 0x40) != 0) &&
            (uVar3 = *(ushort *)(param_1 + 0xfe), uVar3 != 0xe)) &&
           (((~uVar3 & *(ushort *)(param_1 + 0xd6)) != 0 || ((local_110 == 2 && ((uVar3 & 1) != 0)))
            ))) {
          *param_3 = *(float *)(param_1 + 0xe0);
          param_3[1] = *(float *)(param_1 + 0xe4);
          param_3[2] = *(float *)(param_1 + 0xe8);
        }
      }
      else if (((~uVar3 & *(ushort *)(param_1 + 0x56)) != 0) ||
              ((local_114 == 2 && ((uVar3 & 1) != 0)))) {
        *param_3 = -*(float *)(param_1 + 0x60);
        param_3[1] = -*(float *)(param_1 + 100);
        param_3[2] = -*(float *)(param_1 + 0x68);
        return 1;
      }
      return 1;
    }
  }
  return 0;
}

