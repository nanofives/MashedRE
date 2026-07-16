
/* [C1 2026-05-19] Guard: tests `*(*(this+0x70) + 0xc)` (world+0xc), and if nonzero, calls
   FUN_0056bb30(this). */

int FUN_0055ff70(int param_1)

{
  if (*(int *)(*(int *)(param_1 + 0x70) + 0xc) != 0) {
    FUN_0056bb30(param_1);
  }
  return param_1;
}

