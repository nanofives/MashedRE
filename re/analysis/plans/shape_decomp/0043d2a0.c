
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0043d2a0(int param_1,int param_2)

{
  undefined *puVar1;
  short sVar2;
  short sVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  int iVar6;
  bool bVar7;
  float10 extraout_ST0;
  int local_1c;
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  if (param_2 == 0) {
    FUN_00431f30(param_1);
  }
  else if (param_2 == 1) {
    if (-1 < DAT_0067e9f8 + -2) {
      FUN_00431f30((&DAT_0067ed7c)[(DAT_0067e9f8 + -2) * 0x10]);
    }
  }
  else if (param_2 == 2) {
    FUN_00431d90();
  }
  FUN_00472640(0xff);
  if (param_1 == 0x1a) {
    DAT_007f0ef4 = DAT_007f0eec;
    DAT_007f0ef8 = DAT_007f0ef0;
  }
  else if (param_1 == 0x13) {
    DAT_007f0f14 = DAT_007f0f00;
    DAT_007f0f24 = DAT_007f0f10;
    DAT_007f0f20 = DAT_007f0f0c;
    DAT_007f0f1c = DAT_007f0f08;
    DAT_007f0f18 = DAT_007f0f04;
  }
  else if (param_1 == 0x20) {
    DAT_008990d8 = FUN_0040ad20();
  }
  else if (param_1 == 0x1c) {
    puVar5 = &DAT_00899160;
    puVar4 = &DAT_007f105c;
    do {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 0x13;
      puVar5 = puVar5 + 1;
    } while ((int)puVar4 < 0x7f13ec);
  }
  else {
    if ((param_1 == 0x1f) || (param_1 == 0x21)) {
      DAT_007f0f44 = DAT_007f0f30;
      DAT_007f0f48 = DAT_007f0f34;
      DAT_007f0f4c = DAT_007f0f38;
      DAT_007f0f50 = DAT_007f0f3c;
    }
    if (param_1 == 1) {
      FUN_00414120();
      FUN_00422b30();
      FUN_0040b810();
      DAT_0067ea70 = 1;
    }
  }
  FUN_0042d3e0();
  iVar6 = DAT_0067e9f8;
  local_4 = 0;
  local_10 = 0;
  if (param_2 == 0) {
    (&DAT_0067ed78)[DAT_0067e9f8 * 0x10] = (&PTR_DAT_005f7638)[param_1];
    (&DAT_0067ed7c)[iVar6 * 0x10] = param_1;
    (&DAT_0067ed80)[iVar6 * 0x10] = 0;
    FUN_00432800(iVar6);
    local_1c = DAT_0067e9f8 * 0x40;
  }
  else if (param_2 == 2) {
    local_1c = DAT_0067e9f8 * 0x40;
  }
  else {
    DAT_0067e9f8 = DAT_0067e9f8 + -1;
    if (DAT_0067e9f8 < 1) {
      return;
    }
    local_1c = DAT_0067e9f8 * 0x40;
  }
  sVar2 = FUN_0042ad90();
  sVar3 = FUN_0042ad90();
  if ((sVar2 != -1) || (sVar3 != -1)) {
    DAT_00898acc = 0xff;
    DAT_00898acd = 0xff;
    DAT_00898ace = 0xff;
    DAT_00898ae8 = (int)sVar2;
    DAT_00898aec = (int)sVar3;
    DAT_00898ac0 = 0xff000000;
    DAT_00898ad8 = 0x42800000;
    DAT_00898adc = 0x42400000;
    DAT_00898ad4 = 0x3f19999a;
    DAT_00898ae4 = 0;
    if (sVar3 == -1) {
      DAT_00898ac4 = 1;
      DAT_00898ad0 = 0;
      local_10 = 1;
    }
    else {
      DAT_00898ad0 = 0x1ff;
      DAT_00898ac4 = 2;
      if (sVar2 != -1) {
        DAT_00898ac4 = 0;
      }
      local_10 = 1;
    }
  }
  if (param_2 == 0) {
    FUN_0042ac00();
    FUN_0042ac00();
  }
  else {
    FUN_0042ac00();
    FUN_0042ac00();
  }
  if (DAT_0067e9f8 == 0) {
    if (DAT_0067ed78 == &DAT_005f7370) {
      FUN_0042ac50();
    }
    else {
      FUN_0042ac50();
    }
  }
  else if (*(undefined **)((int)&DAT_0067ed78 + local_1c) == &DAT_005f7370) {
    FUN_0042ac50();
  }
  else {
    FUN_0042ac50();
  }
  iVar6 = 0;
  local_8 = 0;
  local_c = 0;
  puVar4 = &DAT_00898ad8 + local_10 * 0xd;
  do {
    sVar2 = FUN_0042ad90();
    sVar3 = FUN_0042ad90();
    if (sVar2 == -1) {
      if (sVar3 == -1) {
        if (param_2 == 0) {
          *(int *)((int)&DAT_0067edb4 + local_1c) = local_4;
        }
        else {
          local_4 = *(int *)(&DAT_0067ed74 + local_1c);
        }
        DAT_0067ece0 = local_4;
        sVar2 = FUN_0042ad90(param_1);
        FUN_00432b30((int)sVar2);
        if (param_2 == 0) {
          DAT_0067e9f8 = DAT_0067e9f8 + 1;
        }
        DAT_0067e844 = param_2;
        return;
      }
    }
    else {
      local_4 = local_4 + 1;
    }
    puVar1 = *(undefined **)((int)&DAT_0067ed78 + local_1c);
    puVar4[6] = puVar1;
    puVar4[-6] = 0xff040000;
    *puVar4 = 0x42800000;
    *(undefined1 *)(puVar4 + -3) = 0xff;
    *(undefined1 *)((int)puVar4 + -0xb) = 0xff;
    *(undefined1 *)((int)puVar4 + -10) = 0xff;
    *(undefined1 *)((int)puVar4 + -9) = 0xff;
    puVar4[-1] = 0x3f4ccccd;
    if ((((puVar1 == &DAT_005f72a0) || (puVar1 == &DAT_005f7370)) ||
        ((*(undefined **)((int)&DAT_0067ed38 + local_1c) == &DAT_005f72a0 && (param_2 == 2)))) &&
       (local_c != 0)) {
      puVar4[-1] = 0x3f147ae1;
    }
    puVar4[3] = 0;
    puVar4[-5] = 0;
    puVar4[1] = (float)((float10)local_c + extraout_ST0);
    if ((undefined *)puVar4[6] == &DAT_005f7370) {
      puVar4[1] = (float)(((float10)local_8 + extraout_ST0) - (float10)_DAT_005cd900);
    }
    if ((undefined *)puVar4[6] == &DAT_005f72a0) {
      puVar4[1] = (float)puVar4[1] - _DAT_005cd900;
    }
    puVar4[2] = iVar6;
    puVar4[4] = (int)sVar2;
    bVar7 = DAT_0067e9f8 == 0;
    puVar4[5] = (int)sVar3;
    puVar4[-2] = 0x1ff;
    if (bVar7) {
      puVar4[-5] = 1;
      puVar4[-2] = 0;
    }
    puVar4 = puVar4 + 0xd;
    local_c = local_c + 0x1e;
    iVar6 = iVar6 + 1;
    local_8 = local_8 + 0x1c;
  } while( true );
}

