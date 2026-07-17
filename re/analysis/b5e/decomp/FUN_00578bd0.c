
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `float10 FUN_00578bd0(int *body_pair, float *face_record_A,
   float *out_axis, float… */

float10 FUN_00578bd0(int *param_1,float *param_2,float *param_3,float *param_4)

{
  float fVar1;
  float fVar2;
  int iVar3;
  float fVar4;
  float *pfVar5;
  float10 fVar6;

  pfVar5 = param_2;
  iVar3 = *(int *)(*param_1 + 0x10);
  fVar4 = *(float *)(iVar3 + 0x38) * param_2[2] +
          *(float *)(iVar3 + 0x30) * *param_2 + *(float *)(iVar3 + 0x34) * param_2[1];
  fVar1 = param_2[3];
  fVar2 = param_2[4];
  FUN_0055c2d0(*(undefined4 *)(param_1[1] + 8),*(undefined4 *)(param_1[1] + 0x10),param_2,&param_2,
               &param_1);
  fVar6 = (float10)(fVar4 + fVar1) - (float10)(float)param_1;
  fVar1 = (float)param_2 - (fVar4 + fVar2);
  if ((float10)fVar1 < fVar6) {
    *param_4 = (float)param_1;
    *param_3 = *pfVar5;
    param_3[1] = pfVar5[1];
    param_3[2] = pfVar5[2];
    return fVar6;
  }
  *param_3 = *pfVar5 * _DAT_005cc33c;
  param_3[1] = pfVar5[1] * _DAT_005cc33c;
  param_3[2] = pfVar5[2] * _DAT_005cc33c;
  *param_4 = -(float)param_2;
  return (float10)fVar1;
}

