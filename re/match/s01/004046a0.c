// FLAGS: /O2 /GS /Ob1
// MATCH-S01. Course id (FUN_00426c00) 0xb / 0x1a / 0x27 picks two linear terms of f = 1 - FUN_004173a0() * 0.1;
// any other id sets the timer to 30 and returns. Then six 0x14-byte slots at 0x00636b88 are reset, slot 0 and every
// slot whose FUN_00426cf0()[i-1] is positive get the second term and are counted in DAT_005ea0b4.
// BEST 96.8% (6 bytes differ), class a?/b. Residual: the original stores DAT_005ea0b4 from the count register EDX
// (ours from the zero register EDI), and hoists the DAT_007f0fe0 store of case 0x27 + default above the 0x27 branch
// (ours keeps it in the 0x27 block). Tried: if-chain (35-44%), switch, i initialised before the calls (+30 points),
// local count n (+12), local w for p[i-1] (+12), count as the global (76.6%), store placement per case / before the
// switch (90.4%), case order, n initialised at the top (68.4%), permuter 305 compiles (no gain).
typedef struct { float t; int who; int f8; int on; int f10; } Slot636b88;   /* 0x14 bytes */
extern Slot636b88 DAT_00636b88[6];
extern int DAT_005ea0b4;
extern float DAT_005ea0a8;
extern float DAT_005ea0ac;
extern int DAT_005ea0b0;
extern int DAT_007f0fe0;
extern float DAT_007f0fe4;
int *FUN_00426cf0(void);
int FUN_00426c00(void);
float FUN_004173a0(void);

// MATCH: 0x004046a0 FUN_004046a0
void FUN_004046a0(void)
{
  int *p;
  int course;
  int i;
  int n;
  int w;
  float f;
  float t;

  i = 0;
  p = FUN_00426cf0();
  course = FUN_00426c00();
  f = 1.0f - FUN_004173a0() * 0.1f;
  n = 0;
  DAT_005ea0b4 = n;
  switch (course) {
  case 0xb:
    DAT_007f0fe4 = f * 3.0f + 14.5f;
    t = f * 0.5f + 4.15f;
    break;
  case 0x1a:
    DAT_007f0fe4 = f * 3.0f + 17.0f;
    t = f * 0.5f + 5.8f;
    break;
  case 0x27:
    t = f * 0.5f + 3.6f;
    DAT_007f0fe4 = f * 3.0f + 16.0f;
    break;
  default:
    DAT_007f0fe0 = 0;
    DAT_007f0fe4 = 30.0f;
    return;
  }
  DAT_007f0fe0 = 0;
  while (i < 6) {
    DAT_00636b88[i].who = -1;
    DAT_00636b88[i].f8 = 0;
    DAT_00636b88[i].t = 0;
    DAT_00636b88[i].on = 0;
    DAT_00636b88[i].f10 = 0;
    if (i == 0) {
      DAT_00636b88[0].t = t;
      DAT_00636b88[0].who = 0;
      DAT_00636b88[0].f8 = 0;
      DAT_00636b88[0].on = 1;
      n++;
    } else {
      w = p[i - 1];
      if (w > 0) {
      DAT_00636b88[i].t = t;
      DAT_00636b88[i].who = w;
      DAT_00636b88[i].f8 = 0;
      DAT_00636b88[i].on = 1;
      n++;
      }
    }
    i++;
  }
  DAT_005ea0b4 = n;
  DAT_005ea0a8 = 1.0f;
  DAT_005ea0ac = DAT_007f0fe4;
  DAT_005ea0b0 = -1;
}
