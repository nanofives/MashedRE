// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00547450 (h2): 66.8% (regs 66.8%, ops 87.4%), 621/641 instructions. Sphere (center, radius
   at +0xc) against triangle v0/v1/v2: per-axis sign-bit rejection of the three vertices relative to the
   centre (float bits read as int), the unit normal (cross product of v1-v0 and v2-v0, FUN_004c3b90 =
   inverse sqrt) written to param_5, plane distance test against the radius with -d written to param_6,
   then vertex-inside (|v|^2 <= r^2 -> 1), nearest-vertex region rejection, and three edge rejections.
   Raw converter 24.7%. Structure and every branch are in place (ops 87.4%). Tried: param slot reuse for the
   sign temps (matches), the squared lengths as three floats / V3d / array, the edge point stored into
   the same V3d as the squared lengths (+1.9), local normal components, operand swaps on the scale and
   compares, permuter (452 compiles, one compare swap +1.7), game flags /O2 /GS /Ob1 60.7%, /O1 13.1%.
   Residuals: (1) frame layout: the original places the squared lengths / edge point at esp+0x24 and the
   second vertex at esp+0x30, ours the reverse, which shifts every operand that touches them; (2) x87
   operand order (fld r; fsubr [x] for x - r, the normal scale and length products) that no source order
   moved; (3) the original duplicates each `return 1` epilogue, ours shares one. Class (b): frame layout
   not found (plus (a) x87 operand order). */
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
typedef struct { float x, y, z; } V3d;
typedef struct { V3d center; float radius; } Sphere;
MGLOBAL(float, DAT_005d757c, 0x005d757c);  /* 0.0 */
float FUN_004c3b90(float lenSq);
#define FI(f) (*(int *)&(f))
#define SIDE_TEST(A, B, C)                                                      \
  s = (FI(A) >> 31) + (FI(B) >> 31) + (FI(C) >> 31);                            \
  if (s == 0) {                                                                 \
    t0 = (A) - sphere->radius;                                                  \
    t1 = (B) - sphere->radius;                                                  \
    t2 = (C) - sphere->radius;                                                  \
    if ((FI(t0) | FI(t1) | FI(t2)) >= 0) {                                      \
      return 0;                                                                 \
    }                                                                           \
  }                                                                             \
  else if (s == -3) {                                                           \
    t0 = (A) + sphere->radius;                                                  \
    t1 = (B) + sphere->radius;                                                  \
    t2 = (C) + sphere->radius;                                                  \
    if ((FI(t0) & FI(t1) & FI(t2)) < 0) {                                       \
      return 0;                                                                 \
    }                                                                           \
  }
#define DOT(u, v) (((u).x * (v).x + (u).y * (v).y) + (u).z * (v).z)
#define EDGE_TEST(P, Q, R)                                                      \
  e.x = (Q).x - (P).x;                                                          \
  e.y = (Q).y - (P).y;                                                          \
  e.z = (Q).z - (P).z;                                                          \
  t = DOT(P, e) / DOT(e, e);                                                    \
  sq.x = (P).x - e.x * t;                                                        \
  sq.y = (P).y - e.y * t;                                                        \
  sq.z = (P).z - e.z * t;                                                        \
  dp = DOT(sq, sq);                                                               \
  if (dp > r2 && dp < DOT(sq, R)) {                                              \
    return 0;                                                                   \
  }

// MATCH: 0x00547450 FUN_00547450
int FUN_00547450(Sphere *sphere, V3d *v0, V3d *v1, V3d *v2, V3d *normal, float *dist)
{
  V3d a, b, c, e, p;
  V3d sq;
  float dp, r2, t, d, lenSq, inv;
  float t0, t1, t2;
  int s;

  a.x = v0->x - sphere->center.x;
  a.y = v0->y - sphere->center.y;
  a.z = v0->z - sphere->center.z;
  b.x = v1->x - sphere->center.x;
  b.y = v1->y - sphere->center.y;
  b.z = v1->z - sphere->center.z;
  c.x = v2->x - sphere->center.x;
  c.y = v2->y - sphere->center.y;
  c.z = v2->z - sphere->center.z;
  SIDE_TEST(a.x, b.x, c.x)
  SIDE_TEST(a.y, b.y, c.y)
  SIDE_TEST(a.z, b.z, c.z)
  {
    V3d e1, e2;
    e1.x = v1->x - v0->x;
    e1.y = v1->y - v0->y;
    e1.z = v1->z - v0->z;
    e2.x = v2->x - v0->x;
    e2.y = v2->y - v0->y;
    e2.z = v2->z - v0->z;
    normal->x = e1.y * e2.z - e1.z * e2.y;
    normal->y = e1.z * e2.x - e1.x * e2.z;
    normal->z = e1.x * e2.y - e1.y * e2.x;
  }
  lenSq = DOT(*normal, *normal);
  if (DAT_005d757c >= lenSq) {
    return 0;
  }
  inv = FUN_004c3b90(lenSq);
  normal->x *= inv;
  normal->y *= inv;
  normal->z *= inv;
  d = DOT(*normal, a);
  if (d < -sphere->radius) {
    return 0;
  }
  if (d > sphere->radius) {
    return 0;
  }
  *dist = -d;
  r2 = sphere->radius * sphere->radius;
  sq.x = DOT(a, a);
  if (sq.x <= r2) {
    return 1;
  }
  sq.y = DOT(b, b);
  if (sq.y <= r2) {
    return 1;
  }
  sq.z = DOT(c, c);
  if (sq.z <= r2) {
    return 1;
  }
  if (sq.x < sq.y) {
    if (sq.z < sq.x) {
      goto nearC;
    }
    if (DOT(b, a) > sq.x && DOT(c, a) > sq.x) {
      return 0;
    }
  }
  else if (sq.z < sq.y) {
nearC:
    if (DOT(c, a) > sq.z && DOT(c, b) > sq.z) {
      return 0;
    }
  }
  else {
    if (DOT(b, a) > sq.y && DOT(c, b) > sq.y) {
      return 0;
    }
  }
  EDGE_TEST(a, b, c)
  EDGE_TEST(b, c, a)
  EDGE_TEST(c, a, b)
  return 1;
}
