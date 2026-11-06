// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046ddb0 (helper h4): best 81.7% (regs 83.4%, ops 97.5%), 825/824 ins, class (b). Raw 17.7%. The car pointer arrives in EDI (caller 0x00470923 sets EDI, then pushes dt, the matrix and a third dword this body never reads). So the body is a static function, and a stand-in caller under #ifndef MATCH_PORT makes cl pick EDI (pilot finding 1). The file also holds static callee 0x0046c5f0 (EAX=a, EDX=2nd, ECX=3rd, ESI=out; 100% here). Moves that scored: literals for every .rdata constant except DAT_005d757c (0.0, a real global); separate damp/t/kk locals; one shared V3 local d for the other-car offset, gravity, history delta and unit force (the original reuses one stack slot); the history delta written as subtract then scale; contact!=0 branches first; the per-wheel clamp as one if/else-if before a single store; ov.z*dn.z+ov.x*dn.x+ov.y*dn.y; (float)((double)..) on the dots; c->f150*(c->f154*c->f158)*drag. nw==4.0f gives the original's integer compare. Permuter 420 s: kept its three statement moves that preserve semantics (82.3% needed a d.y move that drops the dt scale, rejected). Remaining: the per-wheel steer dot is evaluated z-first because our build keeps the scaled delta z on the x87 stack (24 order/cast forms and 20 statement orders tried), [ecx+0x958] addressing in the other-car loop, the gravity z term (fld/fmul st(1)), and scheduling around the ring index. C++ check ok. */
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
typedef struct { float x, y, z; } V3_0046ddb0;
typedef struct { float m[12]; V3_0046ddb0 pos; float pad; } Mtx_0046ddb0;    /* 0x40 */
typedef struct {
    float x, y, z;                    /* +0x00 */
    char p0c[0x14 - 0x0c];
    V3_0046ddb0 wpos;                 /* +0x14 */
    float f20;                        /* +0x20 */
    char p24[0x2c - 0x24];
    int contact;                      /* +0x2c */
    char p30[0x3c - 0x30];
    float steer;                      /* +0x3c */
    char p40[0x84 - 0x40];
    unsigned int surf;                /* +0x84 */
    float k, k1, k2;                  /* +0x88 */
    float f94, f98, f9c;              /* +0x94 */
    float fa0;                        /* +0xa0 */
    float fa4;                        /* +0xa4 */
    V3_0046ddb0 frc;                  /* +0xa8 */
    V3_0046ddb0 rpos;                 /* +0xb4 */
    char pc0[0xc4 - 0xc0];
} Wheel_0046ddb0;
typedef struct {
    int id;                           /* +0x000 */
    int active;                       /* +0x004 */
    int lvl;                          /* +0x008 */
    int p00c;
    int f10;                          /* +0x010 */
    char p014[0x2c - 0x14];
    int f2c;                          /* +0x02c */
    char p030[0x50 - 0x30];
    float f50;                        /* +0x050 */
    float f54;                        /* +0x054 */
    char p058[0x150 - 0x58];
    float f150, f154, f158;           /* +0x150 */
    char p15c[0x16c - 0x15c];
    Wheel_0046ddb0 w[4];              /* +0x16c */
    char p47c[0x928 - 0x47c];
    Mtx_0046ddb0 mt[2];               /* +0x928 */
    int cur;                          /* +0x9a8 */
    int p9ac;
    V3_0046ddb0 vel;                  /* +0x9b0 */
    char p9bc[0x9d4 - 0x9bc];
    V3_0046ddb0 pos;                  /* +0x9d4 */
    float nw;                         /* +0x9e0 */
    float speed;                      /* +0x9e4 */
    char p9e8[0xad0 - 0x9e8];
    int hidx;                         /* +0xad0 */
    V3_0046ddb0 hist[2];              /* +0xad4 */
    char paec[0xb00 - 0xaec];
    V3_0046ddb0 jit;                  /* +0xb00 */
    char pb0c[0xb18 - 0xb0c];
    float fb18;                       /* +0xb18 */
    int pb1c;
    int fb20;                         /* +0xb20 */
    char pb24[0xb2c - 0xb24];
    V3_0046ddb0 ring[16];             /* +0xb2c */
    int ridx;                         /* +0xbec */
    char pbf0[0xd00 - 0xbf0];
    int fd00;                         /* +0xd00 */
} Car_0046ddb0;
MGLOBALA(Car_0046ddb0, DAT_008815a0, 4, 0x008815a0);
MGLOBALA(V3_0046ddb0, DAT_00881560, 4, 0x00881560);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(V3_0046ddb0, DAT_00614708, 0x00614708);
MGLOBAL(V3_0046ddb0, DAT_006146fc, 0x006146fc);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_007f0ff8, 0x007f0ff8);
MGLOBAL(float, DAT_00803334, 0x00803334);
MGLOBAL(float, DAT_00803338, 0x00803338);
MGLOBAL(float, DAT_0080333c, 0x0080333c);
MGLOBAL(float, DAT_00803340, 0x00803340);
MGLOBAL(float, DAT_0088e610, 0x0088e610);
MGLOBAL(float, DAT_0088e5f0, 0x0088e5f0);
MGLOBAL(float, DAT_00613138, 0x00613138);
int FUN_004c3df0(V3_0046ddb0 *out, V3_0046ddb0 *in, int n, Mtx_0046ddb0 *m);
int FUN_004c4d20(Mtx_0046ddb0 *m, V3_0046ddb0 *axis, float angle, int op);
float FUN_004c3ac0(V3_0046ddb0 *v);
float FUN_004c39b0(V3_0046ddb0 *out, V3_0046ddb0 *in);
float FUN_00442ce0(int id, float a, float b);
int FUN_00442c80(int id);
float FUN_00472650(float lo, float hi);
float FUN_004c3b30(float x);

// MATCH: 0x0046c5f0 FUN_0046c5f0
static void FUN_0046c5f0(float *a, float *c, float *b, float *out)
{
    float ax = a[0] - c[0], ay = a[1] - c[1], az = a[2] - c[2];
    float bx = a[0] - b[0], by = a[1] - b[1], bz = a[2] - b[2];
    float nx = bz * ay - by * az;
    float ny = bx * az - bz * ax;
    float nz = by * ax - bx * ay;
    float l = nx * nx + ny * ny + nz * nz;
    if (l >= 0.0f) {
        l = FUN_004c3b30(l);
        if (l != 0.0f) {
            l = 1.0f / l;
            out[0] = nx * l;
            out[1] = ny * l;
            out[2] = l * nz;
        }
    }
}

#define WHEEL_0046ddb0(W) \
    if (c->w[W].contact != 0) { \
      s = (float)((double)c->w[W].wpos.z * d.z + (double)c->w[W].wpos.x * d.x + (double)c->w[W].wpos.y * d.y) * 0.002f; \
      if (s > lim) s = lim; \
      nlim = -lim; \
      if (s < nlim) s = nlim; \
      c->w[W].fa0 = base + s; \
      if (s > 20.0f) s = 20.0f; \
      else if (s < -20.0f) s = -20.0f; \
      DAT_00881560[W].y = s * 0.0033333334f; \
    } else { \
      c->w[W].fa0 = 0.0f; \
    }

// MATCH: 0x0046ddb0 FUN_0046ddb0
static void FUN_0046ddb0(Car_0046ddb0 *c, float dt, Mtx_0046ddb0 *m, int unused)
{
  int i, j, lvl, n;
  float drag, scale, l6c, k, f, g, s, lim, nlim, base, len, t, damp, grav, kk;

  V3_0046ddb0 d, ov, dn, dv;
  Mtx_0046ddb0 rot;
  Wheel_0046ddb0 *w;

  FUN_004c3df0(&c->pos, &DAT_00614708, 1, m);
  c->nw = 0.0f;
  for (j = 0; j < 4; j++) {
    if (c->w[j].contact != 0) c->nw = c->nw + 1.0f;
    DAT_00881560[j].x = c->w[j].x;
    DAT_00881560[j].z = c->w[j].z;
    DAT_00881560[j].y = 0.0f;
    FUN_004c3df0(&c->w[j].wpos, &DAT_00881560[j], 1, m);
    if (c->w[j].steer != DAT_005d757c) {
      FUN_004c4d20(&rot, &DAT_006146fc, c->w[j].steer, 0);
      FUN_004c3df0(&c->w[j].rpos, &c->pos, 1, &rot);
    } else {
      c->w[j].rpos.x = c->pos.x;
      c->w[j].rpos.y = c->pos.y;
      c->w[j].rpos.z = c->pos.z;
    }
  }
  j = (c->hidx - 1) & 1;
  c->hidx = j;
  c->hist[j].x = c->vel.x;
  c->hist[j].y = c->vel.y;
  c->hist[j].z = c->vel.z;
  drag = c->speed;
  if (drag < 0.0001f) drag = DAT_005d757c;
  drag = c->f150 * (c->f154 * c->f158) * drag;
  if (c->nw != 4.0f) drag = drag * 0.5f;
  scale = 1.0f;
  drag = drag * c->f54 * dt * 0.00033333333f;
  for (i = 0; i < 4; i++) {
    if (i != c->id && DAT_008815a0[i].active == 1) {
      d.z = DAT_008815a0[i].mt[DAT_008815a0[i].cur].pos.z - c->mt[c->cur].pos.z;
      d.x = DAT_008815a0[i].mt[DAT_008815a0[i].cur].pos.x - c->mt[c->cur].pos.x;
      d.y = DAT_008815a0[i].mt[DAT_008815a0[i].cur].pos.y - c->mt[c->cur].pos.y;
      ov = DAT_008815a0[i].vel;
      if (!(FUN_004c3ac0(&ov) < 0.0001f)) {
        FUN_004c39b0(&ov, &ov);
        FUN_004c39b0(&dn, &d);
        if (ov.z * dn.z + ov.x * dn.x + ov.y * dn.y > 0.99f) {
          len = FUN_004c3ac0(&d);
          if (len < 6.0f) {
            f = 1.0f - (6.0f - len) * 0.125f;
            if (f < scale) scale = f;
          }
        }
      }
    }
  }
  if (DAT_007f0fd0 == 4) scale = 1.0f;
  else if (DAT_007f0fd0 == 9) scale = 1.0f;
  else if (DAT_007f0fd0 == 8 && c->id != 0) scale = 0.75f;
  t = 60000.0f - (float)DAT_007f0ff8;
  if (t < DAT_005d757c) {
    if (t < -30000.0f) t = -30000.0f;
    l6c = 1.0f - t * -3.3333334e-05f;
  } else {
    l6c = 1.0f;
  }
  k = DAT_005d757c;
  lvl = 0;
  if (DAT_008815a0[0].active != 0 && DAT_008815a0[0].lvl > 0) lvl = DAT_008815a0[0].lvl;
  if (DAT_008815a0[1].active != 0 && DAT_008815a0[1].lvl > lvl) lvl = DAT_008815a0[1].lvl;
  if (DAT_008815a0[2].active != 0 && DAT_008815a0[2].lvl > lvl) lvl = DAT_008815a0[2].lvl;
  if (DAT_008815a0[3].active != 0 && DAT_008815a0[3].lvl > lvl) lvl = DAT_008815a0[3].lvl;
  if (lvl == 0) k = -0.15f;
  else if (lvl == 1) k = DAT_005d757c;
  else if (lvl == 2) k = 0.5f;
  else if (lvl == 3) k = 1.0f;
  if (l6c == DAT_005d757c) {
    lvl = 3;
    if (DAT_008815a0[0].active != 0 && DAT_008815a0[0].lvl < lvl) lvl = DAT_008815a0[0].lvl;
    if (DAT_008815a0[1].active != 0 && DAT_008815a0[1].lvl < lvl) lvl = DAT_008815a0[1].lvl;
    if (DAT_008815a0[2].active != 0 && DAT_008815a0[2].lvl < lvl) lvl = DAT_008815a0[2].lvl;
    if (DAT_008815a0[3].active != 0 && DAT_008815a0[3].lvl < lvl) lvl = DAT_008815a0[3].lvl;
    if (lvl == 0) {
      l6c = 1.0f;
      k = -0.15f;
    }
  }
  scale = (k * l6c + 1.0f) * scale;
  scale = FUN_00442ce0(c->id, scale, l6c) * scale;
  if (l6c > DAT_005d757c && FUN_00442c80(c->id) != 0) {
    n = 0;
    if (DAT_008815a0[0].active != 0) n = 1;
    if (DAT_008815a0[1].active != 0) n++;
    if (DAT_008815a0[2].active != 0) n++;
    if (DAT_008815a0[3].active != 0) n++;
    if (n > 2) scale = scale * 0.5f;
  }
  damp = 1.0f - scale * drag;
  if (damp < DAT_005d757c || damp > 1.0f) damp = DAT_005d757c;
  {
  float *v = &c->vel.x;
  v[0] = damp * v[0];
  v[1] = damp * v[1];
  v[2] = damp * v[2];
  }
  grav = dt * c->speed * 1e-05f;
  if (grav > 2.0f) grav = 2.0f;
  grav = grav * DAT_00803340;
  d.x = DAT_00803334 * grav;
  d.y = DAT_00803338 * grav;
  d.z = DAT_0080333c * grav;
  c->vel.x = d.x + c->vel.x;
  c->vel.y = d.y + c->vel.y;
  c->vel.z = d.z + c->vel.z;
  if (c->nw != DAT_005d757c) {
    i = c->hidx;
    j = (i - 1) & 1;
    d.x = c->hist[i].x - c->hist[j].x;
    d.y = c->hist[i].y - c->hist[j].y;
    d.z = c->hist[i].z - c->hist[j].z;
    d.x = d.x * dt;
    d.y = d.y * dt;
    d.z = d.z * dt;
    base = c->f50 / c->nw * DAT_0088e610;
    lim = base * 0.33333334f;
    WHEEL_0046ddb0(0)
    WHEEL_0046ddb0(1)
    WHEEL_0046ddb0(2)
    WHEEL_0046ddb0(3)
    c->w[3].fa0 = c->w[3].fa0 * 0.5f;
    j = (c->ridx + 1) & 0xf;
    c->ridx = j;
    FUN_0046c5f0(&DAT_00881560[0].x, &DAT_00881560[1].x, &DAT_00881560[2].x, &c->ring[j].x);
    if (c->ring[j].z < DAT_005d757c) c->ring[j].z = c->ring[j].z * 0.25f;
  }
  w = c->w;
  for (n = 4; n != 0; n--, w++) {
    if (w->contact != 0) {
      w->k = 0.25f;
      c->jit.x = 0.0f;
      c->jit.y = 0.0f;
      c->jit.z = 0.0f;
      if (w->surf == 0xffff32ff) {
        f = c->speed * 5e-06f;
        c->jit.x = FUN_00472650(-f, f);
        c->jit.y = 0.0f;
        c->jit.z = FUN_00472650(-f, f);
      } else if (w->surf == 0xffa08080) {
        w->k = 0.1f;
      } else if (w->surf == 0xffaa8080) {
        w->k = 0.01f;
      } else {
        if (w->surf == 0xff961e5a || w->surf == 0xff1e80b4) {
          if (c->fd00 == 0) w->k = 0.01f;
          else w->k = 0.2f;
        }
        if (w->surf == 0xffc81e5a) w->k = 0.2f;
      }
      if (c->f2c != 0) w->k = (float)c->f2c * w->k * 0.01f;
      kk = w->f20 * w->k;
      w->k = kk;
      w->k1 = 0.6f * kk;
      w->k2 = DAT_00613138 * kk;
      if (c->f10 == 1) {
        w->k1 = kk;
        w->k2 = kk;
      }
      w->fa4 = w->fa0 * w->f98;
      if (w->fa4 < DAT_005d757c) w->fa4 = 0.0f;
      if (c->nw == 4.0f) {
        f = DAT_0088e5f0 * w->fa0 * 0.5f;
        w->frc.x = f * w->f94;
        w->frc.y = (w->f98 - 1.0f) * f;
        w->frc.z = f * w->f9c;
        len = FUN_004c3ac0(&w->frc);
        if (len != DAT_005d757c) {
          g = 1.0f / len;
          d.x = g * w->frc.x;
          d.y = g * w->frc.y;
          d.z = g * w->frc.z;
          g = (float)((double)d.z * w->rpos.z + (double)d.x * w->rpos.x + (double)d.y * w->rpos.y) * len;
          w->frc.x = g * w->rpos.x;
          w->frc.y = g * w->rpos.y;
          w->frc.z = g * w->rpos.z;
        }
      } else {
        len = w->fa0 / dt * DAT_0088e5f0;
        w->frc.x = len * w->f94;
        w->frc.y = len * w->f98;
        w->frc.z = len * w->f9c;
      }
      if (c->fb20 == 0 && len > 65536.0f) c->fb20 = 1;
    } else {
      w->frc.z = 0.0f;
      w->frc.y = 0.0f;
      w->frc.x = 0.0f;
    }
  }
  if (c->nw < 3.0f) {
    c->fb18 = c->fb18 - DAT_0088e610 * c->f50 * 3000.0f;
    c->fb20 = 1;
  }
}

#ifndef MATCH_PORT
/* stand-in caller (not the real caller at 0x00470923): only there so cl gives FUN_0046ddb0 a register car argument */
void h4_standin_0046ddb0(Car_0046ddb0 *c, float dt, Mtx_0046ddb0 *m, int x)
{
  int i;
  for (i = 0; i < 4; i++, c++) {
    FUN_0046ddb0(c, dt, m, x);
    FUN_004c3df0(&c->pos, &c->vel, i, m);
  }
}
#endif
