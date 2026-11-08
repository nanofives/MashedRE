// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR05 2026-11-08: 86.7% regs 86.7% (was 69.3). Re-derived with typed structs (V3 *p, L {E e[4]; uint n @0xac}, E {V3 pos @0xc; a; b}): e->pos = *p struct copy gives the original's lea esi,[eax+0xc] tail and store order; tol >= 0.0f literal gives fld tol / fcomp const; e = &l->e[n] with e[-1].pos gives the second block's -0x1c/-0x18/-0x14 addressing. Residual: the original has fxch st(1) before the first faddp of both sums and a private xor/pop/ret for the first return 0; 12 orders x 2 associativities and 8 cast combinations did not produce the fxch. Class b. Supersedes the header below. */
/* MATCH-RWL06 0x00575120: hand-matched (h2). 69.3% regs 73.3% ops 84.0% under FLAGS /O2 /Ob1 /G5 (raw file was /O1 at 6.5%). Float abs sums via (float)fabs, <= compares, entry stores as 32-bit integer copies through an entry struct. Residual: original keeps two separate 'return 0' epilogues and builds esi=entry+0xc as a second base (push edi), plus an fxch in the second abs sum; ours merges the returns. Class b. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
double fabs(double x);
#pragma intrinsic(fabs)
typedef struct { float x, y, z; } V3;
typedef struct { char pad[0xc]; V3 pos; undefined4 a; undefined4 b; char pad2[0x28 - 0x20]; } E;
typedef struct { E e[4]; char pad[0xac - 0xa0]; uint n; } L;
// MATCH: 0x00575120 FUN_00575120
undefined4 FUN_00575120(V3 *p,undefined4 param_2,L *l,float tol)
{
  uint n;
  E *e;
  float d;

  n = l->n;
  if (n != 0 && tol >= 0.0f) {
    if ((float)fabs(l->e[0].pos.z - p->z) + (float)fabs(l->e[0].pos.y - p->y) + (float)fabs(l->e[0].pos.x - p->x) <= tol) {
      return 0;
    }
    if (1 < n) {
      e = &l->e[n];
      if ((float)fabs(e[-1].pos.x - p->x) + (float)fabs(e[-1].pos.y - p->y) + (float)fabs(e[-1].pos.z - p->z) <= tol) {
        return 0;
      }
    }
  }
  e = &l->e[n];
  e->pos = *p;
  e->b = 0;
  e->a = param_2;
  l->n++;
  return 1;
}
