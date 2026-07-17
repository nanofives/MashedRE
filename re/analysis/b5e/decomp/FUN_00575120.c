
/* [C1 2026-05-20] Signature recovered: `int FUN_00575120(float *contact_xyz, uint feature_id, int
   manifold, float… */

undefined4 FUN_00575120(float *param_1,undefined4 param_2,int param_3,float param_4)

{
  int iVar1;
  uint uVar2;
  float fVar3;

  uVar2 = *(uint *)(param_3 + 0xac);
  if ((uVar2 != 0) && (DAT_005d757c <= param_4)) {
    fVar3 = ABS(*(float *)(param_3 + 0xc) - *param_1) +
            ABS(*(float *)(param_3 + 0x14) - param_1[2]) +
            ABS(*(float *)(param_3 + 0x10) - param_1[1]);
    if (fVar3 < param_4 != (fVar3 == param_4)) {
      return 0;
    }
    if ((1 < uVar2) &&
       (iVar1 = param_3 + uVar2 * 0x28,
       fVar3 = ABS(*(float *)(iVar1 + -0x14) - param_1[2]) +
               ABS(*(float *)(param_3 + -0x1c + uVar2 * 0x28) - *param_1) +
               ABS(*(float *)(iVar1 + -0x18) - param_1[1]), fVar3 < param_4 != (fVar3 == param_4)))
    {
      return 0;
    }
  }
  iVar1 = param_3 + uVar2 * 0x28;
  *(float *)(iVar1 + 0xc) = *param_1;
  *(float *)(iVar1 + 0x10) = param_1[1];
  fVar3 = param_1[2];
  *(undefined4 *)(iVar1 + 0x1c) = 0;
  *(float *)(iVar1 + 0x14) = fVar3;
  *(undefined4 *)(iVar1 + 0x18) = param_2;
  *(int *)(param_3 + 0xac) = *(int *)(param_3 + 0xac) + 1;
  return 1;
}

