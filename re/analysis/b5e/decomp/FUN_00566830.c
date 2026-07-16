
/* [C1 2026-05-19] Signature: `void FUN_00566830(float *param_1, float *param_2)`. */

void FUN_00566830(float *param_1,float *param_2)

{
  float fVar1;
  uint uVar2;
  uint uVar3;

  *param_1 = ABS(*param_2);
  param_1[1] = ABS(param_2[1]);
  fVar1 = ABS(param_2[2]);
  param_1[2] = fVar1;
  uVar2 = 0x21312300 >>
          ((((param_1[1] < *param_1) * '\x02' | fVar1 < *param_1) * '\x02' | fVar1 < param_1[1]) <<
          2) & 3;
  param_1[uVar2] = 0.0;
  uVar2 = 1 << (sbyte)uVar2 & 3;
  uVar3 = 1 << (sbyte)uVar2 & 3;
  param_1[uVar2] = param_2[uVar3];
  param_1[uVar3] = -param_2[uVar2];
  return;
}

