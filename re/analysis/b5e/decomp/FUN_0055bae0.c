
/* [C1 2026-05-19] Calls `pfVar10 = (float *)(**(code **)(*(int *)(param_1 + 0x5c) + 8))(param_1,
   param_3)` (cited 0x0055bae9) —… */

void FUN_0055bae0(int param_1,float *param_2,undefined4 param_3)

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

  pfVar10 = (float *)(**(code **)(*(int *)(param_1 + 0x5c) + 8))(param_1,param_3);
  if ((pfVar10 != (float *)0x0) && (param_2 != (float *)0x0)) {
    fVar1 = param_2[9];
    fVar2 = param_2[1];
    fVar3 = *pfVar10;
    fVar4 = param_2[5];
    fVar5 = param_2[10];
    fVar6 = param_2[2];
    fVar7 = *pfVar10;
    fVar8 = param_2[6];
    fVar9 = pfVar10[1];
    *pfVar10 = param_2[0xc] +
               *param_2 * *pfVar10 + param_2[4] * pfVar10[1] + param_2[8] * pfVar10[2];
    pfVar10[1] = param_2[0xd] + fVar4 * pfVar10[1] + fVar2 * fVar3 + fVar1 * pfVar10[2];
    pfVar10[2] = param_2[0xe] + fVar8 * fVar9 + fVar6 * fVar7 + fVar5 * pfVar10[2];
  }
  return;
}

