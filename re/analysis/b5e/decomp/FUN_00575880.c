
/* [C1 2026-05-20] Signature recovered: `int FUN_00575880(World *world, int p2, int *body_pair_ptrs,
   float depth)` —… */

undefined4 FUN_00575880(float param_1,int param_2,int *param_3,float param_4)

{
  float *pfVar1;
  short sVar2;
  undefined4 *puVar3;
  int iVar4;
  int iVar5;
  float *pfVar6;
  int iVar7;
  float fVar8;
  int iVar9;
  uint uVar10;
  int *piVar11;
  int iVar12;

  piVar11 = param_3;
  iVar4 = (int)param_1;
  if ((*(uint *)((int)param_1 + 0x40) < *(int *)((int)param_1 + 0x3c) + 0x10U) ||
     (*(uint *)((int)param_1 + 0x34) <= *(uint *)((int)param_1 + 0x30))) {
    *(undefined4 *)((int)param_1 + 0x78) = 1;
  }
  else {
    iVar12 = *(uint *)((int)param_1 + 0x30) * 0x120 + *(int *)((int)param_1 + 0x2c);
    *(int *)(iVar12 + 0x50) = *(int *)((int)param_1 + 0x3c) * 0x10 + *(int *)((int)param_1 + 0x38);
    param_1 = *(float *)((int)param_1 + 0x74);
    if (((*(uint *)(*(int *)(*(int *)(param_3[1] + 8) + 0x5c) + 0x40) |
         *(uint *)(*(int *)(*(int *)(*param_3 + 8) + 0x5c) + 0x40)) & 0x10) == 0) {
      iVar9 = FUN_00578e50(*(undefined4 *)(*(int *)(param_2 + 0x70) + 4),param_3,param_4,iVar12,
                           param_1);
    }
    else {
      iVar9 = FUN_0057adb0(*(undefined4 *)(*(int *)(param_2 + 0x70) + 4),param_3,param_4,iVar12,
                           param_1);
    }
    if ((iVar9 != 0) &&
       (*(float *)(iVar12 + 0x110) < param_4 != (*(float *)(iVar12 + 0x110) == param_4))) {
      uVar10 = *(int *)(iVar4 + 0x30) + 1;
      *(uint *)(iVar4 + 0x30) = uVar10;
      if (*(uint *)(iVar4 + 0x34) <= uVar10) {
        *(undefined4 *)(iVar4 + 0x78) = 1;
      }
      if (4 < *(ushort *)(iVar12 + 0x54)) {
        *(uint *)(iVar4 + 0x3c) = *(int *)(iVar4 + 0x3c) + (uint)*(ushort *)(iVar12 + 0x54);
      }
      if (4 < *(ushort *)(iVar12 + 0xd4)) {
        *(uint *)(iVar4 + 0x3c) = *(int *)(iVar4 + 0x3c) + (uint)*(ushort *)(iVar12 + 0xd4);
      }
      puVar3 = (undefined4 *)*piVar11;
      *(undefined4 *)(iVar12 + 0x58) = *puVar3;
      *(undefined4 *)(iVar12 + 0x5c) = puVar3[1];
      *(undefined4 *)(iVar12 + 0x6c) = puVar3[3];
      uVar10 = *(uint *)(*(int *)(puVar3[2] + 0x5c) + 0x40);
      *(short *)(iVar12 + 0x7c) = (short)uVar10;
      *(undefined4 *)(iVar12 + 0x78) = *(undefined4 *)(puVar3[2] + 0x4c);
      *(undefined4 *)(iVar12 + 0x70) = *(undefined4 *)(puVar3[2] + 0x54);
      *(undefined4 *)(iVar12 + 0x74) = *(undefined4 *)(puVar3[2] + 0x50);
      *(undefined2 *)(iVar12 + 0x7e) = 0;
      if ((uVar10 & 0x40) != 0) {
        iVar4 = piVar11[1];
        iVar9 = *piVar11;
        pfVar1 = (float *)(iVar12 + 0x60);
        iVar5 = *(int *)(iVar9 + 8);
        sVar2 = *(short *)(iVar5 + 0x48);
        *(short *)(iVar12 + 0x7e) = sVar2;
        piVar11 = param_3;
        if (sVar2 != 0xe) {
          pfVar6 = *(float **)(iVar9 + 0x10);
          *pfVar1 = *pfVar6 * *(float *)(iVar5 + 0x30) +
                    pfVar6[4] * *(float *)(iVar5 + 0x34) + pfVar6[8] * *(float *)(iVar5 + 0x38);
          iVar7 = *(int *)(iVar9 + 0x10);
          *(float *)(iVar12 + 100) =
               *(float *)(iVar7 + 0x14) * *(float *)(iVar5 + 0x34) +
               *(float *)(iVar7 + 4) * *(float *)(iVar5 + 0x30) +
               *(float *)(iVar7 + 0x24) * *(float *)(iVar5 + 0x38);
          iVar7 = *(int *)(iVar9 + 0x10);
          *(float *)(iVar12 + 0x68) =
               *(float *)(iVar7 + 0x18) * *(float *)(iVar5 + 0x34) +
               *(float *)(iVar7 + 8) * *(float *)(iVar5 + 0x30) +
               *(float *)(iVar7 + 0x28) * *(float *)(iVar5 + 0x38);
          if ((*(byte *)(iVar12 + 0x7e) & 1) == 0) {
            FUN_0055c2d0(*(undefined4 *)(iVar4 + 8),*(undefined4 *)(iVar4 + 0x10),pfVar1,&param_1,
                         &param_4);
            iVar4 = *(int *)(iVar9 + 0x10);
            fVar8 = *(float *)(iVar4 + 0x38) * *(float *)(iVar12 + 0x68) +
                    *(float *)(iVar4 + 0x30) * *pfVar1 +
                    *(float *)(iVar4 + 0x34) * *(float *)(iVar12 + 100) +
                    *(float *)(iVar5 + 0x18) * *(float *)(iVar5 + 0x38) +
                    *(float *)(iVar5 + 0x10) * *(float *)(iVar5 + 0x30) +
                    *(float *)(iVar5 + 0x14) * *(float *)(iVar5 + 0x34);
            piVar11 = param_3;
            if (param_4 - fVar8 < fVar8 - param_1) {
              *pfVar1 = -*pfVar1;
              *(float *)(iVar12 + 100) = -*(float *)(iVar12 + 100);
              *(float *)(iVar12 + 0x68) = -*(float *)(iVar12 + 0x68);
            }
          }
        }
      }
      puVar3 = (undefined4 *)piVar11[1];
      *(undefined4 *)(iVar12 + 0xd8) = *puVar3;
      *(undefined4 *)(iVar12 + 0xdc) = puVar3[1];
      *(undefined4 *)(iVar12 + 0xec) = puVar3[3];
      uVar10 = *(uint *)(*(int *)(puVar3[2] + 0x5c) + 0x40);
      *(short *)(iVar12 + 0xfc) = (short)uVar10;
      *(undefined4 *)(iVar12 + 0xf8) = *(undefined4 *)(puVar3[2] + 0x4c);
      *(undefined4 *)(iVar12 + 0xf0) = *(undefined4 *)(puVar3[2] + 0x54);
      *(undefined4 *)(iVar12 + 0xf4) = *(undefined4 *)(puVar3[2] + 0x50);
      *(undefined2 *)(iVar12 + 0xfe) = 0;
      if ((uVar10 & 0x40) != 0) {
        FUN_00575b60(piVar11[1],*piVar11,iVar12 + 0xe0,iVar12 + 0xfe);
      }
      return 1;
    }
  }
  return 0;
}

