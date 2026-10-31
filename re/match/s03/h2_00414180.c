// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00414180 FUN_00414180
/* h2 0x00414180 (155 B). Best: 100% BYTE-IDENTICAL (--bytes, relocations masked). Start (g2s) 58.5%.
   Steps: 0x004a2c48 is _ftol (Ghidra showed it as a call returning the index): idx = (int)DAT_0089a360 and
   rec.b = (int)f; Ghidra dropped the mode test on DAT_007f0fd0 (4/9/8 -> f = DAT_005cd08c, 0x004141e2..f3).
   DAT_0089a370 stored as float 0 (separate `mov [],0` instead of shared xor eax) -> both literal stores.
   The two ints read into separate int locals px/py (not direct fild from the table, not a struct copy;
   struct copy put x/y in swapped stack slots) -> 100%. FUN_00472650 is cdecl(float,float) returning float.
   Class: none (exact). */
typedef struct { int a; int pad0[13]; int b; int pad1[14]; } Rec74;
typedef struct { int x; int y; } IPair;
#ifndef MATCH_PORT
extern int DAT_0089a368;
extern float DAT_0089a370;
extern float DAT_0089a360;
extern int DAT_007f0fd0;
extern float DAT_005cd08c;
extern IPair DAT_005f2d80[];
extern int DAT_005f2eb4[];
extern Rec74 DAT_0089a4c0[4];
float FUN_00472650(float a, float b);
#endif
void FUN_00414180(void)
{
  int i;
  int idx;
  int px;
  int py;
  float f;

  DAT_0089a368 = 0;
  DAT_0089a370 = 0;
  for (i = 0; i < 4; i++) {
    idx = (int)DAT_0089a360;
    px = DAT_005f2d80[idx].x;
    py = DAT_005f2d80[idx].y;
    f = FUN_00472650((float)px, (float)py);
    if (DAT_007f0fd0 == 4 || DAT_007f0fd0 == 9 || DAT_007f0fd0 == 8) f = DAT_005cd08c;
    DAT_0089a4c0[i].b = (int)f;
    DAT_0089a4c0[i].a = DAT_005f2eb4[idx];
  }
}
