// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00467650 (h1): 56.1% (regs 56.3%, ops 89.6%), 1188/1236 ins (4908 B; Ghidra's size 1255 is short).
   Raw 27.9%. Convention: car in ESI (unaff_ESI), four stack args, plain `ret`: a static callee (pilot finding 1)
   with a stand-in caller shaped like 0x00470670 (`mov esi, edi`; no MATCH line, built only outside MATCH_PORT).
   Shape: 5-gear loop (cl unrolls it, which is where Ghidra's i == -1..-4 tests come from); FUN_004a2c48 is
   __ftol, so every Ghidra `x = FUN_004a2c48()` is an (int) cast of the value the disassembly shows (timers
   0x494/0xbf4 minus/plus dt, wheel spin, wheel counter); the 4 wheels at +0x168 (stride 0xc4) indexed as
   &car->wheel[w] (pointer form gave the wrong base, -3.8%); ((mn / gear) / mx) * r with the parentheses (without
   them cl hoists mn/mx out of the loop); ibf4 != 0 tested first; the 0x100 wheel-flag branch first. .rdata
   floats as literals.
   Residual (b, budget): frame layout (0xe8 vs 0xe4, the gear array at another offset), the keep-and-pop x87
   pattern of the scaled-dir adds (original keeps the scale in st(1) and pops it, ours consumes it), dot-product
   term order (canonicalised), and scheduling in the spin-rotation block. An __inline V3MulAdd for the dir adds
   lost 1%. No permuter run (1236 ins, budget spent). */
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
typedef struct V3_00467650 { float x, y, z; } V3_00467650;
typedef struct Mtx_00467650 {
  V3_00467650 right; int f0c;
  V3_00467650 up; int f1c;
  V3_00467650 at; int f2c;
  V3_00467650 pos; int f3c;
} Mtx_00467650;
typedef struct Wheel_00467650 {
  int state;                        /* +0x00 */
  int f04, f08, f0c;
  float f10;                        /* +0x10 */
  float f14;                        /* +0x14 */
  V3_00467650 pos;                  /* +0x18 */
  float f24;                        /* +0x24 */
  int f28, f2c;
  int touching;                     /* +0x30 */
  int f34;
  int cnt;                          /* +0x38 */
  int spin;                         /* +0x3c */
  char pad40[0x88 - 0x40];
  int surface;                      /* +0x88 */
  int f8c;
  float f90;                        /* +0x90 */
  float f94;                        /* +0x94 */
  char pad98[0xa8 - 0x98];
  float fa8;                        /* +0xa8 */
  V3_00467650 force;                /* +0xac */
  V3_00467650 dir;                  /* +0xb8 */
} Wheel_00467650;                   /* 0xc4 */
typedef struct Car_00467650 {
  int id;                           /* +0x000 */
  char pad004[0x10 - 4];
  int i10;                          /* +0x010 */
  char pad014[0x28 - 0x14];
  int i28;                          /* +0x028 */
  int i2c;                          /* +0x02c */
  int i30;
  int i34;                          /* +0x034 */
  char pad038[0x54 - 0x38];
  float f54;                        /* +0x054 */
  float f58;
  float f5c;                        /* +0x05c */
  char pad060[0x168 - 0x60];
  Wheel_00467650 wheel[4];          /* +0x168 */
  float gear[5];                    /* +0x478 */
  int f48c;
  int gearidx;                      /* +0x490 */
  int timer;                        /* +0x494 */
  float f498;                       /* +0x498 */
  float f49c;                       /* +0x49c */
  char pad4a0[0x9b0 - 0x4a0];
  V3_00467650 vel;                  /* +0x9b0 */
  V3_00467650 spin;                 /* +0x9bc */
  char pad9c8[0x9d4 - 0x9c8];
  V3_00467650 dir;                  /* +0x9d4 */
  int f9e0;                         /* +0x9e0 */
  float speed;                      /* +0x9e4 */
  float spinmag;                    /* +0x9e8 */
  char pad9ec[0xb0c - 0x9ec];
  float fb0c;                       /* +0xb0c */
  char padb10[0xb14 - 0xb10];
  V3_00467650 force;                /* +0xb14 */
  int ib20;                         /* +0xb20 */
  char padb24[0xbf4 - 0xb24];
  int ibf4;                         /* +0xbf4 */
  int ibf8;                         /* +0xbf8 */
  char padbfc[0xd00 - 0xbfc];
  int id00;                         /* +0xd00 */
} Car_00467650;
MGLOBAL(float, DAT_0088e5f0, 0x0088e5f0);
MGLOBAL(int, DAT_0088e668, 0x0088e668);
MGLOBAL(int, DAT_0088e66c, 0x0088e66c);
int FUN_0040e340(void);
int FUN_0040e350(void);
float FUN_004c39b0(V3_00467650 *out, V3_00467650 *in);
float FUN_004c3ac0(V3_00467650 *v);
void FUN_004c3df0(V3_00467650 *out, V3_00467650 *in, int n, Mtx_00467650 *m);
void *FUN_004c4d20(Mtx_00467650 *m, V3_00467650 *axis, float angle, int combine);
static __inline void V3Scale_00467650(V3_00467650 *o, V3_00467650 *a, float s) { o->x = a->x * s; o->y = a->y * s; o->z = a->z * s; }

// MATCH: 0x00467650 FUN_00467650
static void FUN_00467650(Car_00467650 *car, int idx, float dt, int unused, unsigned char *in)
{
  int mode;
  int i;
  int cnt;
  int g;
  int w;
  int applied;
  int slip;
  int flags;
  int surf;
  Wheel_00467650 *wh;
  float out[5];
  float t, mn, mx, r, rpm, thr, v, lim, f, d, e4, nx, ny, nz, bc, L, q, acc, acc2, m1, m2, sp;
  V3_00467650 nv, p, tt, F, a, cr, B, C, W, e, D;
  Mtx_00467650 m;

  mode = FUN_0040e350();
  car->speed = FUN_004c3ac0(&car->vel);
  car->spinmag = FUN_004c3ac0(&car->spin);
  acc2 = 0.0f;
  W.z = 0.0f; W.y = 0.0f; W.x = 0.0f;
  C.z = 0.0f; C.y = 0.0f; C.x = 0.0f;
  B.z = 0.0f; B.y = 0.0f; B.x = 0.0f;
  mx = car->f49c * 0.5f;
  mn = car->f498 * 0.5f;
  t = 1500.0f - car->fb0c;
  if (t <= 500.0f) t = 500.0f;
  t = t * 0.00066666666f;
  mn = mn * t;
  mx = mx * t;
  cnt = 0;
  w = 0;
  for (i = 0; i < 5; i++) {
    if (car->gear[i] != 0.0f) {
      if (i == 0) r = (car->speed + 250.0f) * car->gear[0];
      else r = car->gear[i] * car->speed;
      if (r > mx) {
        r = mx;
        if (i != 4) cnt++;
      }
      out[i] = (mn / car->gear[i]) / mx * r + car->speed;
    }
  }
  g = car->gearidx;
  if (cnt != g) {
    if (car->timer == 0 && cnt > g) {
      car->timer = 3000;
      car->gearidx = g + 1;
    }
    if (cnt < car->gearidx) {
      car->timer = -3000;
      car->gearidx = cnt;
    }
  }
  g = car->gearidx;
  rpm = out[g];
  if (car->timer > 0) {
    car->timer = (int)((float)car->timer - dt);
    if (car->timer < 0) car->timer = 0;
  }
  if (car->timer < 0) {
    car->timer = (int)((float)car->timer + dt);
    if (car->timer > 0) car->timer = 0;
  }
  slip = 0;
  thr = (float)in[4];
  if (mode == 7) thr = 0.0f;
  else if (thr > 160.0f) thr = 160.0f;
  surf = car->wheel[0].surface;
  v = rpm * car->gear[g] / (car->speed * 0.01f + 10.0f) * thr;
  if (surf == 0xffa08080) lim = 150000.0f;
  else if (surf == 0xffaa8080) lim = 200000.0f;
  else {
    lim = 350000.0f;
    if ((surf == 0xff961e5a || surf == 0xff1e80b4) && car->id00 == 0) lim = 300000.0f;
    if (surf == -1 || surf == 0xffc8c8c8) {
      f = (1.0f - dt * 0.0166666675f * 0.01f) * 0.99f;
      car->vel.x = f * car->vel.x;
      car->vel.y = f * car->vel.y;
      car->vel.z = f * car->vel.z;
    }
  }
  if (v > lim) {
    slip = 1;
    rpm = rpm * 0.5f;
  }
  acc = 0.0f;
  for (w = 0; w < 4; w++) {
    wh = &car->wheel[w];
    applied = 0;
    if (wh->state == 2 && wh->touching != 0) {
      e4 = (float)in[4];
      if (mode == 7) e4 = 0.0f;
      if (car->i28 != 0) e4 = (float)(100 - car->i28) * e4 * 0.01f;
      if ((car->wheel[0].surface == 0xff961e5a || car->wheel[0].surface == 0xff1e80b4) && car->speed > 0.0f) {
        FUN_004c39b0(&nv, &car->vel);
        d = car->dir.x * nv.x + car->dir.y * nv.y + car->dir.z * nv.z;
        d = d * d;
        d = d * d;
        d = d * d;
        if (d < 0.0f) d = -d;
        if (d < 0.25f) d = 0.25f;
        if (car->id00 == 0 && d < 0.5f) d = 0.5f;
        e4 = d * e4;
      }
      if (mode != 7 && e4 != 0.0f) {
        applied = 1;
        car->ib20 = 1;
        if (e4 > 160.0f) e4 = 160.0f;
        e4 = e4 * rpm;
        car->force.x = wh->dir.x * e4 + car->force.x;
        car->force.y = wh->dir.y * e4 + car->force.y;
        car->force.z = wh->dir.z * e4 + car->force.z;
      }
      if (FUN_0040e350() == 6) {
        car->ibf4 = (int)((float)car->ibf4 - dt);
        if (car->ibf4 > 3000) car->ibf4 = 3000;
        if (car->ibf4 < 0) car->ibf4 = 0;
      }
      if (car->ibf8 == 1) {
        if (car->ibf4 != 0) {
          f = 5000000.0f;
          if (FUN_0040e340() == 4 && (DAT_0088e668 == idx || DAT_0088e66c == idx)) f = 8000000.0f;
          car->force.x = wh->dir.x * f + car->force.x;
          car->force.y = wh->dir.y * f + car->force.y;
          car->force.z = wh->dir.z * f + car->force.z;
          car->ibf4 = (int)((float)car->ibf4 - dt);
          if (car->ibf4 <= 0) {
            car->ibf8 = 0;
            car->ibf4 = 0;
          }
        } else car->ibf8 = 0;
      }
      if (car->ibf8 == 2) {
        if (car->ibf4 != 0) {
          car->force.z = 0.0f;
          car->force.y = 0.0f;
          car->force.x = 0.0f;
          car->ibf4 = (int)((float)car->ibf4 - dt);
          if (car->ibf4 <= 0) {
            car->ibf8 = 0;
            car->ibf4 = 0;
          }
        } else car->ibf8 = 0;
      }
    }
    wh->spin = (int)((wh->dir.x * car->vel.x + wh->dir.y * car->vel.y + wh->dir.z * car->vel.z) / (wh->f14 * 1.74533331f));
    if (slip == 0 || wh->state != 2) wh->cnt = 0;
    else {
      wh->cnt++;
      if (wh->cnt < 0x200) wh->cnt = 0x200;
      wh->spin += (wh->cnt - 0x200) << 6;
    }
    if (wh->touching != 0) {
      if (in[5] != 0 && in[4] == 0) {
        car->ib20 = 1;
        if (wh->spin <= 0) {
          if (wh->state == 2) {
            f = (float)in[5];
            if (car->i28 != 0) f = f * (100 - car->i28) * 0.01f;
            if (f > 160.0f) f = 160.0f;
            f = -(rpm * 0.9f * f);
            wh->force.x = f * wh->dir.x + wh->force.x;
            wh->force.y = f * wh->dir.y + wh->force.y;
            wh->force.z = f * wh->dir.z + wh->force.z;
          }
        } else if (w >= 2) {
          wh->cnt = (int)((float)in[5] + 256.0f);
        }
      }
      if (car->spin.x < -1e-5f || car->spin.x > 1e-5f || car->spin.y < -1e-5f || car->spin.y > 1e-5f ||
          car->spin.z < -1e-5f || car->spin.z > 1e-5f) {
        FUN_004c4d20(&m, &car->spin, 270.0f, 0);
        FUN_004c3df0(&nv, &wh->pos, 1, &m);
        f = car->speed * 0.008f;
        if (f < 8.0f) f = 8.0f;
        f = f * (car->spinmag * 0.019904457f) * 360.0f;
        V3Scale_00467650(&nv, &nv, f);
        nv.x = nv.x - car->vel.x;
        nv.y = nv.y - car->vel.y;
        nv.z = nv.z - car->vel.z;
        e4 = FUN_004c3ac0(&nv);
      } else {
        e4 = car->speed;
        nv.x = -car->vel.x;
        nv.y = -car->vel.y;
        nv.z = -car->vel.z;
      }
      if (e4 > 0.0001f && car->f9e0 == 0x40800000) {
        f = 1.0f / e4;
        nx = nv.x * f;
        ny = nv.y * f;
        nz = nv.z * f;
        if (e4 > 1024.0f) e4 = 1024.0f;
        bc = wh->f90 * wh->fa8 * DAT_0088e5f0 * e4 * 0.0009765625f;
        F.x = 0.0f;
        F.y = 0.0f;
        F.z = 0.0f;
        d = car->dir.x * nx + car->dir.y * ny + car->dir.z * nz;
        p.x = d * wh->dir.x;
        p.y = d * wh->dir.y;
        p.z = d * wh->dir.z;
        tt.x = nx - p.x;
        tt.y = ny - p.y;
        tt.z = nz - p.z;
        L = FUN_004c3ac0(&tt);
        acc = L * e4 + acc;
        flags = wh->cnt;
        if (flags & 0x100) {
          f = bc * 0.85f;
          q = wh->fa8 * 3.0f * DAT_0088e5f0 * (float)flags * 0.001953125f * 1.1f;
          if (applied) q = q * 0.5f;
          F.x = p.x * q + tt.x * f;
          F.y = p.y * q + tt.y * f;
          F.z = p.z * q + f * tt.z;
        } else {
          f = bc;
          if (flags == 0) {
            q = wh->f94 * wh->fa8 * DAT_0088e5f0;
            F.x = p.x * q;
            F.y = p.y * q;
            F.z = q * p.z;
            if (L * car->speed < 50.0f) f = L * car->speed * 0.02f * bc;
          }
          F.x = tt.x * f + F.x;
          F.y = tt.y * f + F.y;
          F.z = f * tt.z + F.z;
        }
        wh->force.x = F.x + wh->force.x;
        wh->force.y = F.y + wh->force.y;
        wh->force.z = F.z + wh->force.z;
      }
    }
    if (FUN_004c3ac0(&wh->force) > 0.0001f) {
      f = 1.0f / wh->f10;
      a.x = wh->pos.x * f;
      a.y = wh->pos.y * f;
      a.z = wh->pos.z * f;
      d = a.x * wh->force.x + a.y * wh->force.y + a.z * wh->force.z;
      p.x = a.x * d;
      p.y = a.y * d;
      p.z = a.z * d;
      tt.x = wh->force.x - p.x;
      tt.y = wh->force.y - p.y;
      tt.z = wh->force.z - p.z;
      cr.x = tt.z * p.y - tt.y * p.z;
      cr.y = tt.x * p.z - tt.z * p.x;
      cr.z = tt.y * p.x - tt.x * p.y;
      if (d > 0.0f) {
        cr.x = -cr.x;
        cr.y = -cr.y;
        cr.z = -cr.z;
      }
      B.x = p.x + B.x;
      B.y = p.y + B.y;
      B.z = p.z + B.z;
      C.x = C.x + wh->force.x;
      C.y = C.y + wh->force.y;
      C.z = C.z + wh->force.z;
      e.x = tt.x * wh->f10;
      e.y = tt.y * wh->f10;
      e.z = tt.z * wh->f10;
      m1 = FUN_004c3ac0(&e) * 50.24f;
      acc2 = m1 + acc2;
      m2 = FUN_004c3ac0(&cr);
      if (m2 > 0.0001f) {
        m2 = m1 / m2;
        W.x = W.x - cr.x * m2;
        W.y = W.y - cr.y * m2;
        W.z = W.z - cr.z * m2;
      }
    }
  }
  sp = dt * car->f5c * 9.25933307e-07f;
  e.x = W.x * sp;
  e.y = W.y * sp;
  e.z = W.z * sp;
  if (car->i10 == 0) {
    car->spin.x = e.x + car->spin.x;
    car->spin.y = e.y + car->spin.y;
    car->spin.z = e.z + car->spin.z;
  }
  q = acc2 - FUN_004c3ac0(&W);
  if (q != 0.0f) {
    q = q / acc2;
    D.x = C.x - B.x;
    D.y = C.y - B.y;
    D.z = C.z - B.z;
    B.x = D.x * q + B.x;
    B.y = D.y * q + B.y;
    B.z = D.z * q + B.z;
  }
  B.z = car->force.z + B.z;
  sp = dt * car->f54 * 0.00033333333f;
  car->vel.x = sp * (car->force.x + B.x) + car->vel.x;
  car->vel.y = sp * (car->force.y + B.y) + car->vel.y;
  car->vel.z = sp * B.z + car->vel.z;
  sp = FUN_004c3ac0(&car->vel);
  acc = acc / car->wheel[0].f24;
  car->speed = sp;
  surf = car->wheel[0].surface;
  if (surf == 0xffa08080) acc = acc * 1.5f;
  else {
    if (surf != 0xffaa8080) {
      if (surf == 0xff961e5a || surf == 0xff1e80b4) {
        if (car->id00 == 0) acc = acc * 3.0f;
        else acc = acc * 1.5f;
      }
      if (surf != 0xffc81e5a) goto skip;
    }
    acc = acc + acc;
  }
skip:
  if (car->i2c != 0) acc = ((float)car->i2c * 0.01f + 1.0f) * acc;
  if (car->i34 != 0) acc = ((float)car->i34 * 0.1f + 1.0f) * acc;
  if (sp != 0.0f && car->f9e0 == 0x40800000) {
    d = car->dir.x * car->vel.x + car->dir.y * car->vel.y + car->dir.z * car->vel.z;
    a.x = car->vel.x - d * car->dir.x;
    a.y = car->vel.y - d * car->dir.y;
    a.z = car->vel.z - d * car->dir.z;
    acc = acc * sp;
    if (acc > 32768.0f) {
      f = (10000000.0f - acc) * 1e-07f;
      if (f < 0.0f) f = 0.0f;
      f = f * 0.1f + f * 0.1f;
      car->vel.x = car->vel.x - a.x * f;
      car->vel.y = car->vel.y - a.y * f;
      car->vel.z = car->vel.z - a.z * f;
      f = 1.0f - f;
      car->spin.x = f * car->spin.x;
      car->spin.y = f * car->spin.y;
      car->spin.z = f * car->spin.z;
      return;
    }
    f = (32768.0f - acc) * 3.05175781e-05f;
    if (f < 0.1f) f = 0.1f;
    car->vel.x = car->vel.x - a.x * f;
    car->vel.y = car->vel.y - a.y * f;
    car->vel.z = car->vel.z - a.z * f;
    if (f < 0.5f) f = 0.5f;
    f = 1.0f - f;
    car->spin.x = f * car->spin.x;
    car->spin.y = f * car->spin.y;
    car->spin.z = f * car->spin.z;
    if (car->ib20 == 0 && sp < 16.0f) {
      car->vel.x = 0.0f;
      car->vel.z = 0.0f;
      car->vel.y = 0.0f;
      car->spin.z = 0.0f;
      car->spin.y = 0.0f;
      car->spin.x = 0.0f;
    }
  }
}

#ifndef MATCH_PORT
/* stand-in caller: 0x00470670 calls it with the car in ESI (mov esi, edi) and four stack args */
void StandIn_00467650(Car_00467650 *car, int idx, float dt, int u, unsigned char *in)
{
  FUN_00467650(car, idx, dt, u, in);
  car->i10 = 0;
}
#endif
