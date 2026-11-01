// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h3 0x0042fe90 (647 B): 100% (regs 100%). Raw converter failed to compile (C2120).
   Same family as 0x0042fb70 (this file HOLDS the same static callees 0x0042ac00, 0x0042ac50, 0x0042b8b0,
   0x0042b8c0). What moved it: RwV2d pos/size by value for the Arrow calls (96.7% with a 4-float struct, 100% as two
   RwV2d), alpha re-derived from the global (`(unsigned char)DAT_0067e814 >> 2`, which gives the original's hoisted
   `mov cl, [0x67e814]` reload at the end of the drawn branch), tw as a named int, the 4-row loop over the 0x34-byte
   table at 0x00898b1c (strength-reduced to a pointer by the compiler). 0x0042ac00 is byte-identical as a 67 B obj
   symbol (instruction score 76.2%, Ghidra size stops early). Residual: none. */
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
typedef struct { int *list; int f4; int sel; int en[13]; } MenuRow;
MGLOBAL(RwGlobalsT *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067e810, 0x0067e810);
MGLOBAL(int, DAT_0067e814, 0x0067e814);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
typedef struct { int code; int pad[12]; } FeatRow;
typedef struct { int flag; int pad[18]; } VehRow;
MGLOBALA(int, DAT_00898aa0, 4, 0x00898aa0);
MGLOBALA(FeatRow, DAT_00898b1c, 4, 0x00898b1c);
MGLOBALA(VehRow, DAT_007f105c, 64, 0x007f105c);
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
// MATCH: 0x0042fe90 FUN_0042fe90
void FUN_0042fe90(void)
{
  int *list;
  int i;
  int text;
  int tw;
  RwRGBA col;
  float ty;
  RwV2d pos;
  RwV2d size;

  if (DAT_0067e810 == 2) {
    return;
  }
  if (DAT_0067e810 == 0 && DAT_0067e814 < 0x60) {
    return;
  }
  if (DAT_0067e810 == 1) {
    list = DAT_0067ed38[DAT_0067e9f8].list;
  }
  else {
    list = DAT_0067ed38[DAT_0067e9f8 + 1].list;
  }
  ty = (float)FUN_0042ac50(FUN_0042ac00(list), 30);
  DAT_007d3ff8->fpRenderStateSet(6, 0);
  DAT_007d3ff8->fpRenderStateSet(8, 0);
  for (i = 0; i < 4; i++) {
    col.alpha = (unsigned char)DAT_0067e814;
    if (DAT_0067ed38[DAT_0067e9f8].en[i] == 0) {
      col.alpha = (unsigned char)DAT_0067e814 >> 2;
    }
    if (DAT_00898b1c[i].code != 0xc3) {
      col.red = col.green = col.blue = 0;
      pos.x = (float)FUN_0042b8b0() * 0.515625f;
      pos.y = (float)FUN_0042b8c0() * (ty - 8.0f) * 0.0020833334f;
      size.x = (float)FUN_0042b8b0() * 0.025f;
      size.y = (float)FUN_0042b8c0() * 0.033333335f;
      FUN_004739f0(FUN_0040bb50(s_Arrow_005cd828), pos, size, col, 0.9999f, 0, 0, 1.0f, 0, 0);
      text = (DAT_007f105c[DAT_00898aa0[i]].flag != 0) + 'Y';
      FUN_00427e00(text, 350.0f, ty, col, 0.7f, 0);
      tw = (int)FUN_004282a0(text, 0.7f);
      pos.x = (float)(FUN_0042b8b0() * (tw + 0x166) / 640);
      FUN_00473870(FUN_0040bb50(s_Arrow_005cd828), pos, size, col, 1);
    }
    ty += 30.0f;
  }
  DAT_007d3ff8->fpRenderStateSet(6, 1);
  DAT_007d3ff8->fpRenderStateSet(8, 1);
}
