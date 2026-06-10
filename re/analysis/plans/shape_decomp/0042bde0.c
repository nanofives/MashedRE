
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-20] 5-call HUD-rect emitter. Takes (param_1 int, param_2 byte). Builds a packed color
   arg by ORing param_2… */

void FUN_0042bde0(int param_1,byte param_2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  
  fVar1 = (float)(param_1 + -0xd);
  param_1 = CONCAT13(param_2 >> 1,0x146ef0);
  FUN_00472c60(0x42400000,fVar1,0x44058000,0x41d00000,param_1);
  param_1 = CONCAT13(param_2,0x1050b4);
  FUN_00472c60(0x42380000,fVar1,0x40000000,0x41d00000,param_1);
  fVar2 = _DAT_005cc35c + 534.0;
  FUN_00472c60(0x42380000,fVar1,fVar2,0x40000000,param_1);
  fVar3 = 26.0 - _DAT_005cc574;
  fVar1 = fVar3 + fVar1;
  FUN_00472c60(0x42380000,fVar1,fVar2,0x40000000,param_1);
  FUN_00472c60(_DAT_005cd784 + 534.0,fVar1 - fVar3,0x40000000,0x41d00000,param_1);
  return;
}

