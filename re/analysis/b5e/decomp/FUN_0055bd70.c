
/* [C1 2026-05-19] Tail-calls vtable[+0x10] on owner at `param_1+0x5c`: `(**(code **)(*(int
   *)(param_1 + 0x5c) + 0x10))()` (cited… */

void FUN_0055bd70(int param_1)

{
                    /* WARNING: Could not recover jumptable at 0x0055bd7b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*(int *)(param_1 + 0x5c) + 0x10))();
  return;
}

