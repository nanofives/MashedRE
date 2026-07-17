
/* [C1 2026-05-19] Signature: `void FUN_005685f0(int param_1, uint *param_2, uint *param_3,
   undefined4 *param_4, undefined4 *param_5)`. */

void FUN_005685f0(int param_1,uint *param_2,uint *param_3,undefined4 *param_4,undefined4 *param_5)

{
  uint uVar1;
  uint *puVar2;
  int iVar3;
  int iVar4;
  uint *puVar5;
  uint *puVar6;

  if (param_2 == param_3) {
    if (param_4 != (undefined4 *)0x0) {
      *param_4 = 0;
      if (param_2[4] == 0) {
        param_2[6] = (uint)param_4;
        param_2[5] = (uint)param_4;
        param_2[4] = 1;
      }
      else {
        *(undefined4 **)param_2[6] = param_4;
        param_2[6] = (uint)param_4;
        param_2[4] = param_2[4] + 1;
      }
    }
    if (param_5 != (undefined4 *)0x0) {
      *param_5 = 0;
      if (param_2[7] != 0) {
        *(undefined4 **)param_2[9] = param_5;
        param_2[9] = (uint)param_5;
        param_2[7] = param_2[7] + 1;
        return;
      }
      param_2[7] = 1;
      param_2[9] = (uint)param_5;
      param_2[8] = (uint)param_5;
      return;
    }
  }
  else {
    uVar1 = *param_3;
    puVar2 = param_2;
    puVar5 = param_3;
    if ((*param_2 <= uVar1) && (puVar2 = param_3, puVar5 = param_2, uVar1 == *param_2)) {
      *param_3 = uVar1 + 1;
    }
    *(uint *)puVar5[2] = puVar2[2];
    *(undefined4 *)(puVar5[2] + 4) = 0;
    *(uint *)(puVar2[3] + 8) = puVar5[2];
    puVar2[3] = puVar5[3];
    puVar2[1] = puVar2[1] + puVar5[1];
    if (puVar5[4] != 0) {
      if (puVar2[4] == 0) {
        puVar2[5] = puVar5[5];
        puVar2[6] = puVar5[6];
        puVar2[4] = puVar5[4];
      }
      else {
        *(uint *)puVar2[6] = puVar5[5];
        puVar2[6] = puVar5[6];
        puVar2[4] = puVar2[4] + puVar5[4];
      }
    }
    if (puVar5[7] != 0) {
      if (puVar2[7] == 0) {
        puVar2[8] = puVar5[8];
        puVar2[9] = puVar5[9];
        puVar2[7] = puVar5[7];
      }
      else {
        *(uint *)puVar2[9] = puVar5[8];
        puVar2[9] = puVar5[9];
        puVar2[7] = puVar2[7] + puVar5[7];
      }
    }
    if (param_4 != (undefined4 *)0x0) {
      *param_4 = 0;
      if (puVar2[4] == 0) {
        puVar2[6] = (uint)param_4;
        puVar2[5] = (uint)param_4;
        puVar2[4] = 1;
      }
      else {
        *(undefined4 **)puVar2[6] = param_4;
        puVar2[6] = (uint)param_4;
        puVar2[4] = puVar2[4] + 1;
      }
    }
    if (param_5 != (undefined4 *)0x0) {
      *param_5 = 0;
      if (puVar2[7] == 0) {
        puVar2[9] = (uint)param_5;
        puVar2[8] = (uint)param_5;
        puVar2[7] = 1;
      }
      else {
        *(undefined4 **)puVar2[9] = param_5;
        puVar2[9] = (uint)param_5;
        puVar2[7] = puVar2[7] + 1;
      }
    }
    iVar3 = *(int *)(param_1 + 4) + -1;
    *(int *)(param_1 + 4) = iVar3;
    if (iVar3 != 0) {
      puVar2 = (uint *)(param_1 + 8 + iVar3 * 0x28);
      puVar6 = puVar5;
      for (iVar4 = 10; iVar4 != 0; iVar4 = iVar4 + -1) {
        *puVar6 = *puVar2;
        puVar2 = puVar2 + 1;
        puVar6 = puVar6 + 1;
      }
      *(uint **)(puVar5[2] + 4) = puVar5;
    }
  }
  return;
}

