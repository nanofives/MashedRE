// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h3 0x0042fb70 (701 B): 100% (regs 100%). Raw g2s 33.8%.
   What moved it: (1) this file HOLDS the static register-convention callees 0x0042ac00 (list in EDX), 0x0042ac50
   (EAX = count, ECX = 30) and 0x0042b8b0 / 0x0042b8c0 (static, so EDX stays live across them); (2) the Arrow
   sprites take RwV2d pos and size by value (FUN_004739f0 / FUN_00473870), which places pos/size contiguously and
   apart from the int->float temp; (3) the text width as a named int before the multiply (`tw`); (4) the selected-row
   test written as an if/else per mode, each arm with its own `sel == i` colour if/else (the compiler merges the
   arms, leaving the original's `cmp [row.sel], ebp` in both branches); (5) switch (i) for the label, if/else chains
   inside. 0x0042ac00 scores 76.2% by instructions because Ghidra's size (59 B) stops before its tail; match.py
   reports its 67 B obj symbol byte-identical. Residual: none. */
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
typedef struct { int *list; int f4; int sel; int pad[13]; } MenuRow;
MGLOBAL(RwGlobalsT *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067e7e8, 0x0067e7e8);
MGLOBAL(int, DAT_0067e7ec, 0x0067e7ec);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(int, DAT_0067ea78, 0x0067ea78);
MGLOBAL(int, DAT_0067ea80, 0x0067ea80);
MGLOBAL(int, DAT_0067ea84, 0x0067ea84);
MGLOBALA(MenuRow, DAT_0067ed38, 16, 0x0067ed38);
MGLOBALA(char, s_Arrow_005cd828, 6, 0x005cd828);
MGLOBAL(short, DAT_0067ea54, 0x0067ea54);
MGLOBAL(short, DAT_0067ea56, 0x0067ea56);
int FUN_0040bb50(const char *name);
void FUN_00427e00(int text, float x, float y, RwRGBA col, float scale, int flags);
float FUN_004282a0(int text, float scale);
void FUN_00473870(int tex, RwV2d pos, RwV2d size, RwRGBA col, int flags);
void FUN_004739f0(int tex, RwV2d pos, RwV2d size, RwRGBA col, float a6, int a7, int a8, float a9, int a10, int a11);
// HELD: 0x0042ac00 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
#ifndef MATCH_PORT
static int FUN_0042ac00(int *p)
{
  int n = 0;
  if (p == 0) return 0;
  for (;;) {
    while (*p != (int)0xff060000 && *p != (int)0xff070000) p++;
    if (*p == (int)0xff070000) return n;
    n++;
    p++;
  }
}
#else
int FUN_0042ac00(int *p);
#endif
// HELD: 0x0042ac50 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
#ifndef MATCH_PORT
static int FUN_0042ac50(unsigned int n, int h)
{
  int y;
  if ((n & 1) == 0) y = 0xf0 - h / 2; else y = 0xf0;
  return y - ((n - 1) >> 1) * h;
}
#else
int FUN_0042ac50(unsigned int n, int h);
#endif
// HELD: 0x0042b8b0 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
#ifndef MATCH_PORT
static short FUN_0042b8b0(void)
{
  return DAT_0067ea54;
}
#else
short FUN_0042b8b0(void);
#endif
// HELD: 0x0042b8c0 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
#ifndef MATCH_PORT
static short FUN_0042b8c0(void)
{
  return DAT_0067ea56;
}
#else
short FUN_0042b8c0(void);
#endif
// MATCH: 0x0042fb70 FUN_0042fb70
void FUN_0042fb70(void)
{
  int *list;
  int i;
  int text;
  int tw;
  RwRGBA col;
  float ty;
  RwV2d pos;
  RwV2d size;

  if (DAT_0067e7e8 == 0) {
    if (DAT_0067e7ec < 0x60) {
      return;
    }
    list = DAT_0067ed38[DAT_0067e9f8].list;
  }
  else {
    list = DAT_0067ed38[DAT_0067e9f8 + 1].list;
  }
  ty = (float)FUN_0042ac50(FUN_0042ac00(list), 30);
  DAT_007d3ff8->fpRenderStateSet(6, 0);
  DAT_007d3ff8->fpRenderStateSet(8, 0);
  col.alpha = (unsigned char)DAT_0067e7ec;
  for (i = 0; i < 3; i++) {
    if (DAT_0067e7e8 == 0) {
      if (DAT_0067ed38[DAT_0067e9f8].sel == i) {
        col.red = col.green = col.blue = 0;
      }
      else {
        col.red = col.green = col.blue = 0xff;
      }
    }
    else {
      if (DAT_0067ed38[DAT_0067e9f8 + 1].sel == i) {
        col.red = col.green = col.blue = 0;
      }
      else {
        col.red = col.green = col.blue = 0xff;
      }
    }
    pos.x = (float)FUN_0042b8b0() * 0.453125f;
    pos.y = (float)FUN_0042b8c0() * (ty - 8.0f) * 0.0020833334f;
    size.x = (float)FUN_0042b8b0() * 0.025f;
    size.y = (float)FUN_0042b8c0() * 0.033333335f;
    FUN_004739f0(FUN_0040bb50(s_Arrow_005cd828), pos, size, col, 0.9999f, 0, 0, 1.0f, 0, 0);
    switch (i) {
    case 0:
      if (DAT_0067ea84 == 0) text = 0xe3;
      else if (DAT_0067ea84 == 1) text = 0xd2;
      else if (DAT_0067ea84 == 2) text = 0xd3;
      else if (DAT_0067ea84 == 3) text = 0xd4;
      break;
    case 1:
      text = (DAT_0067ea78 != 0) + 0x59;
      break;
    case 2:
      if (DAT_0067ea80 == 0) text = 0xe0;
      else if (DAT_0067ea80 == 1) text = 0xe1;
      else if (DAT_0067ea80 == 2) text = 0xe2;
      break;
    }
    FUN_00427e00(text, 310.0f, ty, col, 0.7f, 0);
    tw = (int)FUN_004282a0(text, 0.7f);
    pos.x = (float)(FUN_0042b8b0() * (tw + 0x13e) / 640);
    FUN_00473870(FUN_0040bb50(s_Arrow_005cd828), pos, size, col, 1);
    ty += 30.0f;
  }
  DAT_007d3ff8->fpRenderStateSet(6, 1);
  DAT_007d3ff8->fpRenderStateSet(8, 1);
}
