// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046bfc0 (1446 B): 60.9% (regs 61.2, ops 98.2), by hand (h3). Raw 0% (did not compile).
   Structure, types, callee arities and x87 operand order are in place (ops 98%): car record struct
   (Mtx stride 0x40, pos at +0x30, wheel points V3[4] at +0xa58, vel +0x9b0, mass +0x50, +0xaec), unsigned hit
   count (fild + 2^32 fixup), nested ifs so both failures share one return 0, dot products split against cl
   reassociation, 1/la and 1/lb written inline (not a k variable) so k*t*lb keeps the original order.
   Residual (b): the frame. Ours is 0xb4, the original 0xb8 with one slot the code never touches, and the
   slot assignment differs (original shares pos with la and the loop counter with the hit count; ours shares
   la with the counter), so most [esp+N] operands differ. Also: the original keeps the final scale w on the
   x87 stack until the epilogue (fstp st(0) after pop esi) where ours consumes it in the last multiply, and
   two scheduling spots (r2 = tmp int copies, la store before lea). Declaration order, block scope and
   statement order did not move the frame. Permuter 300 s: 63.0% only with a move that breaks semantics
   (tmp.x used before set); the semantics-preserving moves from it are applied here. cxx: ok. */
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
typedef struct { float x, y, z; } V3;
typedef struct { float m[12]; V3 pos; float pad; } Mtx;   /* 0x40, pos at +0x30 */
typedef struct {
    char p0[0x50];
    float f50;                      /* 0x50 */
    char p1[0x928 - 0x54];
    Mtx mt[2];                      /* 0x928 */
    int mi;                         /* 0x9a8 */
    char p2[0x9b0 - 0x9ac];
    V3 vel;                         /* 0x9b0 */
    char p3[0xa58 - 0x9bc];
    V3 wp[4];                       /* 0xa58 */
    char p4[0xaec - 0xa88];
    int faec;                       /* 0xaec */
    char p5[0xd04 - 0xaf0];
} Car;                              /* 0xd04 */
MGLOBALA(Car, DAT_008815a0, 16, 0x008815a0);
Mtx *FUN_004c4dc0(Mtx *out, Mtx *in);
void FUN_004c3d90(V3 *out, V3 *in, int n, Mtx *m);
int FUN_004c18c0(float *box, V3 *p);
float FUN_004c39b0(V3 *out, V3 *in);
float FUN_004c3ac0(V3 *v);

// MATCH: 0x0046bfc0 FUN_0046bfc0
int FUN_0046bfc0(int param_1, float *param_2, Mtx *param_3, float param_4, V3 *param_5)
{
  Car *c;
  V3 *pos;
  V3 *wp;
  V3 tmp, sum, a, b, r1, r2, d, e, ctr;
  Mtx m;
  uint n;
  int j;
  float k, t, s, la, lb, w;

  c = &DAT_008815a0[param_1];
  pos = &c->mt[c->mi].pos;
  ctr.x = (param_2[3] + param_2[0]) * 0.5f;
  ctr.y = (param_2[4] + param_2[1]) * 0.5f;
  ctr.z = (param_2[5] + param_2[2]) * 0.5f;
  sum.z = 0.0f;
  sum.y = 0.0f;
  sum.x = 0.0f;
  n = 0;
  FUN_004c4dc0(&m, param_3);
  wp = c->wp;
  for (j = 0; j < 4; j++) {
    FUN_004c3d90(&tmp, wp, 1, &m);
    if (FUN_004c18c0(param_2, &tmp) != 0) {
      n++;
      sum.x = sum.x + wp->x;
      sum.y = sum.y + wp->y;
      sum.z = sum.z + wp->z;
    }
    wp++;
  }
  if (n != 0) {
  k = 1.0f / (float)n;
  sum.z = sum.z * k;
  sum.x = sum.x * k;
  sum.y = sum.y * k;
  d.x = sum.x - pos->x;
  d.y = sum.y - pos->y;
  d.z = sum.z - pos->z;
  FUN_004c39b0(&d, &d);
  t = d.x * c->vel.x;
  t = t + d.y * c->vel.y;
  if (!(t + d.z * c->vel.z < 0.0f)) {
  e.x = sum.x - ctr.x;
  e.y = sum.y - ctr.y;
  e.z = sum.z - ctr.z;
  FUN_004c39b0(&e, &e);
  b.y = 0.0f;
  a.x = c->f50 * c->vel.x;
  a.y = c->f50 * c->vel.y;
  a.z = c->vel.z * c->f50;
  b.x = 0.0f;
  b.z = 0.0f;
  r1.x = 0.0f;
  r1.y = 0.0f;
  r1.z = 0.0f;
  r2.x = 0.0f;
  r2.y = 0.0f;
  r2.z = 0.0f;
  la = FUN_004c3ac0(&a);
  lb = FUN_004c3ac0(&b);
  if (la != 0.0f) {
    t = b.y * a.y + b.x * a.x;
    t = (t + b.z * a.z) * (1.0f / la);
    if (lb != 0.0f) {
      t = t / lb;
    }
    t = -((1.0f / la) * t * lb);
    if (t > 0.0f) {
      t = 0.0f;
    } else if (t < -1.0f) {
      t = -1.0f;
    }
    tmp.x = a.x * t;
    tmp.y = a.y * t;
    tmp.z = t * a.z;
    tmp.x = tmp.x + a.x;
    tmp.y = tmp.y + a.y;
    tmp.z = tmp.z + a.z;
    r2.z = tmp.z;
    r2.x = tmp.x;
    r2.y = tmp.y;
    r1.x = -tmp.x;
    r1.y = -tmp.y;
    r1.z = -tmp.z;
  }
  if (lb != 0.0f) {
    t = b.y * a.y + b.x * a.x;
    t = (t + b.z * a.z) * (1.0f / lb);
    if (la != 0.0f) {
      t = t / la;
    }
    t = -((1.0f / lb) * t * la);
    if (t > 0.0f) {
      t = 0.0f;
    } else if (t < -1.0f) {
      t = -1.0f;
    }
    tmp.z = t * b.z;
    tmp.x = b.x * t;
    tmp.y = b.y * t;
    tmp.x = tmp.x + b.x;
    tmp.y = tmp.y + b.y;
    tmp.z = tmp.z + b.z;
    r1.x = tmp.x + r1.x;
    r1.y = tmp.y + r1.y;
    r1.z = tmp.z + r1.z;
    r2.x = r2.x - tmp.x;
    r2.y = r2.y - tmp.y;
    r2.z = r2.z - tmp.z;
  }
  w = 1.0f / (param_4 + c->f50);
  c->vel.x = r1.x * w + c->vel.x;
  c->vel.y = r1.y * w + c->vel.y;
  c->vel.z = r1.z * w + c->vel.z;
  param_5->x = r2.x * w;
  param_5->y = r2.y * w;
  param_5->z = w * r2.z;
  la = FUN_004c3ac0(&r1);
  lb = FUN_004c3ac0(&r2);
  w = (lb + la) * 2.0000000233721948e-07f;
  if (w > 1.0f) {
    w = 1.0f;
  }
  w = w * 100.0f;
  if (w < 200.0f) {
    w = 200.0f;
  }
  c->vel.x = c->vel.x - d.x * w;
  c->vel.y = c->vel.y - d.y * w;
  c->vel.z = c->vel.z - d.z * w;
  param_5->x = param_5->x - e.x * w;
  param_5->y = param_5->y - e.y * w;
  param_5->z = param_5->z - e.z * w;
  c->faec = 0x1e0;
  return 1;
  }
  }
  return 0;
}
