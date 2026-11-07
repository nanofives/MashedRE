// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f8400 (h6): 100% (regs 100%) under /O2 /Ob1 /G5. 2D bounds of a point list (V2 = two floats).
   Moved from the raw converter's <= / else form to `min > p` / `else if (max < p)` (fld min; fcomp p; and eax,0x4100
   form) plus a do/while with a block-local count n-1. The loop starts at pts[0] and runs n-1 times (original
   behaviour, kept). Residual: none. */
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
typedef struct V2 { float x; float y; } V2;
// MATCH: 0x004f8400 FUN_004f8400
void FUN_004f8400(const V2 *pts, int n, V2 *mn, V2 *mx)
{
  mn->x = pts->x;
  mn->y = pts->y;
  mx->x = pts->x;
  mx->y = pts->y;
  if (n > 1) {
    int i = n - 1;
    do {
      if (mn->x > pts->x) mn->x = pts->x; else if (mx->x < pts->x) mx->x = pts->x;
      if (mn->y > pts->y) mn->y = pts->y; else if (mx->y < pts->y) mx->y = pts->y;
      pts++;
    } while (--i);
  }
}
