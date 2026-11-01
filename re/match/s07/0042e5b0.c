// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h3 0x0042e5b0 (755 B): best 54.3% (regs 66.2%). Raw converter failed to compile (C2120).
   Structure is complete and in original order: 16-int table initialiser {0..5,0..5,0..3} (the scattered constant
   stores), FUN_00473c20 / FUN_00473ee0 / FUN_00474890 with RwV2d pos/size by value, t = (int)(DAT_007f1010*60) & 0x1ff,
   the two counters, the four-band alpha chain, two sprites through FUN_0042e590(0/1), final clamp of DAT_008990e0.
   Permuter (6 rounds) best 56.2% only with `0xf & table[DAT_0067e848]`, which changes behaviour: not taken; its
   semantics-preserving moves (unsigned a1, statement order) are in.
   Residual (b) source shape: register allocation. The original keeps t in a stack slot (frame +4, re-read for both
   sprites) and gives ESI the CSE'd constant 2 then s1, EDI the constant 4 then pos.x; ours enregisters t in ESI
   (constants 2/4 land in EDI/ESI swapped), so the frame is 4 bytes smaller and every stack offset shifts.
   Tried: initialiser vs explicit stores (two orders), copy of t for the band chain, s1/s2 placement (before the
   chain, inline in the call), separate vs combined counter ifs. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct { float x, y; } RwV2d;
MGLOBAL(int, DAT_00898ab8, 0x00898ab8);
MGLOBAL(int, DAT_008991a0, 0x008991a0);
MGLOBAL(float, DAT_007f1010, 0x007f1010);
MGLOBAL(int, DAT_0067e848, 0x0067e848);
MGLOBAL(int, DAT_0067f0b8, 0x0067f0b8);
MGLOBAL(int, DAT_008990e0, 0x008990e0);
int FUN_0042e590(int which);
void FUN_00473c20(RwV2d pos, RwV2d size, int a, int b, int c, int d, int e, int f);
void FUN_00473ee0(RwV2d pos, RwV2d size, int a, int b, int c, int d, int e, int f);
void FUN_00474890(int tex, RwV2d pos, RwV2d size, RwRGBA col, int v, int frame, int flag);
// MATCH: 0x0042e5b0 FUN_0042e5b0
void FUN_0042e5b0(void)
{
  int table[16] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3 };
  RwRGBA col;
  RwV2d pos;
  RwV2d size;
  int t;
  int v;
  unsigned a1;
  int a2;
  int s1;
  int s2;

  pos.y = 0.0f;
  pos.x = 0.0f;
  size.x = 512.0f;
  size.y = 512.0f;
  FUN_00473c20(pos, size, 0, DAT_00898ab8, 0, DAT_008991a0, 0, 0xff);
  col.red = 0xff;
  col.green = 0xff;
  col.blue = 0xff;
  pos.x = 288.0f;
  pos.y = 64.0f;
  size.y = 352.0f;
  size.x = 352.0f;
  t = (int)(DAT_007f1010 * 60.0f) & 0x1ff;
  if (t == 0x1e0) {
    DAT_0067e848 += 2;
  }
  else if (t == 0xe0) {
    DAT_0067f0b8 += 2;
  }
  if ((DAT_0067f0b8 & 1) == 0) {
    DAT_0067f0b8++;
  }
  if (t < 0xe0) {
    a2 = 0;
    a1 = 0xff;
  }
  else if (t < 0x100) {
    v = t - 0xe0;
    if (v < 0x10) {
      a2 = v << 4;
      a1 = 0xff;
    }
    else {
      a2 = 0xff;
      a1 = (0x1f - v) << 4;
    }
  }
  else if (t < 0x1e0) {
    a1 = 0;
    a2 = 0xff;
  }
  else {
    v = t - 0x1e0;
    if (v < 0x10) {
      a1 = v << 4;
      a2 = 0xff;
    }
    else {
      a2 = (0x1f - v) << 4;
      a1 = 0xff;
    }
  }
  s1 = table[DAT_0067e848 & 0xf];
  s2 = table[DAT_0067f0b8 & 0xf];
  if (a1 != 0) {
    col.alpha = (unsigned char)a1;
    v = t + 0x20;
    if (v > 0x1ff) {
      v -= 0x200;
    }
    FUN_00474890(FUN_0042e590(0), pos, size, col, (v << 8) / 288, s1, 1);
  }
  if (a2 != 0) {
    col.alpha = (unsigned char)a2;
    FUN_00474890(FUN_0042e590(1), pos, size, col, ((t - 0xe0) << 8) / 288, s2, 1);
  }
  pos.x = 0.0f;
  size.x = (float)DAT_008990e0 + 512.0f;
  pos.y = 0.0f;
  size.y = 512.0f;
  DAT_008990e0 -= 16;
  if (DAT_008990e0 < 0) {
    DAT_008990e0 = 0;
  }
  else if (DAT_008990e0 > 0x200) {
    DAT_008990e0 = 0x200;
  }
  FUN_00473ee0(pos, size, 0, DAT_00898ab8, 0, DAT_008991a0, 0, 0xff);
}
