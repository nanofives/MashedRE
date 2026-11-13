/* MATCH-RWT-S3 (Opus high) 2026-11-13: 100% BYTE-IDENTICAL under rw9044 (79.2% before). Lever: comma rvalues
   (0, pl->normal.y) and (0, pl->normal.z) in the dot, grouped z + (x + y) as in the matched sibling 0x004cbb70.
   A plain field load lets c2 9044 swap `n * c` to `c * n` and consume the dying corner value in place
   (fxch; fmul [n]); the comma node is not swapped, so it emits fld [ecx+4]; fmul st(2) and fld [ecx+8];
   fmul st(1) and pops the dead y and z after fnstsw (fstp st(1); fstp st(0) at 0x004cbc28), as the original.
   The x term keeps the plain load and is consumed (fxch st(3); fmul [ecx]). */
// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWT-S3 2026-11-13: 79.2% under rw9044 (re-score before the edit: 77.4%, not the 75.5% listed); n = 6 moved after pl = (permuter swap, fixes the lea/mov order at 0x004cbbd8). Unchanged residual (b) below. Also tried: inline dot helper (ptr and by value), x/y/z inline one at a time, while/for loop forms, inner-scope V3d + dot local, compare forms, flags /G4 /G6 /Op /Oy- /Oi- (none above 79.2%).
   MATCH-S20 0x004cbbd0 (h2): 75.5% (regs 77.4%). Raw 67.9%. Frustum/bbox test: per plane the closest corner
   coordinates are picked from the bbox with (&box->sup)[1 - closest] into a local V3d, then dot > distance returns
   0. Tried float locals, local V3d struct (+7.6), dot in 4 orders, separate dot statement, permuter (no gain).
   Residual (b): the original keeps the three corner values on the FPU stack and multiplies by register (fld n.y;
   fmul st(2); fxch) then pops the two leftovers; ours multiplies by memory. The source keeps y and z alive past the
   compare in some form not found. */
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
typedef unsigned char byte;
typedef struct { float x, y, z; } V3d;
typedef struct { V3d normal; float distance; unsigned char cx, cy, cz, pad; } FrustumPlane;
typedef struct { V3d sup; V3d inf; } BBox;
typedef struct { char pad[0x94]; FrustumPlane planes[6]; } Camera;
// MATCH: 0x004cbbd0 FUN_004cbbd0
undefined4 FUN_004cbbd0(Camera *param_1,BBox *param_2)
{
  const FrustumPlane *pl;
  int n;
  V3d c;
  pl = param_1->planes;
  n = 6;
  do {
    c.x = (&param_2->sup)[1 - pl->cx].x;
    c.y = (&param_2->sup)[1 - pl->cy].y;
    c.z = (&param_2->sup)[1 - pl->cz].z;
    if ((0, pl->normal.z) * c.z + (pl->normal.x * c.x + (0, pl->normal.y) * c.y) > pl->distance) return 0;
    pl++;
  } while (--n);
  return 1;
}
