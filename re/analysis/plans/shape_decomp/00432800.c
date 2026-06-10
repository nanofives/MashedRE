
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00432800(int param_1)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined4 *puVar5;
  
  iVar4 = param_1 * 0x40;
  puVar3 = &DAT_0067ed84 + param_1 * 0x10;
  puVar5 = puVar3;
  for (iVar2 = 0xc; iVar1 = DAT_0067ea8c, iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar5 = 1;
    puVar5 = puVar5 + 1;
  }
  switch((&DAT_0067ed7c)[param_1 * 0x10]) {
  case 1:
    if (DAT_007f0f2c == 0) {
      *(undefined4 *)(&DAT_0067ed90 + iVar4) = 0;
    }
    DAT_007f0fe8 = 0;
    iVar2 = 0;
    do {
      FUN_0040e480(iVar2,0);
      iVar2 = iVar2 + 1;
    } while (iVar2 < 4);
    break;
  case 2:
    if (DAT_007f0ad4 == 0) {
      *(undefined4 *)(&DAT_0067ed8c + iVar4) = 0;
    }
    break;
  case 3:
    DAT_0067ea74 = 1;
    iVar2 = 0;
    do {
      FUN_0040e480(iVar2,0);
      iVar2 = iVar2 + 1;
    } while (iVar2 < 4);
    break;
  case 8:
    iVar2 = FUN_00492d10();
    if (iVar2 == 1) break;
    *(undefined4 *)(&DAT_0067ed8c + iVar4) = 0;
    goto LAB_00432a17;
  case 10:
    if (DAT_0067ea8c != 2) {
      (&DAT_0067ed88)[param_1 * 0x10] = 0;
    }
    if (iVar1 != 3) {
      *(undefined4 *)(&DAT_0067ed8c + iVar4) = 0;
    }
    break;
  case 0x12:
    if (DAT_0067ea88 == 1) {
      *(undefined4 *)(&DAT_0067ed90 + iVar4) = 0;
      DAT_0067ea7c = 0;
LAB_0043297d:
      iVar2 = FUN_00430b60();
      if (iVar2 == 2) {
        *(undefined4 *)(&DAT_0067ed98 + iVar4) = 0;
        DAT_0067ea78 = 0;
      }
    }
    else {
      if (DAT_0067ea7c == 0) goto LAB_0043297d;
      *(undefined4 *)(&DAT_0067ed98 + iVar4) = 1;
    }
    iVar2 = DAT_0067f17c * 0x30;
    if (*(int *)(&DAT_007f0a58 + iVar2) == 0) {
      *(undefined4 *)(&DAT_0067ed9c + iVar4) = 0;
      DAT_0067ea88 = 0;
    }
    else {
      *(undefined4 *)(&DAT_0067ed9c + iVar4) = 1;
    }
    if (*(int *)(&DAT_007f0a50 + iVar2) == 0) {
      (&DAT_0067ed88)[param_1 * 0x10] = 0;
      DAT_0067ea74 = 0;
    }
    else {
      (&DAT_0067ed88)[param_1 * 0x10] = 1;
    }
    if (DAT_007f0fd0 == 2) {
      DAT_0067ea64 = 0;
    }
    iVar2 = FUN_00430b60();
    if ((iVar2 != 4) && (iVar2 = FUN_0042f500(), iVar2 == 0)) break;
    goto LAB_00432a17;
  case 0x18:
    iVar2 = FUN_00430830(1);
    if (iVar2 != 0) break;
LAB_00432a17:
    *(undefined4 *)(&DAT_0067ed90 + iVar4) = 0;
    break;
  case 0x1c:
    *puVar3 = 1;
    (&DAT_0067ed88)[param_1 * 0x10] = 1;
    *(undefined4 *)(&DAT_0067ed8c + iVar4) = 1;
    *(undefined4 *)(&DAT_0067ed90 + iVar4) = 1;
    iVar2 = 0;
    do {
      iVar4 = thunk_FUN_00497450(iVar2);
      if (iVar4 == 0) {
        *puVar3 = 0;
      }
      iVar2 = iVar2 + 1;
      puVar3 = puVar3 + 1;
    } while (iVar2 < 4);
    DAT_00898aa0 = 0;
    DAT_00898aa4 = 1;
    _DAT_00898aa8 = 2;
    _DAT_00898aac = 3;
    DAT_00898b1c = 0x236;
    _DAT_00898b50 = 0x237;
    _DAT_00898b84 = 0x238;
    _DAT_00898bb8 = 0x239;
  }
  iVar2 = (&DAT_0067ed80)[param_1 * 0x10];
  if (iVar2 == -1) {
    iVar2 = 0;
  }
  if ((&DAT_0067ed84)[param_1 * 0x10 + iVar2] != 1) {
    iVar4 = 0;
    if (0 < (int)(&DAT_0067edb4)[param_1 * 0x10]) {
      do {
        if ((&DAT_0067ed84)[param_1 * 0x10 + iVar2] == 1) {
          (&DAT_0067ed80)[param_1 * 0x10] = iVar2;
          return;
        }
        iVar2 = iVar2 + 1;
        if (iVar2 == (&DAT_0067edb4)[param_1 * 0x10]) {
          iVar2 = 0;
        }
        iVar4 = iVar4 + 1;
      } while (iVar4 < (int)(&DAT_0067edb4)[param_1 * 0x10]);
    }
    (&DAT_0067ed80)[param_1 * 0x10] = 0xffffffff;
  }
  return;
}

