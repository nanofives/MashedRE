// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00419a00 _FUN_00419a00
/* 0x00419a00 (1467 B). Best 60.1% (regs 72.7, ops 93.4), 373/381 ins, bytes not identical. Converter 30.7%.
   ESI = rec (static, caller 0x00419fd0 walks the 4 x 0x64 records at 0x0063bda0, stand-in
   caller_00419fd0, not matched). Callees with register args are statics in this file, transcribed only
   for their convention and with no MATCH line:
   0x00418bd0 (EAX = out matrix, ECX = world, EDX = line), 0x00418f50 (EDI = pos),
   0x00418de0 (EAX = rec, copied from h1_00418e30.c). The ESI/EAX/EDI conventions come out right.
   Fixed: frame size 0x188 (the query struct passed to FUN_00484cf0 is 0x8c bytes, int padding, so no
   /GS cookie). Sphere {V3 c; float r} shared by case 1 (transform output) and case 2. Struct copy
   order `dir = pos-of-frame; pos = dir; start = dir`. The 50/0 clamp written as
   `!(50 <= d2) ? (d2 >= 0 ? d2 : 0) : 50` (structure matches). Literals for the 0x005cXXXX constants.
   Residual, dominated by the stack layout: k sits at F+0x48 in ours and F+0x10 in the original, and
   tgt / pos / p are shifted, so most esp-relative operands differ. Declaration order, reverse order,
   dead `if (0)` references and block scoping all leave the layout unchanged or make it worse. It follows
   the x87 allocation: the original spills k and l140 (fst) where ours keeps them in registers.
   -0.125f as a literal makes cl rewrite `at * -0.125 + p` into fsubr form. The extern keeps fmul but
   with the operands reversed.
   Class: (b) unknown source shape for the tgt/k/l140 block (several rewrites tried), stack layout
   follows from it. Not finished within budget. */
typedef struct V3 { float x, y, z; } V3;
typedef struct Sphere { V3 c; float r; } Sphere;
typedef struct RGBA { unsigned char r, g, b, a; } RGBA;
typedef struct Mtx {
  V3 right; int flags;
  V3 up;    int pad1;
  V3 at;    int pad2;
  V3 pos;   int pad3;
} Mtx;
typedef struct Frame { char obj[0x10]; Mtx modelling; } Frame;
typedef struct Atomic { int type; Frame *parent; } Atomic;
typedef struct Rec64 {
  Mtx     m;        /* +0x00 */
  Atomic *atomic;   /* +0x40 */
  int     state;    /* +0x44 */
  float   timer;    /* +0x48 */
  int     f4c;      /* +0x4c */
  float   f50;      /* +0x50 */
  float   f54;      /* +0x54 */
  float   f58;      /* +0x58 */
  float   f5c;      /* +0x5c */
  int     f60;      /* +0x60 */
} Rec64;
typedef struct Query { Sphere sphere; int kind; Rec64 *rec; int zero; void *cb; int pad20[27]; } Query;
typedef struct Hit {
  V3   n;          /* +0x00 */
  int  tri;        /* +0x0c */
  int  pad10[9];
  int  geom;       /* +0x34 */
  int  f38;        /* +0x38 */
  int  f3c;        /* +0x3c */
} Hit;

#ifndef MATCH_PORT
extern float DAT_005cc32c;
extern float DAT_005cc55c;
extern float DAT_005cd124;
extern float DAT_005cd050;
extern float DAT_005d757c;
extern float DAT_005cc56c;
extern float DAT_005cd0fc;
extern float DAT_005cc574;
extern float DAT_005cd120;
extern float DAT_005cd0ec;
extern float DAT_005cc950;
extern float DAT_005cd11c;
extern float DAT_005cc564;
extern float DAT_005cc9b0;
extern float DAT_005cc9a0;
extern float DAT_007f100c;
extern int   DAT_007f0fd0;
extern V3    DAT_006146fc;
extern float DAT_0063c508[64];
extern V3    DAT_0063c208[64];
extern Rec64 DAT_0063bda0[4];
int    FUN_00418cd0(void *a, void *b);
void   FUN_00413c70(int a, int b, int c);
void   FUN_0041f260(int a, Mtx *out);
void   FUN_00453eb0(Sphere *p, float a, float b, float c, int d);
int    FUN_0045bfe0(void);
void   FUN_0045dce0(int a, V3 *p, int b);
void   FUN_004661a0(int a, int b);
void   FUN_00475ab0(int a, Rec64 *r, int b);
void   FUN_00476880(Sphere *p, V3 *axis, float a, float b);
void   FUN_00477760(int a, Rec64 *r, RGBA *c, float x, float y);
void   FUN_00484cf0(Query *q);
float  FUN_004b4510(float lo, float hi);
int    FUN_004b4cd0(int world, V3 *line, Hit *hit);
void   FUN_004b4650(V3 *out, V3 *a, V3 *b, int c);
void   FUN_004b5080(Mtx *out, void *a, V3 *b);
Frame *FUN_004c1340(Frame *f, V3 *v, int c);
Frame *FUN_004c1520(Frame *f, V3 *axis, float angle, int c);
void   FUN_004c15c0(Frame *f);
float  FUN_004c39b0(V3 *out, V3 *in);
float  FUN_004c3ac0(V3 *v);
void   FUN_004c3d60(V3 *out, V3 *in, Mtx *m);
Mtx   *FUN_004c4dc0(Mtx *out, Mtx *in);
Sphere *FUN_004e6100(Atomic *a);
#endif

/* 0x00418bd0, transcribed only for its register convention (EAX = out, ECX = world, EDX = line) */
static int FUN_00418bd0(Mtx *out, int world, V3 *line)
{
  V3 p;
  Hit hit;
  Mtx m;
  unsigned char *col;

  if (FUN_004b4cd0(world, line, &hit) != 0) {
    col = *(unsigned char **)(*(int *)(hit.f3c + 0x10) +
          ((unsigned int)*(unsigned short *)(*(int *)(hit.geom + 4) + hit.tri * 8 + 6) +
           (unsigned int)*(unsigned short *)(hit.geom + 0x80)) * 4);
    if ((((unsigned int)col[7] << 24) | ((unsigned int)col[6] << 16) | ((unsigned int)col[5] << 8) |
         (unsigned int)col[4]) != 0xff010101) {
      FUN_004b4650(&p, line, line + 1, hit.f38);
      p.x = hit.n.x * DAT_005cc9a0 + p.x;
      p.y = hit.n.y * DAT_005cc9a0 + p.y;
      p.z = hit.n.z * DAT_005cc9a0 + p.z;
      FUN_004b5080(&m, hit.pad10, &p);
      if (out != 0)
        *out = m;
      return 1;
    }
  }
  return 0;
}

/* 0x00418de0, copied from h1_00418e30.c for its register convention (EAX = rec) */
static void FUN_00418de0(Rec64 *rec)
{
  *(int *)&rec->state = 0;
  *(int *)&rec->timer = 0;
  *(int *)&rec->f58 = 0;
  *(int *)&rec->f5c = 0;
  *(int *)&rec->f50 = 0;
  *(int *)&rec->f54 = 0;
  FUN_00413c70(rec->f4c, 2, 0);
  FUN_004661a0(0x14, 0);
  FUN_004661a0(0x14, 1);
  FUN_004661a0(0x14, 2);
  FUN_004661a0(0x14, 3);
}

/* 0x00418f50, transcribed only for its register convention (EDI = pos) */
static void FUN_00418f50(V3 *pos)
{
  int i;
  int best;
  float m;

  for (i = 0; i < 64; i++) {
    if (DAT_0063c508[i] > DAT_005cc950) {
      DAT_0063c208[i] = *pos;
      DAT_0063c508[i] = 0.0f;
      break;
    }
  }
  if (i == 64) {
    best = 0;
    m = DAT_005d757c;
    for (i = 0; i < 64; i++) {
      if (DAT_0063c508[i] > m) {
        best = i;
        m = DAT_0063c508[i];
      }
    }
    DAT_0063c208[best] = *pos;
    DAT_0063c508[best] = 0.0f;
  }
}

static void FUN_00419a00(Rec64 *rec)
{
  Frame *frame;
  Mtx *mat;
  float d2;
  V3 dir;
  float k;
  V3 tgt;
  V3 pos;
  Sphere p;
  V3 start;
  V3 loc;
  V3 t;
  float l140;
  float s;
  V3 line[2];
  Mtx hit;
  Query q;
  Mtx inv;
  RGBA col;

  switch (rec->state) {
  case 1:
    frame = rec->atomic->parent;
    mat = &frame->modelling;
    if (rec->timer >= 10.0f)
      rec->state = 2;
    dir = rec->atomic->parent->modelling.pos;
    pos = dir;
    start = dir;
    dir.x = mat->at.x * -0.125f + dir.x;
    dir.y = mat->at.y * -0.125f + dir.y;
    dir.z = mat->at.z * -0.125f + dir.z;
    FUN_00418f50(&dir);
    FUN_0041f260(rec->f4c, &hit);
    tgt = hit.pos;
    dir.x = tgt.x - start.x;
    dir.y = tgt.y - start.y;
    dir.z = tgt.z - start.z;
    k = FUN_004c3ac0(&dir);
    d2 = dir.y * dir.y + dir.x * dir.x + dir.z * dir.z;
    l140 = k * 0.125f;
    k = l140 * DAT_005d757c;
    tgt.x = k + tgt.x;
    tgt.y = l140 + (tgt.y + 0.125f);
    tgt.z = k + tgt.z;
    FUN_004c4dc0(&inv, mat);
    FUN_004c3d60(&p.c, &tgt, &inv);
    FUN_004c39b0(&loc, &p.c);
    if (loc.y > 0.1f)
      rec->f50 = -8.0f;
    else if (loc.y < -0.1f)
      rec->f50 = 8.0f;
    else
      rec->f50 = 0.0f;
    if (loc.x > 0.1f)
      rec->f54 = 4.0f;
    else if (loc.x < -0.1f)
      rec->f54 = -4.0f;
    else
      rec->f54 = 0.0f;
    if (DAT_007f0fd0 == 7) {
      k = 2.0f;
      s = 2.0f;
    } else {
      k = (!(50.0f <= d2) ? (d2 >= DAT_005d757c ? d2 : DAT_005d757c) : 50.0f) * 0.005f + 0.75f;
      s = (!(50.0f <= d2) ? (d2 >= DAT_005d757c ? d2 : DAT_005d757c) : 50.0f) * 0.015f + 0.25f;
    }
    rec->f50 = s * rec->f50;
    rec->f54 = s * rec->f54;
    rec->f58 = rec->f54 + rec->f58;
    rec->f5c = rec->f50 + rec->f5c;
    FUN_004c15c0(frame);
    FUN_004c1520(frame, &DAT_006146fc, rec->f58, 0);
    FUN_004c1520(frame, &mat->right, rec->f5c, 2);
    s = k * 15.0f;
    t.x = frame->modelling.at.x * s;
    t.y = frame->modelling.at.y * s;
    t.z = frame->modelling.at.z * s;
    pos.x = t.x * DAT_007f100c + pos.x;
    pos.y = t.y * DAT_007f100c + pos.y;
    pos.z = t.z * DAT_007f100c + pos.z;
    FUN_004c1340(frame, &pos, 2);
    line[0] = start;
    line[1] = pos;
    if (FUN_00418bd0(&rec->m, FUN_0045bfe0(), line) != 0) {
      col.r = 0xff;
      col.g = 0x80;
      col.b = 0;
      col.a = 0x80;
      FUN_00475ab0(0, rec, 0);
      FUN_00477760(0, rec, &col, 2.0f, 4.0f);
      rec->state = 2;
    }
    if (rec->state != 2) {
      q.sphere = *FUN_004e6100(rec->atomic);
      q.kind = 5;
      q.rec = rec;
      q.zero = 0;
      q.cb = (void *)FUN_00418cd0;
      FUN_00484cf0(&q);
      rec->timer = DAT_007f100c + rec->timer;
    }
    FUN_0045dce0(0x14, &pos, rec->f60);
    FUN_00413c70(rec->f4c, 2, 1);
    break;
  case 2:
    p.c = rec->atomic->parent->modelling.pos;
    p.c.y = p.c.y - 0.5f;
    p.c.x = FUN_004b4510(-0.25f, 0.25f) + p.c.x;
    p.c.z = FUN_004b4510(-0.25f, 0.25f) + p.c.z;
    p.r = 2.0f;
    if (DAT_007f0fd0 == 7)
      FUN_00453eb0(&p, 30.0f, 700.0f, 1500.0f, 1);
    else
      FUN_00453eb0(&p, 30.0f, 200.0f, 300.0f, 1);
    FUN_00476880(&p, &DAT_006146fc, 30.0f, 1.75f);
    rec->state = 3;
    break;
  case 3:
    rec->state = 4;
    FUN_004661a0(0x14, rec->f60);
    break;
  case 4:
    FUN_00418de0(rec);
    break;
  }
}

/* stand-in for 0x00419fd0 (not matched): walks the 4 records calling 0x00419a00 with ESI = rec */
void caller_00419fd0(void)
{
  int i;
  for (i = 0; i < 4; i++)
    FUN_00419a00(&DAT_0063bda0[i]);
}
