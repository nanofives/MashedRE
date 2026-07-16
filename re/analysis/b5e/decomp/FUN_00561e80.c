
/* [C1 2026-05-19] Calls FUN_00568dd0(*(this+0xc0), this+0xc4) and returns this. Pair with
   FUN_00561e60 — likely "begin" vs "end". */

int FUN_00561e80(int param_1)

{
  FUN_00568dd0(*(undefined4 *)(param_1 + 0xc0),param_1 + 0xc4);
  return param_1;
}

