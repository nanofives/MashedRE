// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00415020 FUN_00415020
/* h2 0x00415020 (183 B). Best: 100% BYTE-IDENTICAL (--bytes, relocations masked), first try. Start (g2s) 72.1%.
   Straight from Ghidra's logic with the 0x74-stride table at 0x0089a4c0 typed as a struct array
   (+0x24 int timer, +0x28 float), FUN_00442cc0 cdecl returning float, a for loop over 4 cars, and the
   72000-tick test as `t += dt; if (t > 72000) return 1;`. Class: none (exact). */
typedef struct { int p0[9]; int t24; float f28; int p1[18]; } Rec74;
#ifndef MATCH_PORT
extern Rec74 DAT_0089a4c0[4];
extern float DAT_005d757c;
extern float DAT_005cd088;
extern float DAT_005cc320;
extern int DAT_007f1008;
int FUN_0040e470(int i);
float FUN_00442cc0(int car);
#endif
int FUN_00415020(int car)
{
  int i;
  int last;
  float f;

  last = -1;
  if (FUN_00442cc0(car) == DAT_005d757c) {
    for (i = 0; i < 4; i++) {
      if (FUN_0040e470(i) == 1) last = i;
    }
    if (last != -1) {
      f = FUN_00442cc0(last);
      DAT_0089a4c0[car].f28 = f;
      if (f > DAT_005cd088) {
        DAT_0089a4c0[car].t24 += DAT_007f1008;
        if (DAT_0089a4c0[car].t24 > 72000) return 1;
      }
      if (f < DAT_005cc320) DAT_0089a4c0[car].t24 = 0;
      return 0;
    }
  }
  DAT_0089a4c0[car].t24 = 0;
  return 0;
}
