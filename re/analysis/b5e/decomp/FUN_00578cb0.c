
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `float10 FUN_00578cb0(int *body_pair, float *face_record_B,
   float *out_axis, float… */

float10 FUN_00578cb0(int *param_1,float *param_2,float *param_3,float *param_4)

{
  int iVar1;
  float fVar2;
  float fVar3;
  float10 fVar4;
  float local_8;
  float local_4;

  FUN_0055c2d0(*(undefined4 *)(*param_1 + 8),*(undefined4 *)(*param_1 + 0x10),param_2,&local_8,
               &local_4);
  iVar1 = *(int *)(param_1[1] + 0x10);
  fVar2 = *(float *)(iVar1 + 0x38) * param_2[2] +
          *(float *)(iVar1 + 0x30) * *param_2 + *(float *)(iVar1 + 0x34) * param_2[1];
  fVar3 = fVar2 + param_2[3];
  fVar2 = fVar2 + param_2[4];
  fVar4 = (float10)local_8 - (float10)fVar2;
  local_4 = fVar3 - local_4;
  if ((float10)local_4 < fVar4) {
    *param_4 = fVar2;
    *param_3 = *param_2;
    param_3[1] = param_2[1];
    param_3[2] = param_2[2];
    return fVar4;
  }
  *param_3 = *param_2 * _DAT_005cc33c;
  param_3[1] = param_2[1] * _DAT_005cc33c;
  param_3[2] = param_2[2] * _DAT_005cc33c;
  *param_4 = -fVar3;
  return (float10)local_4;
}

