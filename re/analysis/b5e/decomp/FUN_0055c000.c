
/* [C1 2026-05-19] If `param_2 != 0`: rotates `param_3` by 3x3 from `param_2[0..10]` into
   `local_18/14/10` (cited 0x0055c01b..0x0055c046)… */

void FUN_0055c000(int param_1,float *param_2,float *param_3,undefined4 param_4)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float *pfVar10;
  float local_18;
  float local_14;
  float local_10;

  if (param_2 != (float *)0x0) {
    local_18 = param_2[2] * param_3[2] + *param_2 * *param_3 + param_2[1] * param_3[1];
    local_14 = param_2[6] * param_3[2] + param_2[4] * *param_3 + param_2[5] * param_3[1];
    local_10 = param_2[10] * param_3[2] + param_2[8] * *param_3 + param_2[9] * param_3[1];
    param_3 = &local_18;
  }
  pfVar10 = (float *)(**(code **)(*(int *)(param_1 + 0x5c) + 0x14))(param_1,param_3,param_4);
  if ((pfVar10 != (float *)0x0) && (param_2 != (float *)0x0)) {
    fVar1 = param_2[9];
    fVar2 = param_2[5];
    fVar3 = param_2[1];
    fVar4 = *pfVar10;
    fVar5 = param_2[10];
    fVar6 = param_2[6];
    fVar7 = pfVar10[1];
    fVar8 = param_2[2];
    fVar9 = *pfVar10;
    *pfVar10 = param_2[0xc] +
               *param_2 * *pfVar10 + param_2[4] * pfVar10[1] + param_2[8] * pfVar10[2];
    pfVar10[1] = param_2[0xd] + fVar3 * fVar4 + fVar2 * pfVar10[1] + fVar1 * pfVar10[2];
    pfVar10[2] = param_2[0xe] + fVar8 * fVar9 + fVar6 * fVar7 + fVar5 * pfVar10[2];
  }
  return;
}

