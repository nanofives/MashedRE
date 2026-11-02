// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00433f40: best 99.3% (regs 100%, ops 100%, 270/270 ins), by hand from the raw 29.6%.
   What moved it: FUN_0042aae0 takes one stack arg (Ghidra said ECX); the colours are 4-byte RGBA structs passed by
   value; the 0.6f/grey args the original pushes before a branch come from if/else with one call per branch
   (tail-merged), not a ternary; `white` is declared before `grey` (initializer store order); the dark/black/white2
   field stores in the observed order. The six switch cases are identical calls in the original yet were NOT merged
   by the compiler (registers rotate eax/ecx/edx through them in layout order 12,11,7,1,2,9): written in that order
   with different spellings of the same float constants (140.0f / 140.0 / 140), which stops this compiler merging
   them and gives the original's layout. Same values, same bytes.
   Residual (1 instruction pair): the first FUN_004282a0 arg is `add eax, edx; push eax` in the original and
   `add edx, eax; push edx` here. Tried both operand orders at both sites, a temp variable (Ghidra form), a ternary,
   inverted condition, global declaration order, and the permuter (313 compiles, no improvement). relocheck flags only
   that pair (ours loads eadc first, the original eab4 first; both add into the eadc register). Class (b). */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } MatchRGBA;
typedef int (*MatchRenderStateSet)(int state, int value);
MGLOBAL(int, DAT_0067eab0, 0x0067eab0);
MGLOBAL(int, DAT_0067eab4, 0x0067eab4);
MGLOBAL(int, DAT_0067eab8, 0x0067eab8);
MGLOBAL(int, DAT_0067eac0, 0x0067eac0);
MGLOBAL(int, DAT_0067eac8, 0x0067eac8);
MGLOBAL(int, DAT_0067eacc, 0x0067eacc);
MGLOBAL(int, DAT_0067ead4, 0x0067ead4);
MGLOBAL(int, DAT_0067eadc, 0x0067eadc);
MGLOBAL(int, DAT_0067eca8, 0x0067eca8);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
void FUN_004278d0(int id, MatchRGBA color, float scale);
void FUN_00427990(int id, float x0, float y0, float x1, float y1, MatchRGBA color, float scale, int flags);
void FUN_00427be0(char *text, MatchRGBA color, float scale);
void FUN_00427e00(int id, float x, float y, MatchRGBA color, float scale, int flags);
float FUN_004282a0(int id, float scale);
void FUN_0042aae0(int a);
void FUN_0042c010(float x, float y, float w, float h, MatchRGBA color);
void FUN_0042c090(MatchRGBA color);
int FUN_004671a0(int a);
void FUN_00492e60(void);
void FUN_004c19f0(int a);
void FUN_004c1a00(int a);
// MATCH: 0x00433f40 FUN_00433f40
void FUN_00433f40(void)
{
  MatchRGBA white = {0xff, 0xff, 0xff, 0xff};
  MatchRGBA grey = {0xdc, 0xdc, 0xdc, 0xff};
  MatchRGBA black;
  MatchRGBA white2;
  MatchRGBA dark;
  char buf[512];
  int saved;

  if (DAT_0067eab0 == 0) {
    return;
  }
  if (DAT_0067eab8 < 0x28) {
    return;
  }
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 0);
  if (DAT_0067ead4 != 0) {
    FUN_004282a0(DAT_0067eab4 + DAT_0067eadc, 0.6f);
  } else {
    FUN_004282a0(DAT_0067eab4, 0.6f);
  }
  saved = DAT_0067eca8;
  DAT_0067eca8 = ((unsigned char)DAT_0067eab8 * 3) / 4;
  FUN_0042aae0(0);
  DAT_0067eca8 = saved;
  dark.r = 0x20;
  dark.g = 0x20;
  dark.b = 0x20;
  black.r = 0;
  black.g = 0;
  black.b = 0;
  white2.r = 0xff;
  white2.g = 0xff;
  white2.b = 0xff;
  dark.a = (unsigned char)DAT_0067eab8;
  white2.a = (unsigned char)DAT_0067eab8;
  black.a = (unsigned char)DAT_0067eab8;
  grey.a = (unsigned char)DAT_0067eab8;
  white.a = (unsigned char)DAT_0067eab8;
  FUN_004c19f0(FUN_004671a0(-1));
  FUN_00492e60();
  FUN_004c1a00(FUN_004671a0(-1));
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 0);
  FUN_0042c010(130.0f, 120.0f, 380.0f, 42.0f, black);
  FUN_0042c010(130.0f, 162.0f, 380.0f, 166.0f, dark);
  FUN_0042c010(130.0f, 328.0f, 380.0f, 32.0f, black);
  FUN_0042c090(white2);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 0);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 0);
  if (DAT_0067ead4 != 0) {
    FUN_00427e00(0xb8, 140.0f, 142.0f, white, 0.8f, 0);
  } else {
    FUN_00427e00(0x41, 140.0f, 142.0f, white, 0.8f, 0);
  }
  if (DAT_0067eab4 != 0x27b) {
    if (DAT_0067ead4 != 0) {
      FUN_004278d0(DAT_0067eadc + DAT_0067eab4, grey, 0.6f);
    } else {
      FUN_004278d0(DAT_0067eab4, grey, 0.6f);
    }
  } else {
    FUN_00427be0(buf, grey, 0.6f);
    FUN_00427990(0x27c, 145.0f, 267.0f, 350.0f, 150.0f, grey, 0.55f, 0);
  }
  switch (DAT_0067eac0) {
  case 12:
    FUN_00427e00(DAT_0067eac8, 140.0f, 345.0f, grey, 0.55f, 0);
    FUN_00427e00(DAT_0067eacc, 270.0f, 345.0f, grey, 0.55f, 0);
    break;
  case 11:
    FUN_00427e00(DAT_0067eac8, 140.0, 345.0, grey, 0.55f, 0);
    break;
  case 7:
    FUN_00427e00(DAT_0067eac8, 140, 345, grey, 0.55f, 0);
    break;
  case 1:
    FUN_00427e00(DAT_0067eac8, 140.0f, 345.0, grey, 0.55f, 0);
    break;
  case 2:
    FUN_00427e00(DAT_0067eac8, 140.0, 345.0f, grey, 0.55f, 0);
    FUN_00427e00(DAT_0067eacc, 270.0, 345.0, grey, 0.55f, 0);
    break;
  case 9:
    FUN_00427e00(DAT_0067eac8, 140, 345.0f, grey, 0.55f, 0);
    FUN_00427e00(DAT_0067eacc, 270, 345, grey, 0.55f, 0);
    break;
  }
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(6, 1);
  (*(MatchRenderStateSet *)(DAT_007d3ff8 + 0x20))(8, 1);
}
