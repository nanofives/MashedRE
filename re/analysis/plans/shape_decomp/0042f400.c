
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

void FUN_0042f400(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint unaff_retaddr;
  undefined4 local_c;
  uint local_4;
  
  local_4 = DAT_00616038 ^ unaff_retaddr;
  iVar2 = FUN_00427780(param_1);
  FUN_004a2b60(&local_c,&DAT_005cd794,param_2);
  pcVar3 = (char *)&local_c;
  do {
    cVar1 = *pcVar3;
    pcVar3 = pcVar3 + 1;
  } while (cVar1 != '\0');
  iVar4 = (int)pcVar3 - ((int)&local_c + 1);
  iVar6 = 0;
  uVar5 = local_c;
  do {
    if (iVar6 == 0) {
      if (iVar4 == 3) {
        uVar5 = (uint)(ushort)(short)(char)local_c;
      }
      else {
        uVar5 = 0x30;
      }
      *(short *)(iVar2 + 2) = (short)uVar5;
    }
    else if (iVar6 == 1) {
      if (iVar4 == 3) {
        uVar5 = (uint)(ushort)(short)local_c._1_1_;
        *(short *)(iVar2 + 4) = (short)local_c._1_1_;
      }
      else if (iVar4 == 2) {
        uVar5 = (uint)(ushort)(short)(char)local_c;
        *(short *)(iVar2 + 4) = (short)(char)local_c;
      }
      else {
        if (iVar4 == 1) {
          uVar5 = 0x30;
        }
        *(short *)(iVar2 + 4) = (short)uVar5;
      }
    }
    else if (iVar6 == 2) {
      if (iVar4 == 3) {
        uVar5 = (uint)(ushort)(short)local_c._2_1_;
        *(short *)(iVar2 + 6) = (short)local_c._2_1_;
      }
      else if (iVar4 == 2) {
        uVar5 = (uint)(ushort)(short)local_c._1_1_;
        *(short *)(iVar2 + 6) = (short)local_c._1_1_;
      }
      else {
        if (iVar4 == 1) {
          uVar5 = (uint)(ushort)(short)(char)local_c;
        }
        *(short *)(iVar2 + 6) = (short)uVar5;
      }
    }
    iVar6 = iVar6 + 1;
  } while (iVar6 < 3);
  return;
}

