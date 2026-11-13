// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00443dc0: 57.5% (regs 59.0%, ops 91.2%) by hand (h8), raw 26.9%. Structure: spline as
   { float x, y } pt[64]; int n, a 64-float scratch array reused as 16 V2 points, the 16-step ternary search,
   the 16-sample look-ahead with an unrolled max search (the compiler unrolls it by 8, as the original), and the
   two line-of-sight walks over the 0x007f1a9c/0x007f9a9c grid with round-half-away (z rounded before x).
   The first loop has to test `i < s->n` (a cached n makes 9176 unroll it by 8; the original does not).
   Residual (b), out of budget: frame layout (ours 0x1ac vs 0x1a0, offx/offy swapped), the original keeps four
   duplicated y-range compares in the first two debug draws (ours folds them in an && chain and in a nested
   if), and the walk/retry loop block layout differs (Ghidra shows it as two gotos into a shared tail).
   PROV-DIVERGED-18 (2026-11-11): the two 1/3 literals were 0.33333334f (0x3eaaaaab); the original
   multiplies by 0x005ce034 = 0x3eaaaa9f (0x0044400e, 0x0044405c), the value 0x00443440's 0.333333f gives. Fixed. */
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
typedef struct { float x; float y; } SplinePt_00443dc0;
typedef struct { SplinePt_00443dc0 pt[64]; int n; } Spline_00443dc0;
typedef struct { float x; float y; } V2_00443dc0;
MGLOBAL(float, DAT_007f1a58, 0x007f1a58);
MGLOBAL(float, DAT_007f1a5c, 0x007f1a5c);
MGLOBALA(int, DAT_008032d4, 20, 0x008032d4);
MGLOBALA(short, DAT_007f1a9c, 0x4000, 0x007f1a9c);
MGLOBALA(char, DAT_007f9a9c, 0x8000, 0x007f9a9c);
void FUN_00416230(int car, int p1);
void FUN_00443300(Spline_00443dc0 *s, int idx, float t, V2_00443dc0 *out);
int FUN_004671a0(int p0);
void FUN_004b55a0(int h, V2_00443dc0 *a, V2_00443dc0 *b, unsigned char *col);
float FUN_004c3bf0(V2_00443dc0 *v);
float FUN_004c3c60(V2_00443dc0 *out, V2_00443dc0 *in);
// MATCH: 0x00443dc0 FUN_00443dc0
void FUN_00443dc0(Spline_00443dc0 *s, float *pos, float *out, int car, int track, int draw)
{
  V2_00443dc0 a;
  unsigned char col[4];
  float best;
  float d0;
  float hi;
  V2_00443dc0 b;
  float lo;
  V2_00443dc0 p;
  V2_00443dc0 q;
  float dlo;
  float dhi;
  V2_00443dc0 phi;
  V2_00443dc0 plo;
  float offx;
  float offy;
  V2_00443dc0 u;
  V2_00443dc0 v;
  float tmp[64];
  float w[16];
  int n;
  int i;
  int j;
  int k;
  int bi;
  int saved;
  int iter;
  float d;
  float t;
  float len;
  float inv;
  float sx;
  float sy;
  float fx;
  float fz;
  int ix;
  int iz;
  short cell;
  char c;
  int hit;
  int done;
  int blocked;

  col[0] = 0xff;
  col[1] = 0xff;
  col[2] = 0;
  col[3] = 0xff;
  if (draw == 1) {
    offx = DAT_007f1a58 + 4.0f;
    offy = DAT_007f1a5c - 4.0f;
  }
  n = s->n;
  bi = 0;
  best = 100000.0f;
  for (i = 0; i < s->n; i++) {
    a.x = s->pt[i].x - pos[0];
    a.y = s->pt[i].y - pos[1];
    d = a.x * a.x + a.y * a.y;
    tmp[i] = d;
    if (d < best) {
      bi = i;
      best = d;
    }
  }
  if (track != 0) {
    saved = DAT_008032d4[car * 5];
    k = bi - saved;
    if (k != 1 - n && (k < -1 || k > 1) && saved < n) {
      bi = saved;
    }
    DAT_008032d4[car * 5] = bi;
  }
  FUN_00443300(s, bi, 0.01f, &p);
  a.x = p.x - pos[0];
  a.y = p.y - pos[1];
  d0 = a.x * a.x + a.y * a.y;
  j = bi - 1;
  if (j < 0) {
    j = s->n - 1;
  }
  FUN_00443300(s, j, 0.99f, &q);
  a.x = q.x - pos[0];
  a.y = q.y - pos[1];
  if (a.x * a.x + a.y * a.y < d0) {
    p = q;
    bi = j;
  }
  hi = 1.0f;
  FUN_00443300(s, bi, 1.0f, &phi);
  a.x = phi.x - pos[0];
  a.y = phi.y - pos[1];
  lo = 0.0f;
  dhi = a.x * a.x + a.y * a.y;
  FUN_00443300(s, bi, 0.0f, &plo);
  a.x = plo.x - pos[0];
  a.y = plo.y - pos[1];
  dlo = a.x * a.x + a.y * a.y;
  for (iter = 16; iter != 0; iter--) {
    if (dhi > dlo) {
      hi = (hi + hi + lo) * 0.333333f;
      FUN_00443300(s, bi, hi, &phi);
      a.x = phi.x - pos[0];
      a.y = phi.y - pos[1];
      dhi = a.x * a.x + a.y * a.y;
    } else {
      lo = (lo + lo + hi) * 0.333333f;
      FUN_00443300(s, bi, lo, &plo);
      a.x = plo.x - pos[0];
      a.y = plo.y - pos[1];
      dlo = a.x * a.x + a.y * a.y;
    }
  }
  if (dhi <= dlo) {
    q = phi;
    t = hi;
  } else {
    q = plo;
    t = lo;
  }
  if (draw != 0) {
    b.x = (pos[1] - offy) * 0.125f;
    b.y = (offx - pos[0]) * 0.125f;
    a.x = (s->pt[bi].y - offy) * 0.125f;
    a.y = (offx - s->pt[bi].x) * 0.125f;
    if (a.x > -1.0f && b.x > -1.0f && a.x < 2.0f && b.x < 2.0f && a.y > -1.0f && b.y > -1.0f &&
        a.y < 2.0f && b.y < 2.0f && a.y > -1.0f && b.y > -1.0f && a.y < 2.0f && b.y < 2.0f) {
      FUN_004b55a0(FUN_004671a0(0), &a, &b, col);
    }
    col[0] = 0;
    a.x = (q.y - offy) * 0.125f;
    a.y = (offx - q.x) * 0.125f;
    if (a.x > -1.0f && b.x > -1.0f && a.x < 2.0f && b.x < 2.0f && a.y > -1.0f && b.y > -1.0f &&
        a.y < 2.0f && b.y < 2.0f && a.y > -1.0f && b.y > -1.0f && a.y < 2.0f && b.y < 2.0f) {
      FUN_004b55a0(FUN_004671a0(0), &a, &b, col);
    }
  }
  for (i = 0; i < 16; i++) {
    t = t + 0.05f;
    if (t > 1.0f) {
      t = t - 1.0f;
      bi = bi + 1;
      if (bi == s->n) {
        bi = 0;
      }
    }
    FUN_00443300(s, bi, t, &p);
    u.x = p.x - q.x;
    u.y = p.y - q.y;
    FUN_004c3c60(&u, &u);
    v.x = q.x - pos[0];
    v.y = q.y - pos[1];
    FUN_004c3c60(&v, &v);
    d = u.x * v.x + u.y * v.y;
    if (d < 0.0f) {
      d = -t;
    }
    if (d > 1.0f) {
      d = 1.0f;
    }
    w[i] = d;
    q = p;
    ((V2_00443dc0 *)tmp)[i] = p;
    if (draw != 0) {
      col[2] = 0xff;
      col[1] = 0;
      a.x = (p.y - offy) * 0.125f;
      a.y = (offx - p.x) * 0.125f;
    }
  }
  j = 0;
  best = 0.0f;
  for (i = 0; i < 16; i++) {
    if (w[i] > best) {
      j = i;
      best = w[i];
    }
  }
  out[0] = tmp[j * 2];
  out[1] = tmp[j * 2 + 1];
  if (draw != 0) {
    col[2] = 0;
    col[0] = 0xff;
    col[1] = 0;
    a.x = (tmp[j * 2 + 1] - offy) * 0.125f;
    a.y = (offx - tmp[j * 2]) * 0.125f;
    if (a.x > -1.0f && b.x > -1.0f && a.x < 2.0f && b.x < 2.0f && a.y > -1.0f && b.y > -1.0f &&
        a.y < 2.0f && b.y < 2.0f) {
      FUN_004b55a0(FUN_004671a0(0), &a, &b, col);
    }
  }
  done = 0;
  hit = 0;
  for (;;) {
    blocked = 0;
    u.x = out[0] - pos[0];
    u.y = out[1] - pos[1];
    t = 0.0f;
    len = FUN_004c3bf0(&u);
    v.x = pos[0];
    v.y = pos[1];
    if (len > 0.0f) {
      do {
        if (blocked) {
          break;
        }
        sx = u.x * (1.0f / len) * t + pos[0];
        sy = 1.0f / len * u.y * t + pos[1];
        if (sx != v.x && sy != v.y) {
          fx = sx * 4.0f;
          fz = sy * 4.0f;
          iz = (int)(fz > 0.0f ? fz + 0.5f : fz - 0.5f) - 16;
          ix = (int)(fx > 0.0f ? fx + 0.5f : fx - 0.5f) - 16;
          cell = DAT_007f1a9c[((iz + 0x200) >> 3) * 0x80 + ((ix + 0x200) >> 3)];
          if (cell > 0 && cell < 0x200) {
            c = DAT_007f9a9c[((ix & 7) + cell * 8) * 8 + (iz & 7)];
            if (c == 0 || c == 3) {
              blocked = 1;
              hit = 1;
            }
          }
        }
        t = t + 0.25f;
      } while (t < len);
    }
    if (!blocked) {
      u.x = out[0] - pos[0];
      t = 0.0f;
      u.y = out[1] - pos[1];
      len = FUN_004c3bf0(&u);
      inv = 1.0f / len;
      v.x = pos[0] - u.y * inv * 0.25f;
      v.y = pos[1] - inv * u.x * 0.25f;
      if (len > 0.0f) {
        do {
          if (blocked) {
            break;
          }
          sx = u.x * inv * t + v.x;
          sy = u.y * inv * t + v.y;
          if (sx != v.x && sy != v.y) {
            fx = sx * 4.0f;
            fz = sy * 4.0f;
            iz = (int)(fz > 0.0f ? fz + 0.5f : fz - 0.5f) - 16;
            ix = (int)(fx > 0.0f ? fx + 0.5f : fx - 0.5f) - 16;
            cell = DAT_007f1a9c[((iz + 0x200) >> 3) * 0x80 + ((ix + 0x200) >> 3)];
            if (cell > 0 && cell < 0x200) {
              c = DAT_007f9a9c[((ix & 7) + cell * 8) * 8 + (iz & 7)];
              if (c == 0 || c == 3) {
                blocked = 1;
                hit = 1;
              }
            }
          }
          t = t + 0.25f;
        } while (t < len);
      }
    }
    if (blocked == 1) {
      if (j == 0 || --j == 0) {
        done = 1;
      } else {
        j--;
      }
      out[0] = tmp[j * 2];
      out[1] = tmp[j * 2 + 1];
    } else {
      done = 1;
    }
    FUN_00416230(car, j == 0);
    if (done != 0) {
      break;
    }
  }
  if (hit != 0) {
    if (j != 0 && --j != 0) {
      j--;
    }
    out[0] = tmp[j * 2];
    out[1] = tmp[j * 2 + 1];
  }
  if (draw != 0) {
    col[2] = 0;
    col[0] = 0xff;
    col[1] = 0;
    a.x = (tmp[j * 2 + 1] - offy) * 0.125f;
    a.y = (offx - tmp[j * 2]) * 0.125f;
    if (a.x > -1.0f && b.x > -1.0f && a.x < 2.0f && b.x < 2.0f && a.y > -1.0f && b.y > -1.0f &&
        a.y < 2.0f && b.y < 2.0f) {
      FUN_004b55a0(FUN_004671a0(0), &a, &b, col);
    }
  }
}
