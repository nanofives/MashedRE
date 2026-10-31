// FLAGS: /O2 /GS /Ob1
// MATCH-S01. Three local 4-byte colours stored bytewise; the one picked by FUN_0042f6a0's result is copied to
// +4 of the object FUN_004b4080 returns.
typedef struct { unsigned char r, g, b, a; } Rgba401340;
typedef struct { int f0; Rgba401340 col; } Obj4b4080;
#ifndef MASHED_RE_PORT
extern int DAT_007f0ff0;
extern void *DAT_00636564;
extern const float DAT_005cc328;
extern float DAT_00636574;
int FUN_0042f6a0(void);
Obj4b4080 *FUN_004b4080(void *p);
#endif

// 0x00401340
// MATCH: 0x00401340 FUN_00401340
void FUN_00401340(void)
{
  Rgba401340 c1;
  Rgba401340 c2;
  Rgba401340 c3;
  Obj4b4080 *o;

  DAT_00636574 = (float)-DAT_007f0ff0 * DAT_005cc328;
  o = FUN_004b4080(DAT_00636564);
  c1.r = 0xd8; c1.g = 0x66; c1.b = 0x0a; c1.a = 0xff;
  c2.r = 0xf0; c2.g = 0xf0; c2.b = 0xf5; c2.a = 0xff;
  c3.r = 0xff; c3.g = 0xc5; c3.b = 0x14; c3.a = 0xff;
  if (FUN_0042f6a0() == 3) {
    o->col = c1;
  } else if (FUN_0042f6a0() == 4) {
    o->col = c2;
  } else {
    o->col = c3;
  }
}
