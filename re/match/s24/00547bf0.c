// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00547bf0 (h4): 46.0% (regs 49.9%, ops 95.6%), 1932 vs 1949 instructions. Structure pass only
   (5807 B, 60-minute cap). Triangle (param_2..4) against RwBBox (param_1: sup at +0, inf at +0xc): outcodes per
   vertex (1/2, 4/8, 0x10/0x20), trivial accept/reject, then each edge whose outcodes share no bit is tested
   against the faces its outcode XOR names, then the box diagonal picked by the plane normal's signs is tested
   against the triangle. The per-face and per-edge code is written as macros so every hit is its own
   `return 1` (the original repeats the epilogue at each hit; __forceinline helpers returning a flag merge them,
   1708 instructions). Moved the score: macro form (16.3 raw -> 26.8), each comparison written with the
   product of the edge's own offset on the left as the original loads it (44.1), the plane test with the
   negative-denominator branch first and the zero constant on the left (46.0). Residual (b): stack slot and
   register assignment (the original keeps the outcodes in the dead parameter slots and the edge deltas at
   +0x10..+0x30; ours packs the deltas into the parameter slots), so most stack offsets differ. Tried: the edge
   locals at function scope (41.9). Not run through the permuter (size). */
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
typedef struct RwV3d { float x; float y; float z; } RwV3d;
typedef struct RwBBox { RwV3d sup; RwV3d inf; } RwBBox;
MGLOBAL(float, DAT_005d757c, 0x005d757c);

static __forceinline int Outcode547bf0(const RwBBox *box, const RwV3d *p)
{
  int cx, cy, cz;
  if (p->x > box->sup.x) cx = 1; else if (p->x < box->inf.x) cx = 2; else cx = 0;
  if (p->y > box->sup.y) cy = 4; else if (p->y < box->inf.y) cy = 8; else cy = 0;
  if (p->z > box->sup.z) cz = 0x10; else if (p->z < box->inf.z) cz = 0x20; else cz = 0;
  return cz | cy | cx;
}

/* one box face against edge a->b: q = a - plane on axis A, (sB, iB) / (sC, iC) = a - sup / a - inf on the
   other two axes, d = b - a */
#define FACE_547BF0(q, dA, sB, iB, dB, sC, iC, dC)                                 \
  {                                                                                \
    float c_ = (q) * (dB);                                                         \
    if ((dA) < DAT_005d757c) {                                                     \
      if (c_ < (sB) * (dA) && c_ > (iB) * (dA) && (q) * (dC) < (sC) * (dA) &&      \
          (q) * (dC) > (iC) * (dA))                                                \
        return 1;                                                                  \
    } else {                                                                       \
      if (c_ > (sB) * (dA) && c_ < (iB) * (dA) && (q) * (dC) > (sC) * (dA) &&      \
          (q) * (dC) < (iC) * (dA))                                                \
        return 1;                                                                  \
    }                                                                              \
  }

#define EDGE_547BF0(box, a, b, code)                                               \
  {                                                                                \
    float dx = (b)->x - (a)->x;                                                    \
    float dy = (b)->y - (a)->y;                                                    \
    float dz = (b)->z - (a)->z;                                                    \
    float ix = (a)->x - (box)->inf.x;                                              \
    float iy = (a)->y - (box)->inf.y;                                              \
    float iz = (a)->z - (box)->inf.z;                                              \
    float sx = (a)->x - (box)->sup.x;                                              \
    float sy = (a)->y - (box)->sup.y;                                              \
    float sz = (a)->z - (box)->sup.z;                                              \
    if ((code) & 1) FACE_547BF0(sx, dx, sy, iy, dy, sz, iz, dz)                    \
    if ((code) & 2) FACE_547BF0(ix, dx, sy, iy, dy, sz, iz, dz)                    \
    if ((code) & 4) FACE_547BF0(sy, dy, sz, iz, dz, sx, ix, dx)                    \
    if ((code) & 8) FACE_547BF0(iy, dy, sz, iz, dz, sx, ix, dx)                    \
    if ((code) & 0x10) FACE_547BF0(sz, dz, sx, ix, dx, sy, iy, dy)                 \
    if ((code) & 0x20) FACE_547BF0(iz, dz, sx, ix, dx, sy, iy, dy)                 \
  }

// MATCH: 0x00547bf0 FUN_00547bf0
int FUN_00547bf0(const RwBBox *param_1, const RwV3d *param_2, const RwV3d *param_3, const RwV3d *param_4)
{
  int c0, c1, c2;
  RwV3d e1, e2, n, m;
  float wx, wy, wz, ox, oy, oz, num, den, t;

  c0 = Outcode547bf0(param_1, param_2);
  if (c0 == 0) return 1;
  c1 = Outcode547bf0(param_1, param_3);
  if (c1 == 0) return 1;
  c2 = Outcode547bf0(param_1, param_4);
  if (c2 == 0) return 1;
  if ((c0 & c2 & c1) != 0) return 0;
  if ((c0 & c1) == 0) EDGE_547BF0(param_1, param_2, param_3, c1 ^ c0)
  if ((c0 & c2) == 0) EDGE_547BF0(param_1, param_2, param_4, c2 ^ c0)
  if ((c2 & c1) == 0) EDGE_547BF0(param_1, param_3, param_4, c2 ^ c1)

  e1.x = param_3->x - param_2->x;
  e1.y = param_3->y - param_2->y;
  e1.z = param_3->z - param_2->z;
  e2.x = param_4->x - param_2->x;
  e2.y = param_4->y - param_2->y;
  e2.z = param_4->z - param_2->z;
  n.x = e2.z * e1.y - e2.y * e1.z;
  n.y = e2.x * e1.z - e2.z * e1.x;
  n.z = e2.y * e1.x - e2.x * e1.y;
  wx = param_1->sup.x - param_1->inf.x;
  ox = param_1->inf.x - param_2->x;
  if ((*(int *)&n.y ^ *(int *)&n.x) < 0) {
    wy = param_1->inf.y - param_1->sup.y;
    oy = param_1->sup.y;
  } else {
    wy = param_1->sup.y - param_1->inf.y;
    oy = param_1->inf.y;
  }
  oy -= param_2->y;
  if ((*(int *)&n.x ^ *(int *)&n.z) < 0) {
    wz = param_1->inf.z - param_1->sup.z;
    oz = param_1->sup.z;
  } else {
    wz = param_1->sup.z - param_1->inf.z;
    oz = param_1->inf.z;
  }
  oz -= param_2->z;
  den = -(wy * n.y + wx * n.x + wz * n.z);
  num = oy * n.y + ox * n.x + oz * n.z;
  if (den < DAT_005d757c) {
    if (DAT_005d757c > num && num > den) {
      m.x = wz * oy - oz * wy;
      m.y = oz * wx - wz * ox;
      m.z = wy * ox - oy * wx;
      t = m.y * e2.y + m.x * e2.x + m.z * e2.z;
      if (DAT_005d757c > t && t > den) {
        num = -(m.y * e1.y + m.x * e1.x + m.z * e1.z);
        if (DAT_005d757c > num && t + num > den) return 1;
      }
    }
  } else if (DAT_005d757c < num && num < den) {
    m.x = wz * oy - oz * wy;
    m.y = oz * wx - wz * ox;
    m.z = wy * ox - oy * wx;
    t = m.y * e2.y + m.x * e2.x + m.z * e2.z;
    if (DAT_005d757c < t && t < den) {
      num = -(m.y * e1.y + m.x * e1.x + m.z * e1.z);
      if (DAT_005d757c < num && t + num < den) return 1;
    }
  }
  return 0;
}
#undef FACE_547BF0
#undef EDGE_547BF0
