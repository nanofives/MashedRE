// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042f530: 96.9% (regs 99.0%) by hand (raw 59.8%). FUN_004c7760 takes 3 args (the &rect push before
   FUN_004671a0(-1) is its third), the four DAT_00899150.. stores come from the scalars, not the struct.
   Residual (b): the x clamp. Original computes t in eax, clamps in eax (esi = t, eax = 0) and copies to edi at the
   join; every form tried (if/else, ternary, t = 0 then x = t, x = t before the if, ex = 0 first, two ifs, struct
   field as the clamp target) either coalesces x into edi before the branch (this file) or loses more. Permuter
   (205 compiles) found nothing better. */
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
typedef struct { int x; int y; int w; int h; } Rect0042f530;
MGLOBAL(int, DAT_0067ed68, 0x0067ed68);
MGLOBAL(int, DAT_0067f190, 0x0067f190);
MGLOBAL(int, DAT_00899150, 0x00899150);
MGLOBAL(int, DAT_00899154, 0x00899154);
MGLOBAL(int, DAT_00899158, 0x00899158);
MGLOBAL(int, DAT_0089915c, 0x0089915c);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
int FUN_004671a0(int p0);
void *FUN_004c7760(void *p0, void *p1, Rect0042f530 *p2);
// MATCH: 0x0042f530 FUN_0042f530
int FUN_0042f530(void)
{
  Rect0042f530 r;
  int x;
  int y;
  int w;
  int h;
  int ex;
  int t;
  t = (FUN_0042b8b0() * (DAT_0067ed68 - 0x127)) / 0x280 + 2;
  x = t;
  ex = 0;
  if (t < 0) {
    x = 0;
    ex = t;
  }
  r.x = x;
  y = (FUN_0042b8c0() * 100) / 0x1e0 + 2;
  r.y = y;
  w = (FUN_0042b8b0() * (ex + 0x100)) / 0x280 - 4;
  r.w = w;
  if (w < 1) {
    return 0;
  }
  h = (FUN_0042b8c0() * 0xdc) / 0x1e0 - 4;
  r.h = h;
  DAT_00899150 = x;
  DAT_00899154 = y;
  DAT_00899158 = w;
  DAT_0089915c = h;
  FUN_004c7760(*(void **)(DAT_0067f190 + 0x60), *(void **)(FUN_004671a0(-1) + 0x60), &r);
  FUN_004c7760(*(void **)(DAT_0067f190 + 100), *(void **)(FUN_004671a0(-1) + 100), &r);
  return 1;
}
