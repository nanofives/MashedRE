// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00468d80 (h1): 82.1% (regs 84.9%, ops 96.5%), 463/462 ins. Raw: did not compile.
   Shape: terrain triangles (param 2, 0x90-B records, count DAT_0088e60c) with precomputed edge planes at
   +0x6c/+0x78/+0x84, n.y < 0.42f filter, 18 contact points as in 0x004694e0; the 0x007e9de0 event queue as one
   struct (count, type/a/b[256], c[256][8], m[256][16]) with the push written out per colour (macro); spin
   branch RwMatrixRotate 90 + transform + normalize + length. What moved it (39% -> 82%): __inline V3Scale
   (arm * s) and V3Add (c->vel = car->vel + arm) helpers, 0.2777778 (0x005cea44) as an MGLOBAL so cl does not
   fold it into 360, and &car->contact[j] taken at the first contact store.
   Residual (b): the contact pointer is strength-reduced to +0x20 in EDI (original +0x18 = vel, in EBP), which
   swaps EDI/EBP uses and some local slots (frame size matches, 0xa8); dot term order (canonicalised); keep-and-pop of
   the clamped dot. Permuter 570 compiles: no gain.
   PROV-DIVERGED-18 (2026-11-11): the spin scale was a VALUE difference. The original multiplies the length by 360
   (0x005ccac4), then c->f14, then 0x005cea44 (0x00469071 / 0x0046907d); the one-expression form compiled to length * f14 *
   0x005cea44 * 360 (reassociated), and the replay diverged at tick 408 by 1 ULP (0x00881c64 body 0x3d82ea2a / orig
   0x3d82ea29). Now one statement per multiply with 360 as a const global. The contact dot after the normalize
   (n . vel) was reassociated the same way, to (z + y) + x where the original sums (y + x) + z: now one statement
   per term. Score 82.1% (regs 85.1%, ops 96.3%), unchanged. */
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
typedef struct V3_00468d80 { float x, y, z; } V3_00468d80;
typedef struct Mtx_00468d80 {
  V3_00468d80 right; int f0c;
  V3_00468d80 up; int f1c;
  V3_00468d80 at; int f2c;
  V3_00468d80 pos; int f3c;
} Mtx_00468d80;                     /* 0x40 */
typedef struct Contact_00468d80 {
  float depth;                      /* +0x00 */
  int id;                           /* +0x04 */
  V3_00468d80 n;                    /* +0x08 */
  float f14;                        /* +0x14 */
  V3_00468d80 vel;                  /* +0x18 */
  float pen;                        /* +0x24 */
  float f28;                        /* +0x28 */
  V3_00468d80 pt;                   /* +0x2c */
  float imp;                        /* +0x38 */
  float f3c;                        /* +0x3c */
} Contact_00468d80;                 /* 0x40 */
typedef struct Wheel_00468d80 {
  int touching;                     /* +0x00 */
  char pad04[0xc4 - 4];
} Wheel_00468d80;                   /* 0xc4 */
typedef struct Car_00468d80 {
  int id;                           /* +0x000 */
  char pad004[0x60 - 4];
  V3_00468d80 local[18];            /* +0x060 */
  char pad138[0x15c - 0x138];
  V3_00468d80 com;                  /* +0x15c */
  char pad168[0x198 - 0x168];
  Wheel_00468d80 wheel[4];          /* +0x198 */
  Contact_00468d80 contact[18];     /* +0x4a8 */
  Mtx_00468d80 mtx[2];              /* +0x928 */
  char pad9a8[0x9ac - 0x9a8];
  int mtxidx;                       /* +0x9ac */
  V3_00468d80 vel;                  /* +0x9b0 */
  V3_00468d80 spin;                 /* +0x9bc */
  char pad9c8[0x9f0 - 0x9c8];
  int mode;                         /* +0x9f0 */
  char pad9f4[0x9f8 - 0x9f4];
  V3_00468d80 world[18];            /* +0x9f8 */
} Car_00468d80;
typedef struct Tri_00468d80 {
  V3_00468d80 v0, v1, v2, n;        /* +0x00 */
  int id;                           /* +0x30 */
  int color;                        /* +0x34 */
  char pad38[0x6c - 0x38];
  V3_00468d80 e0, e1, e2;           /* +0x6c */
} Tri_00468d80;                     /* 0x90 */
typedef struct EvQ_00468d80 {
  unsigned int count;
  int type[256];
  int a[256];
  int b[256];
  int c[256][8];
  int m[256][16];
} EvQ_00468d80;
MGLOBAL(EvQ_00468d80, DAT_007e9de0, 0x007e9de0);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBAL(float, DAT_005cea44, 0x005cea44);
MGLOBAL(const float, DAT_005ccac4, 0x005ccac4);
MGLOBAL(unsigned int, DAT_0088e60c, 0x0088e60c);
static __inline void V3Scale_00468d80(V3_00468d80 *o, V3_00468d80 *a, float s) { o->x = a->x * s; o->y = a->y * s; o->z = a->z * s; }
static __inline void V3Add_00468d80(V3_00468d80 *o, V3_00468d80 *a, V3_00468d80 *b) { o->x = a->x + b->x; o->y = a->y + b->y; o->z = a->z + b->z; }
int FUN_0040e350(void);
int FUN_00468b40(Tri_00468d80 *t, Car_00468d80 *car);
float FUN_004c39b0(V3_00468d80 *out, V3_00468d80 *in);
float FUN_004c3ac0(V3_00468d80 *v);
void FUN_004c3df0(V3_00468d80 *out, V3_00468d80 *in, int n, Mtx_00468d80 *m);
void *FUN_004c4d20(Mtx_00468d80 *m, V3_00468d80 *axis, float angle, int combine);

#define PUSHEV_00468d80(col) \
  { \
    DAT_007e9de0.type[DAT_007e9de0.count] = 6; \
    DAT_007e9de0.a[DAT_007e9de0.count] = DAT_007f1030; \
    DAT_007e9de0.b[DAT_007e9de0.count] = car->id + 0xff00; \
    DAT_007e9de0.c[DAT_007e9de0.count][0] = (col); \
    DAT_007e9de0.c[DAT_007e9de0.count][1] = ((int *)&rot)[1]; \
    DAT_007e9de0.c[DAT_007e9de0.count][2] = ((int *)&rot)[2]; \
    DAT_007e9de0.c[DAT_007e9de0.count][3] = ((int *)&rot)[3]; \
    DAT_007e9de0.c[DAT_007e9de0.count][4] = ((int *)&rot)[4]; \
    DAT_007e9de0.c[DAT_007e9de0.count][5] = ((int *)&rot)[5]; \
    DAT_007e9de0.c[DAT_007e9de0.count][6] = ((int *)&rot)[6]; \
    DAT_007e9de0.c[DAT_007e9de0.count][7] = ((int *)&rot)[7]; \
    for (k = 0; k < 16; k++) \
      DAT_007e9de0.m[DAT_007e9de0.count][k] = ((int *)&rot)[k]; \
    DAT_007e9de0.count++; \
    if (DAT_007e9de0.count >= 0x100) DAT_007e9de0.count = 0; \
  }

// MATCH: 0x00468d80 FUN_00468d80
void FUN_00468d80(Car_00468d80 *car, Tri_00468d80 *tri)
{
  Mtx_00468d80 *m;
  Tri_00468d80 *e;
  unsigned int i;
  int j;
  int k;
  Contact_00468d80 *c;
  float v0x, v0y, v0z, v1x, v1y, v1z, v2x, v2y, v2z, nx, ny, nz;
  float px, py, pz;
  float d, mag, dot, s;
  V3_00468d80 arm;
  Mtx_00468d80 rot;

  m = &car->mtx[car->mtxidx];
  for (i = 0, e = tri; i < DAT_0088e60c; i++, e++) {
    v0x = e->v0.x; v0y = e->v0.y; v0z = e->v0.z;
    v1x = e->v1.x; v1y = e->v1.y; v1z = e->v1.z;
    v2x = e->v2.x; v2y = e->v2.y; v2z = e->v2.z;
    nx = e->n.x; ny = e->n.y; nz = e->n.z;
    if (ny < 0.42f) {
      for (j = 0; j < 18; j++) {
        if (j >= 4 || car->wheel[j].touching == 0) {
          px = car->world[j].x;
          py = car->world[j].y;
          pz = car->world[j].z;
          d = (px - v0x) * nx + (py - v0y) * ny + (pz - v0z) * nz;
          if (d < 0.0f &&
              (px - v0x) * e->e0.x + (py - v0y) * e->e0.y + (pz - v0z) * e->e0.z >= 0.0f &&
              (px - v1x) * e->e1.x + (py - v1y) * e->e1.y + (pz - v1z) * e->e1.z >= 0.0f &&
              (px - v2x) * e->e2.x + (py - v2y) * e->e2.y + (pz - v2z) * e->e2.z >= 0.0f &&
              !(d < -0.1f)) {
            c = &car->contact[j];
            c->pen = -d;
            arm.x = car->local[j].x - car->com.x;
            arm.y = car->local[j].y - car->com.y;
            arm.z = car->local[j].z - car->com.z;
            V3Scale_00468d80(&arm, &arm, 3.6f);
            FUN_004c3df0(&c->pt, &arm, 1, m);
            if (car->spin.x == 0.0f && car->spin.y == 0.0f && car->spin.z == 0.0f) {
              arm.z = 0.0f;
              arm.y = 0.0f;
              arm.x = 0.0f;
            } else {
              FUN_004c4d20(&rot, &car->spin, 90.0f, 0);
              FUN_004c3df0(&arm, &c->pt, 1, &rot);
              FUN_004c39b0(&arm, &arm);
              s = FUN_004c3ac0(&car->spin); s = s * DAT_005ccac4; s = s * c->f14; s = s * DAT_005cea44;
              V3Scale_00468d80(&arm, &arm, s);
            }
            V3Add_00468d80(&c->vel, &car->vel, &arm);
            mag = FUN_004c3ac0(&c->vel);
            if (mag > 0.0001f) {
              FUN_004c39b0(&c->vel, &c->vel);
              dot = e->n.x * c->vel.x; dot = dot + e->n.y * c->vel.y; dot = dot + e->n.z * c->vel.z;
              if (dot < 0.0f) {
                if (dot < -1.0f) dot = -1.0f;
                mag = mag * dot;
                if (e->color == 0xff010101) {
                  if (car->mode == 0) {
                    k = FUN_0040e350();
                    if (k != 1 && k != 2 && k != 3 && k != 4 && k != 5)
                      PUSHEV_00468d80(0xff010101)
                  }
                } else {
                  if (e->color == 0xff0000ff && (car->mode == 0 || car->mode == 1)) {
                    k = FUN_0040e350();
                    if (k != 1 && k != 2 && k != 3 && k != 4 && k != 5)
                      PUSHEV_00468d80(0xff0000ff)
                  }
                  if (FUN_00468b40(e, car) == 0) {
                    c->imp = mag;
                    c->n = e->n;
                    c->depth = d;
                    c->id = e->id;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
