// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004694e0 (h1): 46.5% (regs 53.2%, ops 90.0%), 402/402 ins. Raw: did not compile.
   Shape: list from FUN_00485370 (0x90-B triangle records, count FUN_00485360) filtered by owner == car id;
   the triangle copied into 12 scalar locals; 18 contact points (world points +0x9f8, wheels 0x198 stride 0xc4
   for j < 4, contact records +0x4a8 stride 0x40); three edge tests with a V3 cross product and scalar edge
   deltas (that put the cross products right); !(d < -0.4f); the vel copy through an __inline V3Set with float
   params (fld/fstp instead of mov); arm computed then scaled by 3.6f in a second pass; the -10 / 100000
   scaled deltas as scalar temps. .rdata floats as literals (3.6 is the pooled 0x005cc754 inside a string).
   Residual (b): stack frame layout (0x7c vs 0x84: the original has more 4-byte locals, every esp offset
   differs), the contact pointer strength-reduced to +0x20 in ours vs +0x18 (vel) in the original, the
   world-point/wheel pointers in other registers, the -10/100000 groups (original evaluates all three products
   before the stores and pops the leftovers; ours forwards them), the facing-dot order (canonicalised).
   The 0x00468d80 sibling moved from 39% to 82% with __inline V3Scale/V3Add helpers and the contact index form;
   the same helpers did not move this one. Permuter (2905 compiles) reached 53.5% only by moving statements
   across data dependences (Normalize before the delta, c used before set): not kept. */
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
typedef struct V3_004694e0 { float x, y, z; } V3_004694e0;
typedef struct Mtx_004694e0 {
  V3_004694e0 right; int f0c;
  V3_004694e0 up; int f1c;
  V3_004694e0 at; int f2c;
  V3_004694e0 pos; int f3c;
} Mtx_004694e0;                     /* 0x40 */
typedef struct Contact_004694e0 {
  float depth;                      /* +0x00 */
  int id;                           /* +0x04 */
  V3_004694e0 n;                    /* +0x08 */
  float f14;                        /* +0x14 */
  V3_004694e0 vel;                  /* +0x18 */
  float pen;                        /* +0x24 */
  float f28;                        /* +0x28 */
  V3_004694e0 pt;                   /* +0x2c */
  float imp;                        /* +0x38 */
  float f3c;                        /* +0x3c */
} Contact_004694e0;                 /* 0x40 */
typedef struct Wheel_004694e0 {
  int touching;                     /* +0x00 */
  char pad04[0xc4 - 4];
} Wheel_004694e0;                   /* 0xc4 */
typedef struct Car_004694e0 {
  int id;                           /* +0x000 */
  char pad004[0x50 - 4];
  float mass;                       /* +0x050 */
  char pad054[0x60 - 0x54];
  V3_004694e0 local[18];            /* +0x060 */
  char pad138[0x15c - 0x138];
  V3_004694e0 com;                  /* +0x15c */
  char pad168[0x198 - 0x168];
  Wheel_004694e0 wheel[4];          /* +0x198 */
  Contact_004694e0 contact[18];     /* +0x4a8 */
  Mtx_004694e0 mtx[2];              /* +0x928 */
  char pad9a8[0x9ac - 0x9a8];
  int mtxidx;                       /* +0x9ac */
  V3_004694e0 vel;                  /* +0x9b0 */
  char pad9bc[0x9f8 - 0x9bc];
  V3_004694e0 world[18];            /* +0x9f8 */
} Car_004694e0;
typedef struct Obj_004694e0 {
  V3_004694e0 v0, v1, v2, n;        /* +0x00 */
  int f30, f34;
  int owner;                        /* +0x38 */
  float k;                          /* +0x3c */
  V3_004694e0 g;                    /* +0x40 */
  V3_004694e0 imp;                  /* +0x4c */
  V3_004694e0 pos;                  /* +0x58 */
  int hit;                          /* +0x64 */
  int obj;                          /* +0x68 */
  char pad6c[0x90 - 0x6c];
} Obj_004694e0;                     /* 0x90 */
static __inline void V3Set_004694e0(V3_004694e0 *v, float x, float y, float z) { v->x = x; v->y = y; v->z = z; }
unsigned int FUN_00485360(void);

Obj_004694e0 *FUN_00485370(void);
void FUN_00485420(int obj, V3_004694e0 *out);
float FUN_004c39b0(V3_004694e0 *out, V3_004694e0 *in);
float FUN_004c3ac0(V3_004694e0 *v);
void FUN_004c3df0(V3_004694e0 *out, V3_004694e0 *in, int n, Mtx_004694e0 *m);

// MATCH: 0x004694e0 FUN_004694e0
void FUN_004694e0(Car_004694e0 *car)
{
  Mtx_004694e0 *m;
  Obj_004694e0 *e;
  unsigned int i;
  int j;
  Contact_004694e0 *c;
  Wheel_004694e0 *wh;
  V3_004694e0 *w;
  float v0x, v0y, v0z, v1x, v1y, v1z, v2x, v2y, v2z, nx, ny, nz;
  float px, py, pz;
  float d, mag, dot, ex, ey, ez, tx, ty, tz;
  V3_004694e0 cr;
  V3_004694e0 arm;
  V3_004694e0 dl;

  m = &car->mtx[car->mtxidx];
  e = FUN_00485370();
  for (i = 0; i < FUN_00485360(); i++, e++) {
    if (e->owner == car->id) {
      v0x = e->v0.x; v0y = e->v0.y; v0z = e->v0.z;
      v1x = e->v1.x; v1y = e->v1.y; v1z = e->v1.z;
      v2x = e->v2.x; v2y = e->v2.y; v2z = e->v2.z;
      nx = e->n.x; ny = e->n.y; nz = e->n.z;
      for (j = 0, c = car->contact; j < 18; j++, c++) {
        if (j >= 4 || car->wheel[j].touching == 0) {
          px = car->world[j].x;
          py = car->world[j].y;
          pz = car->world[j].z;
          d = (px - v0x) * nx + (py - v0y) * ny + (pz - v0z) * nz;
          if (d < 0.0f) {
            ex = v1x - v0x;
              ey = v1y - v0y;
              ez = v1z - v0z;
              cr.x = ny * ez - ey * nz;
              cr.y = ex * nz - ez * nx;
              cr.z = ey * nx - ex * ny;
            if ((px - v0x) * cr.x + (py - v0y) * cr.y + (pz - v0z) * cr.z >= 0.0f) {
              ex = v2x - v1x;
              ey = v2y - v1y;
              ez = v2z - v1z;
              cr.x = ny * ez - ey * nz;
              cr.y = ex * nz - ez * nx;
              cr.z = ey * nx - ex * ny;
              if ((px - v1x) * cr.x + (py - v1y) * cr.y + (pz - v1z) * cr.z >= 0.0f) {
                ex = v0x - v2x;
              ey = v0y - v2y;
              ez = v0z - v2z;
              cr.x = ny * ez - ey * nz;
              cr.y = ex * nz - ez * nx;
              cr.z = ey * nx - ex * ny;
                if ((px - v2x) * cr.x + (py - v2y) * cr.y + (pz - v2z) * cr.z >= 0.0f &&
                    !(d < -0.4f)) {
                  c->pen = -d;
                  arm.x = car->local[j].x - car->com.x;
                  arm.y = car->local[j].y - car->com.y;
                  arm.z = car->local[j].z - car->com.z;
                  arm.x = arm.x * 3.6f;
                  arm.y = arm.y * 3.6f;
                  arm.z = arm.z * 3.6f;
                  FUN_004c3df0(&c->pt, &arm, 1, m);
                  V3Set_004694e0(&c->vel, car->vel.x, car->vel.y, car->vel.z);
                  arm.z = 0.0f;
                  arm.y = 0.0f;
                  arm.x = 0.0f;
                  mag = FUN_004c3ac0(&c->vel);
                  if (mag > 0.0001f) {
                    FUN_004c39b0(&c->vel, &c->vel);
                    dot = e->n.x * c->vel.x + e->n.y * c->vel.y + e->n.z * c->vel.z;
                    if (dot < 0.0f) {
                      if (dot < -1.0f) dot = -1.0f;
                      c->imp = mag * dot;
                      c->n = e->n;
                      c->depth = d;
                      *(int *)&c->id = -2;
                      e->imp.x = car->mass * car->vel.x;
                      e->imp.y = car->mass * car->vel.y;
                      e->hit = 1;
                      e->imp.z = car->mass * car->vel.z;
                      e->imp.x = e->imp.x - e->g.x * e->k;
                      e->imp.y = e->imp.y - e->g.y * e->k;
                      e->imp.z = e->imp.z - e->g.z * e->k;
                      e->pos = car->world[j];
                      goto done;
                    }
                  }
                  e->hit = 1;
                  e->pos = car->world[j];
                  e->imp.z = 0.0f;
                  e->imp.y = 0.0f;
                  e->imp.x = 0.0f;
done:
                  FUN_00485420(*(int *)(e->obj + 0xc), &arm);
                  dl.x = arm.x - m->pos.x;
                  dl.y = arm.y - m->pos.y;
                  dl.z = arm.z - m->pos.z;
                  FUN_004c39b0(&dl, &dl);
                  tx = dl.x * -10.0f;
                  ty = dl.y * -10.0f;
                  tz = dl.z * -10.0f;
                  car->vel.x = tx + car->vel.x;
                  car->vel.y = ty + car->vel.y;
                  car->vel.z = tz + car->vel.z;
                  tx = dl.x * 100000.0f;
                  ty = dl.y * 100000.0f;
                  tz = dl.z * 100000.0f;
                  e->imp.x = e->imp.x + tx;
                  e->imp.y = e->imp.y + ty;
                  e->imp.z = e->imp.z + tz;
                }
              }
            }
          }
        }
      }
    }
  }
}
