// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00420870 by hand (h2): best 72.1% (regs 72.1%, ops 87.9%), 275/280 ins. Raw g2s 20.3%.
   The record is passed in registers to six callees, so the file holds them as file-`static` functions defined before
   the caller (pilot finding 1), and every call convention comes out as in the original: FUN_0041f4f0 (EDI, 100%,
   body from re/match/s04/0041f4f0.c), FUN_0041f590 (ESI, 78.4%, body from re/match/s04/0041f590.c), FUN_0041f710
   (EAX, 100%, body from h1's re/match/s05/0041f710.c), FUN_0041f770 (EAX + one stack float, 79.5%, written here
   from the disassembly; h1's slice function), FUN_0041f880 (EDI, 100%, body from h1's re/match/s05/0041f880.c),
   FUN_00420340 (ESI, 100%) with its callee FUN_0041f360 (100%). FUN_0041f3d0 is a plain cdecl call (prototype only).
   Moved it: one PSlot layout for all of them (0x0063d9e0, 0x2ac B), V3 axis separate from the FUN_0046cb30 output
   (0x1c frame), (v.x + v.z) order, the squared distance as its own statement d = (dy*dy + dx*dx) + dz*dz then
   CLAMP01(d * 25.0f) (MIN-then-MAX macro, gives the original's jnp / test ah,1 / jp layout and the param-slot
   store), switch cases written in the original's block order (63.9% -> 71.1%), previous matrix copy as a struct
   assignment (rep movsd).
   Residuals: (1) the switch: cl 9176 cross-jumps the compound-case tails (mode==N && flag tests) more than the
   original, which keeps separate copies for 4/6 vs 0x25/0x27 etc. (5 fewer instructions in ours); tried boolean
   &&, if/else 1/0, ternary 1:0, if-only, case orders; (2) fld [m+0x34] scheduled one slot later. Permuter (6
   rounds) gave only the (v.x + v.z) commute. Class (b). */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { int x, y, z; } V3i;
typedef struct { V3 start, end; } Line;
typedef struct { V3 right; int flags; V3 up; int p1; V3 at; int p2; V3 pos; int p3; } Mtx;
typedef struct {
  char *ev[0x80];                /* +0x000 atomics by part id */
  float hist[8];                 /* +0x200 */
  float spin[4];                 /* +0x220 */
  float v230[6];                 /* +0x230 */
  float sph248[4];               /* +0x248 */
  V3 pos;                        /* +0x258 */
  int u264;                      /* +0x264 */
  char *ent;                     /* +0x268 */
  char *obj;                     /* +0x26c */
  int natom;                     /* +0x270 */
  float f274;                    /* +0x274 */
  float f278;                    /* +0x278 */
  char *u27c;                    /* +0x27c */
  float f280;                    /* +0x280 */
  float avg;                     /* +0x284 */
  float x288;                    /* +0x288 */
  int x28c;                      /* +0x28c */
  unsigned int x290;             /* +0x290 */
  unsigned int x294;             /* +0x294 */
  int mode;                      /* +0x298 */
  char *car;                     /* +0x29c */
  int u2a0[3];                   /* +0x2a0 */
} PSlot;
typedef struct {
  V3i v;
  int a;
  int b;
  int c, d, e;
  int pad[0x1b];
} Msg8c;
MGLOBALA(PSlot, DAT_0063d9e0, 4, 0x0063d9e0);
MGLOBALA(Mtx, DAT_0063d8e0, 4, 0x0063d8e0);
MGLOBALA(char *, DAT_0063d840, 4, 0x0063d840);
MGLOBAL(int, DAT_0063d850, 0x0063d850);
MGLOBAL(int, DAT_0063d7ec, 0x0063d7ec);
MGLOBAL(int, DAT_00636acc, 0x00636acc);
MGLOBAL(int, DAT_00636ad0, 0x00636ad0);
MGLOBAL(unsigned int, DAT_007f101c, 0x007f101c);
MGLOBAL(float, DAT_0063e4a4, 0x0063e4a4);
MGLOBAL(V3, DAT_006146f0, 0x006146f0);
MGLOBAL(V3, DAT_006146fc, 0x006146fc);
MGLOBAL(V3, DAT_00614708, 0x00614708);
int FUN_004260b0(void);
int FUN_004260a0(void);
char *FUN_004c1480(char *frame, Mtx *m, int op);
char *FUN_004c0ed0(char *frame);
void FUN_0046cb30(V3 *out, int slot);
char *FUN_004c1520(char *frame, V3 *axis, float angle, int op);
char *FUN_004c1340(char *frame, V3 *v, int op);
void FUN_004b5240(char *atomic, int on);
void FUN_0041f3d0(PSlot *r, unsigned int dt, int unused);
void FUN_0046bce0(V3 *out, unsigned int v);
void FUN_0046bd20(V3 *out, unsigned int v);
void FUN_0046bda0(float *out, unsigned int v, int i);
int FUN_00426dc0(Line *line, float *hit, float *frac);
void FUN_00484cf0(Msg8c *m);
void FUN_00544bf0(char *geom, float v);
float FUN_00422470(float x, unsigned int seed);
void FUN_00422aa0(unsigned int a, int b);
void FUN_00420290(int param_1, float param_2);
int FUN_00540080(int *p, float v);
int FUN_00540100(int *p, float v);
int FUN_00540160(int *p, float v);
// MATCH: 0x0041f4f0 FUN_0041f4f0
static void FUN_0041f4f0(PSlot *r)
{
  char *frame = *(char **)(r->ev[0x13] + 4);
  V3 a, b, pos;
  float ang;
  FUN_0046bce0(&a, r->x290);
  FUN_0046bd20(&b, r->x290);
  pos = *(V3 *)(frame + 0x40);
  ang = (a.z + b.z) * -100.0f;
  FUN_004c1520(frame, &DAT_00614708, (a.x + b.x) * 40.0f, 0);
  FUN_004c1520(frame, &DAT_006146f0, ang, 2);
  FUN_004c1340(frame, &pos, 2);
}
// MATCH: 0x0041f590 FUN_0041f590
static void FUN_0041f590(PSlot *r)
{
  float hit = 0.0f;
  float frac = 1.0f;
  Line line;
  int i;
  V3 *pos;
  float f;
  for (i = 7; i > 0; i--)
    r->hist[i] = r->hist[i - 1];
  pos = (V3 *)(FUN_004c0ed0(*(char **)(r->obj + 4)) + 0x30);
  line.start.x = pos->x;
  line.end.z = pos->z;
  line.start.y = pos->y + 0.0625f;
  line.start.z = pos->z;
  line.end.x = pos->x;
  line.end.y = pos->y - 0.9375f;
  if ((DAT_007f101c & 3) == r->x290) {
    if (FUN_00426dc0(&line, &hit, &frac))
      r->hist[0] = hit;
    r->x288 = 1.0f - frac;
  }
  r->avg = 0.0f;
  for (i = 0; i < 8; i++)
    r->avg += r->hist[i];
  r->avg *= 0.125f;
  f = r->avg;
  r->avg = ((1.0f <= f ? 1.0f : f) >= 0.0f ? (1.0f <= f ? 1.0f : f) : 0.0f);
}
// MATCH: 0x0041f710 FUN_0041f710
static void FUN_0041f710(char *p)
{
  Msg8c m;
  m.v = *(V3i *)(p + 0x258);
  m.a = *(int *)(p + 0x264);
  m.b = *(int *)(p + 0x290) + 0xff00;
  m.c = 0;
  m.d = 0;
  m.e = 0;
  FUN_00484cf0(&m);
}
// MATCH: 0x0041f770 FUN_0041f770
static void FUN_0041f770(PSlot *r, float t)
{
  float v;
  float x;
  char *frame;
  if (r->mode == 0) {
    FUN_0046bda0(&v, r->x290, 0);
    v = v * 0.05f;
    v = ((1.0f <= v ? 1.0f : v) >= -1.0f ? (1.0f <= v ? 1.0f : v) : -1.0f) * 0.5f;
    FUN_00544bf0(r->ev[0x12], v + 0.5f);
    frame = *(char **)(r->ev[0x14] + 4);
    x = FUN_00422470(DAT_0063e4a4, 0xff) - 0.5f;
    FUN_004c1520(frame, &DAT_006146fc, x * 30.0f, 0);
    FUN_004c1520(frame, &DAT_006146f0, x * 15.0f, 1);
    DAT_0063e4a4 += t * 0.0625f;
  }
}
// MATCH: 0x0041f880 FUN_0041f880
static void FUN_0041f880(PSlot *r)
{
  unsigned int f = r->x294;
  int k = 0;
  int id = 0x3b;
  if (f & 0x400)
    id = 0x40;
  else if ((f & 0x10) && (f & 0x20)) {
    k = 3;
    id = 0x3e;
  } else if (f & 0x10) {
    k = 1;
    id = 0x3c;
  } else if (f & 0x20) {
    k = 2;
    id = 0x3d;
  }
  r->x28c = id;
  if (DAT_00636acc == 0)
    FUN_00422aa0(r->x290, k);
}
// MATCH: 0x0041f360 FUN_0041f360
static int FUN_0041f360(int param_1)
{
  if (DAT_00636ad0 == 0) {
    switch (param_1) {
    case 4: case 5: case 8: case 10: case 0xb: case 0xe: case 0xf:
    case 0x25: case 0x26: case 0x29: case 0x2b: case 0x2c: case 0x2f: case 0x30:
      return 1;
    }
  }
  return 0;
}
// MATCH: 0x00420340 FUN_00420340
static void FUN_00420340(PSlot *s)
{
  unsigned int flags;
  unsigned int b2, b4, b1;
  int i;
  int *p;
  FUN_00420290(s->x290, s->avg);
  flags = s->x294;
  b2 = flags & 2;
  b4 = flags & 4;
  b1 = flags & 1;
  for (i = 0; i < 0x80; i++) {
    if (s->ev[i] != 0 && FUN_0041f360(i) != 0) {
      p = **(int ***)(*(char **)(s->ev[i] + 0x18) + 0x20);
      if (b2) FUN_00540160(p, s->f274);
      if (b4) FUN_00540100(p, *(float *)&s->u27c);
      if (b1) FUN_00540080(p, s->f278);
    }
  }
  if (b2) s->x294 &= ~2u;
  if (b4) s->x294 &= ~4u;
  if (b1) s->x294 &= ~1u;
}
#define CLAMP01(e) ((1.0f <= (e) ? 1.0f : (e)) >= 0.0f ? (1.0f <= (e) ? 1.0f : (e)) : 0.0f)
// MATCH: 0x00420870 FUN_00420870
void FUN_00420870(int slot, unsigned int dt, Mtx *m)
{
  PSlot *s;
  V3 v;
  V3 axis;
  float f, d, t;
  float dx, dy, dz;
  int i;
  int on;
  char *a;
  DAT_0063d850 = FUN_004260b0();
  DAT_0063d7ec = FUN_004260a0();
  s = &DAT_0063d9e0[slot];
  FUN_004c1480(*(char **)(s->obj + 4), m, 0);
  s->pos = *(V3 *)(FUN_004c0ed0(*(char **)(s->ev[0x13] + 4)) + 0x30);
  if (s->u27c == DAT_0063d840[slot]) {
    FUN_0046cb30(&v, slot);
    f = s->f280 - (v.x + v.z) * 0.0003f;
    s->f280 = f;
    axis = *(V3 *)(*(char **)(s->obj + 4) + 0x10);
    FUN_004c1520(s->u27c, &axis, f, 0);
  }
  dx = m->pos.x - DAT_0063d8e0[slot].pos.x;
  dy = m->pos.y - DAT_0063d8e0[slot].pos.y;
  dz = m->pos.z - DAT_0063d8e0[slot].pos.z;
  d = (dy * dy + dx * dx) + dz * dz;
  t = CLAMP01(d * 25.0f);
  FUN_0041f3d0(s, dt, slot);
  FUN_0041f4f0(s);
  FUN_0041f590(s);
  FUN_0041f710((char *)s);
  FUN_0041f770(s, t);
  FUN_0041f880(s);
  FUN_00420340(s);
  for (i = 0; i < 0x80; i++) {
    a = s->ev[i];
    on = 0;
    if (a == 0)
      continue;
    switch (i) {
    case 0: case 1: case 2: case 3: case 8: case 9: case 0x10: case 0x12: case 0x14:
      on = s->mode == 0; break;
    case 0x21: case 0x22: case 0x23: case 0x24: case 0x29: case 0x2a:
      on = s->mode == 1; break;
    case 0x46:
      on = s->mode == 0; break;
    case 4: case 6:
      on = s->mode == 0 && !(s->x294 & 0x10); break;
    case 0x25: case 0x27:
      on = s->mode == 1 && !(s->x294 & 0x10); break;
    case 10: case 0xc:
      on = s->mode == 0 && !(s->x294 & 0x20); break;
    case 0x2b: case 0x2d:
      on = s->mode == 1 && !(s->x294 & 0x20); break;
    case 5: case 7:
      on = s->mode == 0 && (s->x294 & 0x10); break;
    case 0x26: case 0x28:
      on = s->mode == 1 && (s->x294 & 0x10); break;
    case 0xb: case 0xd:
      on = s->mode == 0 && (s->x294 & 0x20); break;
    case 0x2c: case 0x2e:
      on = s->mode == 1 && (s->x294 & 0x20); break;
    case 0x31: case 0x47:
      on = s->mode == 1; break;
    case 0x15:
      on = (s->x294 & 0x40) && !(s->x294 & 0x10); break;
    case 0x18:
      on = (s->x294 & 0x80) && !(s->x294 & 0x20); break;
    case 0x11:
      on = (s->x294 & 0x800) && !(s->x294 & 0x20); break;
    case 0x17:
      on = s->mode == 0 && (s->x294 & 0x400); break;
    case 0x32:
      on = s->mode == 1 && (s->x294 & 0x400); break;
    case 100:
      on = s->mode == 2 && !(s->x294 & 0x30); break;
    case 0x65:
      on = s->mode == 2 && (s->x294 & 0x10) && !(s->x294 & 0x20); break;
    case 0x66:
      on = s->mode == 2 && !(s->x294 & 0x10) && (s->x294 & 0x20); break;
    case 0x67:
      on = s->mode == 2 && (s->x294 & 0x10) && (s->x294 & 0x20); break;
    }
    if (i != 0x17 && i != 0x32)
      on = (s->x294 & 0x400) ? 0 : on;
    FUN_004b5240(a, on);
  }
  DAT_0063d8e0[slot] = *m;
}
