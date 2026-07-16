
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] Signature recovered: `float10 FUN_00578b20(int *body_pair, float *axis, float
   *out_offset)` —… */

float10 FUN_00578b20(int *param_1,float *param_2,float *param_3)

{
  int *piVar1;
  float *pfVar2;
  float local_8;
  float local_4;

  pfVar2 = param_2;
  piVar1 = param_1;
  FUN_0055c2d0(*(undefined4 *)(*param_1 + 8),*(undefined4 *)(*param_1 + 0x10),param_2,&local_8,
               &local_4);
  FUN_0055c2d0(*(undefined4 *)(piVar1[1] + 8),*(undefined4 *)(piVar1[1] + 0x10),pfVar2,&param_2,
               &param_1);
  if ((float10)((float)param_2 - local_4) < (float10)local_8 - (float10)(float)param_1) {
    *param_3 = (float)param_1;
    return (float10)local_8 - (float10)(float)param_1;
  }
  *pfVar2 = *pfVar2 * _DAT_005cc33c;
  pfVar2[1] = pfVar2[1] * _DAT_005cc33c;
  pfVar2[2] = pfVar2[2] * _DAT_005cc33c;
  *param_3 = -(float)param_2;
  return (float10)((float)param_2 - local_4);
}

