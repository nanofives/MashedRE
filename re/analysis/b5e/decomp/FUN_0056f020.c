
/* [C1 2026-05-19] Signature: `void FUN_0056f020(int param_1)`. */

void FUN_0056f020(int param_1)

{
  *(uint *)(param_1 + 0x100) = *(int *)(param_1 + 0x100) + 3U & 0xfffffffc;
  *(int *)(param_1 + 0xf4) =
       *(int *)(param_1 + 0xf4) + *(int *)(*(int *)(param_1 + 0xe8) + *(int *)(param_1 + 0xfc) * 4);
  *(int *)(param_1 + 0xec) = *(int *)(param_1 + 0xec) + 1;
  *(int *)(param_1 + 200) = *(int *)(param_1 + 200) + 1;
  *(int *)(param_1 + 0xe0) = *(int *)(param_1 + 0xe0) + 1;
  *(int *)(param_1 + 0xd4) = *(int *)(param_1 + 0xd4) + 1;
  *(int *)(param_1 + 0xfc) = *(int *)(param_1 + 0xfc) + 1;
  return;
}

