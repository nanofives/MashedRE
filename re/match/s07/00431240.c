// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00431240 (622 B): best 98.9% (regs 98.9, ops 99.5), 184/184 ins, by hand (raw g2s 28.3%).
   This file holds 0x0042ac00 (item counter, EDX-passed pointer; whole 67 B symbol byte-identical, Ghidra's
   size stops at 59 B) and 0x0042ac50 (EAX = count, ECX = spacing; 100%) as `static` callees defined first
   (pilot finding 1: register-passed args). What moved the score:
   - static callees with parameter order (n, h) for 0x0042ac50 (EAX/ECX) and the loop of 0x0042ac00 as
     `while (*p != 0xff060000 && *p != 0xff070000) p++; if (*p == 0xff070000) return n;` -> 54.5%;
   - one call to FUN_004282a0 with a ternary text id, and the 4 arrow coordinates as one struct local
     (MRect, fixed slots 0x20..0x2c instead of packed temps) -> 84%;
   - float literals instead of the .rdata constants as globals (356.0f, 20.0f, 26.0f, 1/640, 1/480, ...): the
     compiler then keeps `fld w; fadd 20` order and pops the dead sum in the 356 branch -> 98.9%.
   Constants: 0x005cd8e0 356.0, 0x005ccd6c 20.0, 0x005cd8dc 26.0, 0x005cd5a8 0.0015625, 0x005cc9f4 8.0,
   0x005cc560 0.0020833334, 0x005cc9a4 0.025, 0x005cc324 0.033333335; 0x3f7ff972 = 0.9999f; 0x004a2c48 = _ftol.
   Residual (2 ins): the int scratch store + fild for r.h sit one push later in the original
   (0x004313bd/0x004313c8). Tried: arg types of the zero/1.0 args (int/float/pointer), r.h through a short/int
   temp, statement order of r.w/r.h, a separate colour copy, permuter (233 compiles, no gain). Class (b),
   instruction scheduling around the pushes, same residual as 0x004314b0. */
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
typedef struct { unsigned char r, g, b, a; } MRgba;
typedef struct { int *items; int pad[15]; } MMenu;
typedef struct { float w; int pad[3]; int t1; int t2; int pad2[7]; } MOpt;
typedef void (*MRsSet)(int, int);
typedef struct { float x, y, w, h; } MRect;
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067e838, 0x0067e838);
MGLOBAL(int, DAT_0067e83c, 0x0067e83c);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBALA(MMenu, DAT_0067ed38, 1, 0x0067ed38);
MGLOBALA(MMenu, DAT_0067ed78, 1, 0x0067ed78);
MGLOBALA(MOpt, DAT_00898aa4, 1, 0x00898aa4);
MGLOBALA(char, s_Arrow_005cd828, 6, 0x005cd828);
int FUN_0040ad20(void);
void *FUN_0040bb50(const char *name);
void FUN_00427e00(int c, float x, float y, MRgba col, float scale, int flags);
float FUN_004282a0(int text, float scale);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
void FUN_00473870(void *tex, float x, float y, float w, float h, MRgba col, int flags);
void FUN_004739f0(void *tex, float x, float y, float w, float h, MRgba col, float z, int a, int b, float u, int c, int d);

// MATCH: 0x0042ac00 FUN_0042ac00
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

// MATCH: 0x0042ac50 FUN_0042ac50
static int FUN_0042ac50(unsigned int n, int h)
{
  int y;
  if ((n & 1) == 0) y = 0xf0 - h / 2; else y = 0xf0;
  return y - ((n - 1) >> 1) * h;
}

// MATCH: 0x00431240 FUN_00431240
void FUN_00431240(void)
{
  int mode, alpha, n, iy, tw;
  int *items;
  int c;
  MRgba col;
  float fy, w, f;
  MRect r;
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(8, 0);
  mode = DAT_0067e838;
  alpha = DAT_0067e83c;
  if (mode == 0 && alpha < 0x60) {
    (*(MRsSet *)(DAT_007d3ff8 + 0x20))(6, 1);
    (*(MRsSet *)(DAT_007d3ff8 + 0x20))(8, 1);
    return;
  }
  if (mode == 1)
    items = DAT_0067ed38[DAT_0067e9f8].items;
  else
    items = DAT_0067ed78[DAT_0067e9f8].items;
  n = FUN_0042ac00(items);
  col.a = (unsigned char)alpha; col.b = 0; col.g = 0; col.r = 0;
  iy = FUN_0042ac50(n, 0x1e);
  fy = (float)iy;
  f = FUN_004282a0(mode == 1 ? DAT_00898aa4[n].t1 : DAT_00898aa4[n].t2, 0.8f) + DAT_00898aa4[n].w;
  if (f < 356.0f) w = 356.0f; else w = f + 20.0f;
  r.x = (float)FUN_0042b8b0() * w * 0.0015625f;
  r.y = (float)FUN_0042b8c0() * (fy - 8.0f) * 0.0020833334f;
  r.w = (float)FUN_0042b8b0() * 0.025f;
  r.h = (float)FUN_0042b8c0() * 0.033333335f;
  FUN_004739f0(FUN_0040bb50(s_Arrow_005cd828), r.x, r.y, r.w, r.h, col, 0.9999f, 0, 0, 1.0f, 0, 0);
  c = (FUN_0040ad20() != 0) + 'Y';
  FUN_00427e00(c, w + 20.0f, fy, col, 0.7f, 0);
  tw = (int)FUN_004282a0(c, 0.7f);
  r.x = (float)FUN_0042b8b0() * ((float)tw + w + 26.0f) * 0.0015625f;
  FUN_00473870(FUN_0040bb50(s_Arrow_005cd828), r.x, r.y, r.w, r.h, col, 1);
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(6, 1);
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(8, 1);
}
