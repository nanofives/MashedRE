
/* [C1 2026-05-20] BSP world file loader. Already has a [C1 2026-05-07] comment in Ghidra noting
   pattern shared with… */

undefined4 FUN_0042a640(char *param_1)

{
  char *pcVar1;
  int iVar2;
  undefined4 uVar3;
  
  (**(code **)(DAT_007d3ff8 + 0xcc))(&DAT_0067e3a8,param_1);
  pcVar1 = _strchr(param_1,0x2e);
  if (pcVar1 == (char *)0x0) {
    (**(code **)(DAT_007d3ff8 + 0xd4))(&DAT_0067e3a8,PTR_DAT_005f65b4);
  }
  iVar2 = FUN_0042a530(&DAT_0067dba8,&DAT_0067e3a8);
  if (iVar2 != 0) {
    uVar3 = FUN_004b3c60();
    return uVar3;
  }
  return 0;
}

