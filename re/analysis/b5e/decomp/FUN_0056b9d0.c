
/* [C1 2026-05-19] Signature: `undefined4 FUN_0056b9d0(int param_1, undefined4 param_2..param_7)`.
    */

undefined4
FUN_0056b9d0(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5
            ,undefined4 param_6,undefined4 param_7)

{
  *(undefined4 *)(param_1 + 0x84) = 0;
  FUN_0056b7a0(*(undefined4 **)(param_1 + 0x70),param_2,param_3,
               *(undefined4 *)(*(int *)**(undefined4 **)(param_1 + 0x70) + 0xc018));
  FUN_00573670(param_1,*(undefined4 *)(param_1 + 0x80),*(undefined4 *)(param_1 + 0x84),param_4,
               param_5,param_6,param_7);
  return 1;
}

