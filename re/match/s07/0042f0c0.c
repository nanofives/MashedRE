// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h3 0x0042f0c0 (801 B): best 84.5% (regs 87.4%). Raw g2s 12.2%.
   Same family as 0x0042fb70 / 0x0042fe90 (both 100%). This file HOLDS static 0x0042b8b0 / 0x0042b8c0 (EDX stays
   live across them). What moved it: dead char buf[8] for the /GS cookie (the original carries one, the frame has
   no real char buffer), black/white as two RwRGBA locals (black is enregistered), RwV2d pos/size by value for the
   Arrow sprites, the label call written as an if/else per colour (tail-merged), tw as a named int, mode as a local,
   switch with a jump table for row 0, permuter compare-operand swaps (84.0 -> 84.5).
   Residual (b) source shape: the frame order of sel/i/black/white (original sel 0, i 4, black 8, white 0xc; ours
   i, black, white, sel) and the sel store: the original computes both arms into EAX with idx in EDX and stores sel
   once after the merge, ours computes idx in EAX and stores sel in each arm. Tried: ternary / if-else / inverted
   test / pointer-to-row / inline helper / temp copy / idx loaded inside the arm / declaration order (no effect on
   this compiler's slot order), permuter 6 rounds. Loop-head alignment padding (lea esp,[esp]) follows from the
   size difference. */
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
typedef int (*RenderStateSetFn)(int, int);
typedef struct { char pad[0x20]; RenderStateSetFn fpRenderStateSet; } RwGlobalsT;
typedef struct { int f0; int f4; int sel; int pad[13]; } MenuRow;
MGLOBAL(RwGlobalsT *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067e7b0, 0x0067e7b0);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(int, DAT_0067e7b4, 0x0067e7b4);
MGLOBAL(int, DAT_0067ea8c, 0x0067ea8c);
MGLOBAL(int, DAT_005f76c4, 0x005f76c4);
MGLOBAL(int, DAT_005f76c8, 0x005f76c8);
MGLOBAL(int, DAT_0067e844, 0x0067e844);
MGLOBALA(MenuRow, DAT_0067ed38, 16, 0x0067ed38);
MGLOBALA(char, s_Arrow_005cd828, 6, 0x005cd828);
int FUN_0040bb50(const char *name);
void FUN_00427e00(int text, float x, float y, RwRGBA col, float scale, int flags);
float FUN_004282a0(int text, float scale);
MGLOBAL(short, DAT_0067ea54, 0x0067ea54);
MGLOBAL(short, DAT_0067ea56, 0x0067ea56);
// MATCH: 0x0042b8b0 FUN_0042b8b0
static short FUN_0042b8b0(void)
{
  return DAT_0067ea54;
}
// MATCH: 0x0042b8c0 FUN_0042b8c0
static short FUN_0042b8c0(void)
{
  return DAT_0067ea56;
}
void FUN_00473870(int tex, RwV2d pos, RwV2d size, RwRGBA col, int flags);
void FUN_004739f0(int tex, RwV2d pos, RwV2d size, RwRGBA col, float a6, int a7, int a8, float a9, int a10, int a11);
// MATCH: 0x0042f0c0 FUN_0042f0c0
void FUN_0042f0c0(void)
{
  int sel;
  int mode;
  int alpha;
  int idx;
  int i;
  RwRGBA black;
  RwRGBA white;
  char buf[8];
  RwV2d pos;
  RwV2d size;
  float ty;
  int text;
  int tw;

  DAT_007d3ff8->fpRenderStateSet(6, 0);
  DAT_007d3ff8->fpRenderStateSet(8, 0);
  mode = DAT_0067e7b0;
  idx = DAT_0067e9f8;
  if (mode == 0) {
    sel = DAT_0067ed38[idx].sel;
  }
  else {
    if (DAT_0067e844 == 0) {
      idx -= 2;
    }
    sel = DAT_0067ed38[idx + 1].sel;
  }
  white.red = white.green = white.blue = 0xff;
  alpha = DAT_0067e7b4;
  white.alpha = black.alpha = (unsigned char)alpha;
  if (mode == 0 && alpha < 0x60) {
    DAT_007d3ff8->fpRenderStateSet(6, 1);
    DAT_007d3ff8->fpRenderStateSet(8, 1);
    return;
  }
  pos.x = (float)FUN_0042b8b0() * 0.40625f;
  black.red = black.green = black.blue = 0;
  pos.y = (float)FUN_0042b8c0() * 0.42291668f;
  size.x = (float)FUN_0042b8b0() * 0.025f;
  size.y = (float)FUN_0042b8c0() * 0.033333335f;
  ty = 211.0f;
  for (i = 0; i < 3; i++) {
    if (i == sel) {
      FUN_004739f0(FUN_0040bb50(s_Arrow_005cd828), pos, size, black, 0.9999f, 0, 0, 1.0f, 0, 0);
    }
    if (i == 0) {
      switch (DAT_0067ea8c) {
      case 0: text = 0x3a; break;
      case 1: text = 0x154; break;
      case 2: text = 0x5f; break;
      case 3: text = 0x60; break;
      }
    }
    else if (i == 1) {
      text = DAT_005f76c4 == 1 ? 0x69 : 0x61;
    }
    else if (i == 2) {
      text = DAT_005f76c8 == 1 ? 0x6a : 0x62;
    }
    if (i == sel) {
      FUN_00427e00(text, 280.0f, ty, black, 0.7f, 0);
    }
    else {
      FUN_00427e00(text, 280.0f, ty, white, 0.7f, 0);
    }
    if (sel == i) {
      tw = (int)FUN_004282a0(text, 0.7f);
      pos.x = (float)(FUN_0042b8b0() * (tw + 0x120) / 640);
      FUN_00473870(FUN_0040bb50(s_Arrow_005cd828), pos, size, black, 1);
    }
    pos.y += (float)(FUN_0042b8c0() * 30 / 480);
    ty += 30.0f;
  }
  DAT_007d3ff8->fpRenderStateSet(6, 1);
  DAT_007d3ff8->fpRenderStateSet(8, 1);
}
