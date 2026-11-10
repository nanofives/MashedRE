// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004538b0 (717 B) and the static callees it holds. This file holds 0x00453100, 0x00453210,
   0x004532f0 and 0x00453730 (pilot finding 1: each is static, defined before its caller, address not taken, so
   the record pointer arrives in a register: ESI for 0x00453100, EDI for 0x00453210, EAX for 0x004532f0,
   0x00453730 and 0x004538b0). All five work on one 0x110-byte record (pool at 0x00684ea8, see 0x00452f30).
   0x00453100 100%: i = 0 before the colour stores, sz.y = sz.x = v, history shift as a do/while counting down
     from 15 with hist[i] = pos at i == 0.
   0x00453210 100%. 0x004532f0 100% (int pad array: a char pad made /GS add a cookie).
   0x00453730 95.3% (regs 95.3): the hit test is FUN_004b4cd0(FUN_0045bfe0(), &line, &hit) (Ghidra gave the 3 args
     to 0x0045bfe0's wrong arity); the line end needs float temps vx/vy/vz read before the start copy (otherwise
     fld start first, 90.6%). Residual (b): push of &hit and one fstp of out.x scheduled one slot apart; tried
     operand orders, inline V3 add/copy helpers, separate statements, permuter (no gain).
   0x004538b0 87.4% (regs 87.4): the homing target copy needs its own V3 (tgt) distinct from old (one V3 shared both: the
     stack layout rotated, 73.5%); the if (homing) branch order is call-first; off.x/off.z read into float temps
     before the target copy. Residual (b): the original keeps (off.x+tgt.x) and (off.z+tgt.z) on the x87 stack and
     subtracts with fsubr st(2)/st(1) then pops them (no source form found: tried scalar temps, V3 aim, in-place
     tgt +=, split statements), plus x87/integer store interleave in the move scale and position update. The permuter (742 compiles)
     moved the pos.x store after nz (86.5 -> 87.4).
   The stand-in caller at the end is the second loop of the real caller 0x00453b80 (not in this slice). */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct { V3 p; float w; } V4;
typedef struct { V3 start; V3 end; } Line;
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct { float x, y; } V2;
typedef struct { int m[16]; } Mat;
typedef struct {
  V3 v0;           /* 0x00 */
  int f0c;         /* 0x0c */
  V3 v10;          /* 0x10 */
  int pad1c[7];    /* 0x1c */
  float f38;       /* 0x38 */
  int f3c;         /* 0x3c */
} Hit;
typedef struct { int pad[12]; V3 pos; } Obj30;          /* +0x30 */
typedef struct { int pad[8]; V3 v; } Obj20;             /* +0x20 */
typedef struct { int pad; Obj20 *o; } FrameRef;          /* +4 */
typedef struct {
  int id;          /* 0x00 */
  int f4;          /* 0x04 */
  V3 off;          /* 0x08 */
  V3 pos;          /* 0x14 */
  V3 move;         /* 0x20 */
  V3 prev;         /* 0x2c */
  V3 vel;          /* 0x38 */
  int homing;      /* 0x44 */
  FrameRef *frame; /* 0x48 */
  float time;      /* 0x4c */
  float t;         /* 0x50 */
  float baseY;     /* 0x54 */
  int handle;      /* 0x58 */
  V3 hist[15];     /* 0x5c */
} Rec;              /* 0x110 */
typedef void (*EvCb)(void *a, void *b);
typedef struct {
  V3 pos;          /* 0x00 */
  float radius;    /* 0x0c */
  int type;        /* 0x10 */
  Rec *owner;      /* 0x14 */
  int f18;         /* 0x18 */
  EvCb cb;         /* 0x1c */
  int pad20[(0x8c - 0x20) / 4];
} Ev;
double sin(double);
#pragma intrinsic(sin)
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBAL(V3, DAT_006146fc, 0x006146fc);
void FUN_004893b0(int h);
void FUN_004893d0(int h, V3 *v, RGBA *c);
void FUN_00489450(int h, V2 *v);
void FUN_00489480(int h, float f);
void FUN_00489890(int h);
void FUN_00453eb0(V4 *p, float a, float b, float c, int n);
void FUN_00474d60(int a, int b);
void FUN_00474de0(V3 *a, V3 *b, float c);
void FUN_00476860(V3 *a, V3 *b, float c);
void FUN_00453290(void *a, void *b);
int FUN_00484cf0(Ev *e);
int FUN_0045bfe0(void);
int FUN_004b4cd0(int w, Line *l, Hit *h);
int FUN_0045c350(Hit *h, V3 *p);
void FUN_004b4650(V3 *out, V3 *a, V3 *b, float t);
void FUN_004b5080(Mat *m, V3 *a, V3 *b);
void FUN_004c5010(Mat *m, V3 *s, int op);
void FUN_00477760(int a, Mat *m, RGBA *c, float f1, float f2);
void FUN_00486610(Mat *m, float f);
void FUN_0046d4a0(Obj30 **out, int id);
float FUN_004c3ac0(V3 *v);
float FUN_004c39b0(V3 *out, V3 *in);
void FUN_004c1340(int clump, V3 *pos, int op);
void FUN_004b42c0(int clump, V3 *at, V3 *up);
void FUN_004c13e0(int clump, V3 *scale, int op);
// MATCH: 0x00453100 FUN_00453100
static void FUN_00453100(Rec *rec)
{
  int i;
  float t;
  RGBA col;
  V2 sz;
  i = 0;
  col.r = 0x80;
  col.g = 0x80;
  col.b = 0x80;
  for (; i < 15; i++) {
    t = (float)i * 0.06666667f;
    col.a = (unsigned char)(100.0f - t * 90.0f);
    sz.y = sz.x = (1.4f - t) * 0.3f;
    FUN_004893d0(rec->handle, &rec->hist[i], &col);
    FUN_00489450(rec->handle, &sz);
    FUN_00489480(rec->handle, t * 3.1415927f);
  }
  FUN_004893b0(rec->handle);
  FUN_00489890(rec->handle);
  i = 15;
  do {
    i--;
    if (i == 0) rec->hist[i] = rec->pos;
    else rec->hist[i] = rec->hist[i - 1];
  } while (i != 0);
}
// MATCH: 0x00453210 FUN_00453210
static void FUN_00453210(Rec *rec)
{
  V3 tmp;
  V4 s;
  rec->f4 = 0;
  FUN_00474de0(&rec->pos, &tmp, 0.3f);
  FUN_00474d60((int)rec->frame, 0);
  FUN_00476860(&rec->pos, &DAT_006146fc, 360.0f);
  s.p = rec->pos;
  s.w = 2.0f;
  FUN_00453eb0(&s, 30.0f, 150.0f, 150.0f, 2);
}
// MATCH: 0x004532f0 FUN_004532f0
static void FUN_004532f0(Rec *rec)
{
  Ev e;
  e.pos = rec->pos;
  e.radius = 0.05f;
  e.owner = rec;
  e.f18 = 0;
  e.cb = FUN_00453290;
  e.type = 0x13;
  FUN_00484cf0(&e);
}
// MATCH: 0x00453730 FUN_00453730
static int FUN_00453730(Rec *rec)
{
  RGBA col;
  V3 out;
  float t;
  Line line;
  V3 scale;
  Hit hit;
  Mat mat;
  line.start = rec->pos;
  { float vx = rec->vel.x, vy = rec->vel.y, vz = rec->vel.z;
  line.end.x = vx + line.start.x;
  line.end.y = (float)(vy + (double)(line.start.y));
  line.end.z = vz + line.start.z; }
  if (FUN_004b4cd0(FUN_0045bfe0(), &line, &hit) != 0) {
    t = hit.f38;
    col.r = 0xff;
    col.g = 0x80;
    col.b = 0;
    col.a = 0x80;
    if (FUN_0045c350(&hit, &rec->pos) == 0) {
      FUN_004b4650(&out, &line.start, &line.end, t);
      { float hx = hit.v0.x * 0.05f; out.x = out.x + hx; }  /* MATCH-X87-CASTLEVER-OPORDER: 24 -> 16 B with the y cast above */
      out.y += hit.v0.y * 0.05f;
      out.z += hit.v0.z * 0.05f;
      FUN_004b5080(&mat, &hit.v10, &out);
      scale.y = 1.0f;
      scale.x = 1.0f;
      scale.z = 1.0f;
      FUN_004c5010(&mat, &scale, 1);
      FUN_00477760(0, &mat, &col, 2.5f, 10.0f);
      FUN_00486610(&mat, 0.4f);
      FUN_00453210(rec);
      return 1;
    }
  }
  return 0;
}
// MATCH: 0x004538b0 FUN_004538b0
static void FUN_004538b0(Rec *rec)
{
  Obj30 *m;
  float nx, nz, vy, vz, len, k, h, ny, tx, tz;
  V3 d;
  V3 at;
  V3 up;
  V3 old;
  V3 tgt;
  V3 scale;
  int clump;
  scale.x = 2.0f;
  scale.y = 2.0f;
  scale.z = 2.0f;
  at.x = 0.0f;
  at.y = 0.0f;
  at.z = 0.0f;
  up.x = 0.0f;
  up.y = 0.0f;
  up.z = 0.0f;
  if (FUN_00453730(rec) != 0) return;
  rec->t = rec->time * 0.7692308f;
  if (rec->t > 2.0f) {
    FUN_00453210(rec);
    return;
  }
  if (rec->homing != 0) {
    FUN_0046d4a0(&m, rec->id);
    { float ox = rec->off.x, oz = rec->off.z;
    tgt = m->pos;
    tx = ox + tgt.x;
    tz = oz + tgt.z;
    d.x = tx - rec->pos.x;
    d.y = 0.0f;
    d.z = tz - rec->pos.z; }
    len = FUN_004c3ac0(&d);
    FUN_004c39b0(&d, &d);
    k = len * rec->t * 0.38f;
    d.x *= k;
    d.y *= k;
    d.z *= k;
    rec->move = d;
    if (rec->t > 0.58f) rec->homing = 0;
  }
  if (rec->t < 1.25f) h = (float)sin(rec->t * 3.1415927f);
  else h = (rec->time - 1.3f) * -2.2f;
  rec->time = DAT_007f100c + rec->time;
  old = rec->pos;
  rec->prev = rec->pos;
  nx = rec->move.x + rec->pos.x;
  nz = rec->move.z + rec->pos.z;
  rec->pos.z = nz;
  rec->pos.x = nx;
  ny = h * 1.5f + rec->baseY;
  rec->pos.y = ny;
  rec->vel.x = nx - old.x;
  vy = ny - old.y;
  rec->vel.y = vy;
  vz = nz - old.z;
  rec->vel.z = vz;
  clump = (int)rec->frame->o;
  if (rec->homing != 0) {
    FUN_0046d4a0(&m, rec->id);
    at = m->pos;
  } else {
    at.x = rec->vel.x + nx;
    at.y = vy + ny;
    at.z = vz + nz;
  }
  up = rec->frame->o->v;
  FUN_004c39b0(&up, &up);
  FUN_004c1340(clump, &rec->pos, 0);
  FUN_004b42c0(clump, &at, &up);
  FUN_004c13e0(clump, &scale, 1);
  FUN_00453100(rec);
}
#ifndef MATCH_PORT
/* stand-in for the second loop of the real caller 0x00453b80 (not in this slice): it only makes the DDK
   keep the static functions above and pass the record in EAX; not compiled in the port. */
MGLOBALA(Rec, DAT_00684ea8, 32, 0x00684ea8);
void standin(void)
{
  Rec *r;
  for (r = DAT_00684ea8; r < DAT_00684ea8 + 32; r++) {
    if (r->f4 != 0) {
      FUN_004538b0(r);
      FUN_004532f0(r);
    }
  }
}
#endif
