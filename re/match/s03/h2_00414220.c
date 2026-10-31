// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00414220 FUN_00414220
/* h2 0x00414220 (213 B). Best: 100% BYTE-IDENTICAL (--bytes), first try. Start (g2s) 65.8%.
   if/else-if chains on DAT_0089a368 inside if/else on last == lead; 0x005cc31c / 0x005cc574 / 0x005cc320
   written as literals 3.0f / 2.0f / 1.0f. Class: none (exact). */
#ifndef MATCH_PORT
extern int DAT_0089a364;
extern int DAT_0089a368;
extern float DAT_0089a380;
int FUN_0040e470(int i);
int FUN_0040e4b0(void);
#endif
void FUN_00414220(void)
{
  int lead;
  int i;
  int last;

  lead = FUN_0040e4b0();
  if (lead != -1) {
    last = -1;
    DAT_0089a364 = lead;
    for (i = 0; i < 4; i++) {
      if (FUN_0040e470(i) == 1) last = i;
    }
    if (last == lead) {
      if (DAT_0089a368 == 2) DAT_0089a380 += 3.0f;
      else if (DAT_0089a368 == 0) DAT_0089a380 += 2.0f;
      else if (DAT_0089a368 == 1) DAT_0089a380 += 1.0f;
    } else {
      if (DAT_0089a368 == 2) DAT_0089a380 -= 1.0f;
      else if (DAT_0089a368 == 0) DAT_0089a380 -= 2.0f;
      else if (DAT_0089a368 == 1) DAT_0089a380 -= 3.0f;
    }
  }
}
