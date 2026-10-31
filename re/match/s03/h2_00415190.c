// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00415190 FUN_00415190
/* h2 0x00415190 (105 B). Best: 100% BYTE-IDENTICAL (--bytes), first try. Start (g2s) 62.3%.
   Same last-place scan as 0x00415020; range test is `f >= lo && f <= hi` (Ghidra's `< != ==` = <=).
   Class: none (exact). */
#ifndef MATCH_PORT
extern float DAT_005d757c;
int FUN_0040e470(int i);
float FUN_00442cc0(int car);
#endif
int FUN_00415190(int car, float lo, float hi)
{
  int i;
  int last;
  float f;

  if (FUN_00442cc0(car) == DAT_005d757c) {
    last = -1;
    for (i = 0; i < 4; i++) {
      if (FUN_0040e470(i) == 1) last = i;
    }
    if (last != -1) {
      f = FUN_00442cc0(last);
      if (f >= lo && f <= hi) return 1;
    }
  }
  return 0;
}
