
/* [C1 2026-05-20] Signature recovered: `void FUN_00575b60(int bodyB, int otherShape, float
   *out_normal, byte *out_tag)` —… */

void FUN_00575b60(float param_1,int param_2,float *param_3,byte *param_4)

{
  short sVar1;
  int iVar2;
  float *pfVar3;
  int iVar4;
  float fVar5;
  float *pfVar6;

  pfVar6 = param_3;
  fVar5 = param_1;
  iVar2 = *(int *)((int)param_1 + 8);
  sVar1 = *(short *)(iVar2 + 0x48);
  *(short *)param_4 = sVar1;
  if (sVar1 != 0xe) {
    pfVar3 = *(float **)((int)param_1 + 0x10);
    *param_3 = *pfVar3 * *(float *)(iVar2 + 0x30) +
               pfVar3[4] * *(float *)(iVar2 + 0x34) + pfVar3[8] * *(float *)(iVar2 + 0x38);
    iVar4 = *(int *)((int)param_1 + 0x10);
    param_3[1] = *(float *)(iVar4 + 0x14) * *(float *)(iVar2 + 0x34) +
                 *(float *)(iVar4 + 4) * *(float *)(iVar2 + 0x30) +
                 *(float *)(iVar4 + 0x24) * *(float *)(iVar2 + 0x38);
    iVar4 = *(int *)((int)param_1 + 0x10);
    param_3[2] = *(float *)(iVar4 + 0x18) * *(float *)(iVar2 + 0x34) +
                 *(float *)(iVar4 + 8) * *(float *)(iVar2 + 0x30) +
                 *(float *)(iVar4 + 0x28) * *(float *)(iVar2 + 0x38);
    if ((*param_4 & 1) == 0) {
      FUN_0055c2d0(*(undefined4 *)(param_2 + 8),*(undefined4 *)(param_2 + 0x10),param_3,&param_4,
                   &param_1);
      iVar4 = *(int *)((int)fVar5 + 0x10);
      fVar5 = *(float *)(iVar4 + 0x38) * pfVar6[2] +
              *(float *)(iVar4 + 0x30) * *pfVar6 + *(float *)(iVar4 + 0x34) * pfVar6[1] +
              *(float *)(iVar2 + 0x18) * *(float *)(iVar2 + 0x38) +
              *(float *)(iVar2 + 0x14) * *(float *)(iVar2 + 0x34) +
              *(float *)(iVar2 + 0x10) * *(float *)(iVar2 + 0x30);
      if (param_1 - fVar5 < fVar5 - (float)param_4) {
        *pfVar6 = -*pfVar6;
        pfVar6[1] = -pfVar6[1];
        pfVar6[2] = -pfVar6[2];
      }
    }
  }
  return;
}

