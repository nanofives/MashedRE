// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00431710 (975 B): best 85.5% (regs 91.1, ops 97.2), 280/282 ins, by hand (raw converter
   failed to compile: C2120). This file holds 0x0042ac00 (EDX-passed pointer; whole 67 B symbol
   byte-identical) and 0x0042ac50 (EAX/ECX; 100%) as `static` callees defined first (pilot finding 1).
   What moved the score: integer x for the arrows `(s * 356) / 640` and `(s * (w + 0x17e)) / 640`; the gauge
   rectangle as a struct local b (B+0x1c..0x28) passed to 0x00472c60 as `*(int *)&b.x` ... (the original
   stores every constant into b and reloads it through an integer register, which only happened once the
   fields were read through an int lvalue; with plain float args the compiler pushes the literals: 63.8% ->
   74.1%); `if (i == 2 || i == 3) w = (int)FUN_004282a0(n, 0.7f); else w = 0x66;` as its own statement after
   the i == 0 block -> 83.3%; the two dead colour-alpha stores at the loop end (0x00431aad, 0x00431ab1,
   overwritten at the loop top) written out -> 85.5%.
   NOTE for the port: FUN_00472c60 is declared here with int parameters to get the int reads; its real
   parameters are four floats and a colour, so the port must declare it with float params and pass b.x...
   Constants: 0x005cc9b8 7.0, 0x005cd8d8 13.0, 0x005cc31c 3.0, 0x005cc568 100.0, 0x005cc320 1.0,
   0x005cc72c 30.0, gauge literals 374/106/16/3/477/377/10 stored as immediates.
   Residual: the alpha/2 and alpha colour locals sit in swapped slots (B+8 / B+0xc) and the zero stores of
   one colour are placed after the loop guard; register choice for the 0x66/width value (ebx vs edx) and the
   tail of the i == 0 path (original reloads n into ebp on each exit). Tried: declaration order, statement
   order, names, MRgba arrays (73.8-75.9%), permuter (456 compiles; its one gain moved b.h = 10 after the
   call, which changes behaviour, rejected). Class (b). */
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
typedef struct { float x, y, w, h; } MRect;
typedef void (*MRsSet)(int, int);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067e820, 0x0067e820);
MGLOBAL(int, DAT_0067e824, 0x0067e824);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(float, DAT_0067eaa8, 0x0067eaa8);
MGLOBALA(MMenu, DAT_0067ed38, 1, 0x0067ed38);
MGLOBALA(MMenu, DAT_0067ed78, 1, 0x0067ed78);
MGLOBALA(char, s_Arrow_005cd828, 6, 0x005cd828);
void *FUN_0040bb50(const char *name);
float FUN_004282a0(int text, float scale);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
void FUN_00472c60(int x, int y, int w, int h, MRgba col);
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

// MATCH: 0x00431710 FUN_00431710
void FUN_00431710(void)
{
  int mode, alpha, n, i, w;
  int *items;
  MRgba bar, col, shade;
  float fy, by;
  MRect b;
  MRect r;
  bar.r = 0xd8; bar.g = 0x80; bar.b = 0x20; bar.a = 0xff;
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(8, 0);
  mode = DAT_0067e820;
  alpha = DAT_0067e824;
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
  fy = (float)FUN_0042ac50(n, 0x1e);
  for (i = 0; i < n; i++) {
    bar.a = (unsigned char)alpha;
    col.a = (unsigned char)alpha; col.b = 0; col.g = 0; col.r = 0;
    shade.a = (unsigned char)alpha >> 1; shade.b = 0; shade.g = 0; shade.r = 0;
    r.x = (float)((FUN_0042b8b0() * 356) / 640);
    r.y = (float)FUN_0042b8c0() * (fy - 8.0f) * 0.0020833334f;
    r.w = (float)FUN_0042b8b0() * 0.025f;
    r.h = (float)FUN_0042b8c0() * 0.033333335f;
    FUN_004739f0(FUN_0040bb50(s_Arrow_005cd828), r.x, r.y, r.w, r.h, col, 0.9999f, 0, 0, 1.0f, 0, 0);
    if (i == 0) {
      by = fy - 7.0f;
      b.x = 374.0f; b.y = by; b.w = 106.0f; b.h = 16.0f;
      FUN_00472c60(*(int *)&b.x, *(int *)&b.y, *(int *)&b.w, *(int *)&b.h, shade);
      b.h = 3.0f;
      FUN_00472c60(*(int *)&b.x, *(int *)&b.y, *(int *)&b.w, *(int *)&b.h, col);
      b.y += 13.0f;
      FUN_00472c60(*(int *)&b.x, *(int *)&b.y, *(int *)&b.w, *(int *)&b.h, col);
      b.x = 374.0f; b.y = by; b.w = 3.0f; b.h = 16.0f;
      FUN_00472c60(*(int *)&b.x, *(int *)&b.y, *(int *)&b.w, *(int *)&b.h, col);
      b.x = 477.0f;
      FUN_00472c60(*(int *)&b.x, *(int *)&b.y, *(int *)&b.w, *(int *)&b.h, col);
      b.x = 377.0f; b.y = by + 3.0f; b.w = DAT_0067eaa8 * 100.0f;
      if (b.w > 1.0f) {
        b.h = 10.0f;
        FUN_00472c60(*(int *)&b.x, *(int *)&b.y, *(int *)&b.w, *(int *)&b.h, bar);
      }
    }
    if (i == 2 || i == 3)
      w = (int)FUN_004282a0(n, 0.7f);
    else
      w = 0x66;
    r.x = (float)((FUN_0042b8b0() * (w + 0x17e)) / 640);
    FUN_00473870(FUN_0040bb50(s_Arrow_005cd828), r.x, r.y, r.w, r.h, col, 1);
    fy += 30.0f;
    alpha = DAT_0067e824;
    shade.a = (unsigned char)alpha;
    col.a = (unsigned char)alpha;
  }
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(6, 1);
  (*(MRsSet *)(DAT_007d3ff8 + 0x20))(8, 1);
}
