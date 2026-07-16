
/* [C1 2026-05-20] Signature recovered: `int FUN_0057adb0(undefined4 unused, undefined4 body_pair,
   float depth_tol, int… */

undefined4
FUN_0057adb0(undefined4 param_1,undefined4 param_2,float param_3,int param_4,undefined4 param_5)

{
  int iVar1;

  iVar1 = FUN_0057a9a0(param_1,param_2,param_3,param_4 + 0x100,param_4 + 0x10c,
                       (float *)(param_4 + 0x110),param_5);
  if ((iVar1 != 0) && (*(float *)(param_4 + 0x110) < param_3)) {
    FUN_005757d0(param_2,param_4);
    return 1;
  }
  return 0;
}

