// FLAGS: /O2 /GS /Ob1
// MATCH-S01. Ghidra's arities were wrong: 0x004671a0 takes 1 argument, 0x004c1bb0 / 0x004c1be0 take 3.
// The local is a 4-byte colour stored bytewise (0, 0, 0, 0xff).
typedef struct { unsigned char r, g, b, a; } Rgba4026d0;
#ifndef MASHED_RE_PORT
void *FUN_004671a0(int i);
void FUN_004c19f0(void *p);
int FUN_004c1a00(void *p);
void FUN_004c1bb0(void *p, Rgba4026d0 *c, int n);
void FUN_004c1be0(void *p, int a, int b);
#endif

// 0x004026d0
// MATCH: 0x004026d0 FUN_004026d0
void FUN_004026d0(int n)
{
  Rgba4026d0 c;

  c.r = 0;
  c.g = 0;
  c.b = 0;
  c.a = 0xff;
  if (n > 0) {
    do {
      FUN_004c1bb0(FUN_004671a0(0), &c, 3);
      if (FUN_004c1a00(FUN_004671a0(0)) != 0) {
        FUN_004c19f0(FUN_004671a0(0));
      }
      FUN_004c1be0(FUN_004671a0(0), 0, 1);
      n--;
    } while (n != 0);
  }
}
