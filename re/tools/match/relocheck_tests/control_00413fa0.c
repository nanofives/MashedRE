// FIXTURE (MATCH-EXACT-DATA 2026-11-11): frozen copy of re/match/s03/h2_00413fa0.c, the control (relocheck OK).
// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00413fa0 FUN_00413fa0
/* h2 0x00413fa0 (49 B). Best: 100% BYTE-IDENTICAL (--bytes), first try. Start (g2s) 45.0%.
   The `xor ecx,ecx ... lea eax,[ecx+esi]` tail is an offset local initialised to 0 and set to 1 / 2 by an
   if / else-if on FUN_0042f6a0() == 4 / 5, returned as base + off (base = FUN_00430790() * 3);
   cl tail-duplicates the add into each arm. Class: none (exact). */
#ifndef MATCH_PORT
int FUN_0042f6a0(void);
int FUN_00430790(void);
#endif
int FUN_00413fa0(void)
{
  int base;
  int s;
  int off;

  base = FUN_00430790() * 3;
  s = FUN_0042f6a0();
  off = 0;
  if (s == 4) off = 1;
  else if (s == 5) off = 2;
  return base + off;
}
