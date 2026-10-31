// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00412130 FUN_00412130
/* h2 0x00412130 (58 B code + 7-entry jump table at 0x0041216c). Best: 100% exact, BYTE-IDENTICAL (--bytes).
   Start (g2s) 75.0%. Needs BOTH: an outer switch on the FUN_0042f6a0 result (case 2 / cases 3..5; the
   `if (s == 2) ... if (s > 2 && s <= 5)` form puts `return 3` inline, 65-68%) AND explicit
   `case 5: case 6: case 7: return 0;` in the inner switch on DAT_007f0fd0 (without them cl builds a
   two-level byte-index table, movzx + jmp, 75%). With both: one direct 7-dword table, as the original.
   Class: none (exact). */
#ifndef MATCH_PORT
extern int DAT_007f0fd0;
int FUN_0042f6a0(void);
#endif
int FUN_00412130(void)
{
  int s;

  s = FUN_0042f6a0();
  switch (s) {
  case 2:
    return 3;
  case 3:
  case 4:
  case 5:
    switch (DAT_007f0fd0) {
    case 4:
    case 8:
    case 9:
      return 1;
    case 5:
    case 6:
    case 7:
      return 0;
    case 10:
      return 6;
    }
  }
  return 0;
}
