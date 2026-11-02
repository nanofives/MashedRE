// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h4 0x00430b90: BYTE-IDENTICAL whole symbol (1712 B incl. the jump table at 0x00431230; match.py --bytes).
   Instruction score 99.4% only because Ghidra's size (1685 B) stops before the case-3 tail at 0x00431225..0x0043122d
   (call __ftol; mov edx,eax; jmp 0x00430e99), which our symbol carries. Started from a hand draft (raw converter
   failed to compile, C2120).
   What moved it: (1) FUN_00472c60 / FUN_00473870 / FUN_004739f0 take two 8-byte RwV2d structs by value, not four
   floats: that is what produces the original's `mov [slot],imm; mov reg,[slot]; push reg` pairs (26.9% -> 90.5%);
   (2) the three bar cases written out (macro), not an __inline helper (the DDK does not inline it under /Ob1);
   (3) register-passed callees as static functions defined first (pilot finding 1): this file HOLDS 0x0042ac00 (EDX),
   0x0042ac50 (EAX = count, ECX = 0x1e; 100%), 0x0042b8b0 and 0x0042b8c0 (static, so EDX stays live across them);
   (4) DAT_0067e7fc read once into an int before the gate, c2/c3 rgb zeroed inside the loop (hoisted after the jle),
   and the alpha bytes re-stored from the global at the loop end before the int reload.
   0x0042ac00 scores 76.2% by instructions but its 67 B whole symbol is BYTE-IDENTICAL (Ghidra size 59 B stops early).
   Residual: none. */
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
typedef struct RwRGBA { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct RwV2d { float x, y; } RwV2d;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067e7f8, 0x0067e7f8);
MGLOBAL(int, DAT_0067e7fc, 0x0067e7fc);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBALA(int *, DAT_0067ed38, 16, 0x0067ed38);
MGLOBALA(int *, DAT_0067ed78, 16, 0x0067ed78);
MGLOBAL(int, DAT_007f0f10, 0x007f0f10);
MGLOBAL(short, DAT_0067ea54, 0x0067ea54);
MGLOBAL(short, DAT_0067ea56, 0x0067ea56);
MGLOBAL(float, DAT_007f0f00, 0x007f0f00);
MGLOBAL(float, DAT_007f0f04, 0x007f0f04);
MGLOBAL(float, DAT_007f0f08, 0x007f0f08);
MGLOBALA(char, s_Arrow_005cd828, 6, 0x005cd828);
typedef void (*MatchFn007d3ff8_20)(int, int);
void *FUN_0040bb50(const char *a1);
void FUN_00427e00(int a1, float a2, float a3, RwRGBA a4, float a5, int a6);
float FUN_004282a0(int a1, float a2);
void FUN_00472c60(RwV2d a1, RwV2d a2, RwRGBA a3);
void FUN_00473870(void *a1, RwV2d a2, RwV2d a3, RwRGBA a4, int a5);
void FUN_004739f0(void *a1, RwV2d a2, RwV2d a3, RwRGBA a4, float a5, float a6, float a7, float a8, float a9, float a10);

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

// HELD: 0x0042ac00 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
#ifndef MATCH_PORT
static int FUN_0042ac00(int *p)
{
  int n = 0;
  if (p == 0)
    return 0;
  for (;;) {
    while (*p != (int)0xff060000) {
      if (*p == (int)0xff070000)
        return n;
      p++;
    }
    if (*p == (int)0xff070000)
      return n;
    n++;
    p++;
  }
}
#else
int FUN_0042ac00(int *p);
#endif

// HELD: 0x0042ac50 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
#ifndef MATCH_PORT
static int FUN_0042ac50(unsigned int n, int k)
{
  int y;
  if (!(n & 1))
    y = 0xf0 - k / 2;
  else
    y = 0xf0;
  return y - (n - 1) / 2 * k;
}
#else
int FUN_0042ac50(unsigned int n, int k);
#endif

/* the five quads and the gated sixth of rows 0..2 (cases 0x00430d54 / 0x00430f3d / 0x0043107a), written out by
   this macro so each case keeps its own copy as in the original */
#define ROW_QUADS(g)   pos.x = 374.0f; pos.y = fy - 7.0f; size.x = 106.0f; size.y = 16.0f;   FUN_00472c60(pos, size, c2);   size.y = 3.0f;   FUN_00472c60(pos, size, c3);   pos.y += 13.0f;   FUN_00472c60(pos, size, c3);   pos.x = 374.0f; pos.y = fy - 7.0f; size.x = 3.0f; size.y = 16.0f;   FUN_00472c60(pos, size, c3);   pos.x = 477.0f;   FUN_00472c60(pos, size, c3);   pos.x = 377.0f; pos.y = fy - 7.0f + 3.0f; size.x = (g) * 100.0f;   if (size.x > 1.0f) { size.y = 10.0f; FUN_00472c60(pos, size, c1); }

// MATCH: 0x00430b90 FUN_00430b90
void FUN_00430b90(void)
{
  RwRGBA c1 = { 0xd8, 0x80, 0x20, 0xff };
  RwRGBA c2, c3;
  int *p;
  int n, i, y, id, w;
  float fy;
  RwV2d apos, asize;
  RwV2d pos, size;
  int a;

  (*(MatchFn007d3ff8_20 *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MatchFn007d3ff8_20 *)(DAT_007d3ff8 + 0x20))(8, 0);
  a = DAT_0067e7fc;
  if (DAT_0067e7f8 == 0 && a < 0x60) {
    (*(MatchFn007d3ff8_20 *)(DAT_007d3ff8 + 0x20))(6, 1);
    (*(MatchFn007d3ff8_20 *)(DAT_007d3ff8 + 0x20))(8, 1);
    return;
  }
  if (DAT_0067e7f8 == 1)
    p = DAT_0067ed38[DAT_0067e9f8 * 16];
  else
    p = DAT_0067ed78[DAT_0067e9f8 * 16];
  n = FUN_0042ac00(p);
  y = FUN_0042ac50(n, 0x1e);
  fy = (float)y;
#ifdef MATCH_PORT
  /* port only (not compiled by the DDK): the original's case-3 `id` lives in the stack slot that holds y
     (0x00430c35 mov [esp+0x20],eax at esp-0x48 = 0x004311c0 [esp+0x24] at esp-0x4c), so when DAT_007f0f10
     is not 0..2 the id passed to 0x00427e00 / 0x004282a0 is this 0x0042ac50 result. C leaves it uninitialized. */
  id = y;
#endif
  for (i = 0; i < n; i++) {
    c3.red = c3.green = c3.blue = 0;
    c2.red = c2.green = c2.blue = 0;
    c1.alpha = (unsigned char)a;
    c3.alpha = (unsigned char)a;
    c2.alpha = (unsigned char)a >> 1;
    apos.x = (float)((FUN_0042b8b0() * 0x164) / 0x280);
    apos.y = (fy - 8.0f) * FUN_0042b8c0() * 0.0020833334f;
    asize.x = FUN_0042b8b0() * 0.025f;
    asize.y = FUN_0042b8c0() * 0.033333335f;
    FUN_004739f0(FUN_0040bb50(s_Arrow_005cd828), apos, asize, c3, 0.9999f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    switch (i) {
    case 0:
      ROW_QUADS(DAT_007f0f04)
      break;
    case 1:
      ROW_QUADS(DAT_007f0f00)
      break;
    case 2:
      ROW_QUADS(DAT_007f0f08)
      break;
    case 3:
      if (DAT_007f0f10 == 0)
        id = 0x59;
      else if (DAT_007f0f10 == 1)
        id = 0x1b2;
      else if (DAT_007f0f10 == 2)
        id = 0x1b1;
      FUN_00427e00(id, 376.0f, fy, c3, 0.7f, 0);
      break;
    }
    if (i == 3)
      w = (int)FUN_004282a0(id, 0.7f);
    else
      w = 0x66;
    apos.x = (float)((FUN_0042b8b0() * (w + 0x17e)) / 0x280);
    FUN_00473870(FUN_0040bb50(s_Arrow_005cd828), apos, asize, c3, 1);
    fy += 30.0f;
    c2.alpha = (unsigned char)DAT_0067e7fc;
    c3.alpha = (unsigned char)DAT_0067e7fc;
    a = DAT_0067e7fc;
  }
  (*(MatchFn007d3ff8_20 *)(DAT_007d3ff8 + 0x20))(6, 1);
  (*(MatchFn007d3ff8_20 *)(DAT_007d3ff8 + 0x20))(8, 1);
}
