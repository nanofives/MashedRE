// FLAGS: /O2 /Ob1 /G5
/* MATCH-RT2D-FONT 2026-11-11: 98.0% with the zero compares written as the 0.0f literal (0x005d757c is a .rdata __real@ 0.0f, as in the 100% twin 0x00557110); permuter 395 compiles no gain. MATCH-S24 0x00556e90 (h2): 97.5% (regs 97.5%), 198/198 instructions. Colour twin of 0x00557110: RGBA8 ->
   RGBAReal for c0 (+0x00, from param_5), d0 (+0x18, param_4 minus c0), c1 (+0x30, param_2), d1 (+0x48, param_3
   minus c1), raw RGBA8 copy of *param_5 at +0x60, bit 0 of +0x64 set unless all deltas are 0 and c0 == c1.
   Flags: /O2 /Ob1 /G5 97.5%, the raw converter's /O2 /GS /Ob1 gives 65.7% on this source. Residual: three
   integer copies of spilled deltas (mov edx,[esp+k]) and one reload of esi are scheduled one x87 instruction
   later than the original, and the first fcomp lands after the +0x54/+0x60 stores. Permuter 307 compiles, no
   gain. Class (a): x87/integer scheduling, same as 0x00557110. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef struct { unsigned char red, green, blue, alpha; } RGBA8;
typedef struct { float red, green, blue, alpha; } RGBAReal;
typedef struct {
  RGBAReal c0;      /* +0x00 */
  char pad0[8];
  RGBAReal d0;      /* +0x18 */
  char pad1[8];
  RGBAReal c1;      /* +0x30 */
  char pad2[8];
  RGBAReal d1;      /* +0x48 */
  char pad3[8];
  RGBA8 raw;        /* +0x60 */
  unsigned int flags; /* +0x64 */
} ColLine;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
// MATCH: 0x00556e90 FUN_00556e90
ColLine *FUN_00556e90(ColLine *param_1, RGBA8 *param_2, RGBA8 *param_3, RGBA8 *param_4, RGBA8 *param_5)
{
  param_1->c0.red = (float)param_5->red;
  param_1->c0.green = (float)param_5->green;
  param_1->c0.blue = (float)param_5->blue;
  param_1->c0.alpha = (float)param_5->alpha;
  param_1->d0.red = (float)param_4->red;
  param_1->d0.green = (float)param_4->green;
  param_1->d0.blue = (float)param_4->blue;
  param_1->d0.alpha = (float)param_4->alpha;
  param_1->d0.red = param_1->d0.red - param_1->c0.red;
  param_1->d0.green = param_1->d0.green - param_1->c0.green;
  param_1->d0.blue = param_1->d0.blue - param_1->c0.blue;
  param_1->d0.alpha = param_1->d0.alpha - param_1->c0.alpha;
  param_1->c1.red = (float)param_2->red;
  param_1->c1.green = (float)param_2->green;
  param_1->c1.blue = (float)param_2->blue;
  param_1->c1.alpha = (float)param_2->alpha;
  param_1->d1.red = (float)param_3->red;
  param_1->d1.green = (float)param_3->green;
  param_1->d1.blue = (float)param_3->blue;
  param_1->d1.alpha = (float)param_3->alpha;
  param_1->d1.red = param_1->d1.red - param_1->c1.red;
  param_1->d1.green = param_1->d1.green - param_1->c1.green;
  param_1->d1.blue = param_1->d1.blue - param_1->c1.blue;
  param_1->d1.alpha = param_1->d1.alpha - param_1->c1.alpha;
  param_1->raw = *param_5;
  if (param_1->d0.red == 0.0f && param_1->d1.red == 0.0f &&
      param_1->c0.red == param_1->c1.red &&
      param_1->d0.green == 0.0f && param_1->d1.green == 0.0f &&
      param_1->c0.green == param_1->c1.green &&
      param_1->d0.blue == 0.0f && param_1->d1.blue == 0.0f &&
      param_1->c0.blue == param_1->c1.blue &&
      param_1->d0.alpha == 0.0f && param_1->d1.alpha == 0.0f &&
      param_1->c0.alpha == param_1->c1.alpha) {
    param_1->flags &= ~1;
    return param_1;
  }
  param_1->flags |= 1;
  return param_1;
}
