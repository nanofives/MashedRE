// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x004314b0 (602 B): best 94.4% (regs 95.5, ops 98.9), 177/177 ins, by hand (raw g2s 14.1%).
   Same family as 0x00431240. This file holds 0x0042ac50 (EAX = count, ECX = spacing; 100%) as a `static`
   callee defined first (pilot finding 1). What moved the score: menu pointer compared with 0x005f6f90
   (count 2, first 1, else count 4, first 0), loop `for (i = 0; i < cnt; i++)` over DAT_007f0f30[first + i]
   (the compiler strength-reduces it to the ebp pointer and the [esp+0x18] down-counter), float literals for
   the .rdata constants (0x005cd8e8 0.55625, 0x005cd8e4 382.0, 0x005cc72c 30.0, 376.0f pushed as
   0x43bc0000), the 4 arrow coordinates as one struct local (fixed slots) -> 94.4%.
   Residual (10 ins): the int scratch store + fild for r.h one push earlier than the original (0x004315fc),
   and `push edi` (colour) scheduled before the x2 fild sequence instead of after it (0x004316aa).
   Tried: x2 passed directly as an argument (86.4%), a pointer + down-counter loop (87.0%), permuter (215
   compiles, no gain). Class (b), instruction scheduling around the pushes. */
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
typedef void (*MRsSet)(int, int);
typedef struct { float x, y, w, h; } MRect;
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067e830, 0x0067e830);
MGLOBAL(int, DAT_0067e834, 0x0067e834);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBALA(MMenu, DAT_0067ed38, 1, 0x0067ed38);
MGLOBALA(MMenu, DAT_0067ed78, 1, 0x0067ed78);
MGLOBALA(int, DAT_005f6f90, 4, 0x005f6f90);
MGLOBALA(int, DAT_007f0f30, 4, 0x007f0f30);
MGLOBALA(char, s_Arrow_005cd828, 6, 0x005cd828);
void *FUN_0040bb50(const char *name);
void FUN_00427e00(int c, float x, float y, MRgba col, float scale, int flags);
float FUN_004282a0(int text, float scale);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
void FUN_00473870(void *tex, float x, float y, float w, float h, MRgba col, int flags);
void FUN_004739f0(void *tex, float x, float y, float w, float h, MRgba col, float z, int a, int b, float u, int c, int d);

// MATCH: 0x0042ac50 FUN_0042ac50
static int FUN_0042ac50(unsigned int n, int h)
{
  int y;
  if ((n & 1) == 0) y = 0xf0 - h / 2; else y = 0xf0;
  return y - ((n - 1) >> 1) * h;
}

// MATCH: 0x004314b0 FUN_004314b0
void FUN_004314b0(void)
{
  int mode, alpha, cnt, first, i, tw;
  int *items;
  int c;
  MRgba col;
  float fy;
  MRect r;
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(8, 0);
  mode = DAT_0067e830;
  alpha = DAT_0067e834;
  if (mode == 0 && alpha < 0x60) {
    (*(MRsSet *)(DAT_007d3ff8 + 0x20))(6, 1);
    (*(MRsSet *)(DAT_007d3ff8 + 0x20))(8, 1);
    return;
  }
  first = 0;
  cnt = 4;
  if (mode == 1)
    items = DAT_0067ed38[DAT_0067e9f8].items;
  else
    items = DAT_0067ed78[DAT_0067e9f8].items;
  if (items == DAT_005f6f90) {
    cnt = 2;
    first = 1;
  }
  fy = (float)FUN_0042ac50(cnt, 0x1e);
  col.a = (unsigned char)alpha; col.b = 0; col.g = 0; col.r = 0;
  for (i = 0; i < cnt; i++) {
    r.x = (float)FUN_0042b8b0() * 0.55625f;
    r.y = (float)FUN_0042b8c0() * (fy - 8.0f) * 0.0020833334f;
    r.w = (float)FUN_0042b8b0() * 0.025f;
    r.h = (float)FUN_0042b8c0() * 0.033333335f;
    FUN_004739f0(FUN_0040bb50(s_Arrow_005cd828), r.x, r.y, r.w, r.h, col, 0.9999f, 0, 0, 1.0f, 0, 0);
    c = (DAT_007f0f30[first + i] != 0) + 'Y';
    FUN_00427e00(c, 376.0f, fy, col, 0.7f, 0);
    tw = (int)FUN_004282a0(c, 0.7f);
    r.x = (float)FUN_0042b8b0() * ((float)tw + 382.0f) * 0.0015625f;
    FUN_00473870(FUN_0040bb50(s_Arrow_005cd828), r.x, r.y, r.w, r.h, col, 1);
    fy += 30.0f;
  }
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(6, 1);
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(8, 1);
}
