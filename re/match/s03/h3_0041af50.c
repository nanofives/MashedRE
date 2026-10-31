// FLAGS: /O2 /GS /Ob1
// MATCH: 0x0041ae60 _FUN_0041ae60
// MATCH: 0x0041af00 _FUN_0041af00
// MATCH: 0x0041af50 _FUN_0041af50
/* Static group: 0x0041ae60 (145 B) and 0x0041af00 (78 B) BYTE-IDENTICAL, 0x0041af50 (998 B) best 96.3%
   (regs 96.3, ops 98.5), 267/269 ins, bytes not identical. Converter was 26.8%.
   Register args: 0x0041af50 takes EAX = rec (caller 0x0041b608, loop over 4 x 0x74 records at
   0x0063c8d0). 0x0041ae60 / 0x0041af00 take ESI = rec, EDI = &scale. All three are static callees
   defined before their caller. caller_0041b5fc is a stand-in for that loop (not matched).
   What moved it: the _DAT_005cXXXX operands are compiler float literals (0x005cc94c = 4294967296.0, the
   unsigned->float fixup). Written as literals (1.0f, 0.015f, 1.125f, 0.2f, 0.04f, 25.0f), cl loads the
   member first and uses fmul/fadd [const], as the original does (83.0 -> 88.8). 0x005d757c (0.0) stays an
   extern: the original reads it from memory. Also a switch on FUN_0040b420, fabs() without float casts
   with the |rot| operand on the left, `(float)sin(x) * 0.2f + 1.0f`, and `0 >= v` spellings.
   Residual:
   1. m.flags |= 0x20003: the original loads the flags early and stores them after the 12 matrix element
      stores. Ours stores them first. Moving the statement last moves the load too (94.1%).
   2. s.y *= 1.125f: fld s.y is scheduled one push later in ours.
   3. the [0, 1] clamp: the original duplicates v (fld st0 / fcomp / fstp st0) before comparing with
      1.0f. Four source forms tried (if/else, ternary, a temp copy, a split if). None gives the dup.
   Class: (b) for 3. For 1 and 2, (a) candidate x87/store scheduling, not proven. */
typedef struct V3 { float x, y, z; } V3;
typedef struct Mtx {
  V3 right; unsigned int flags;
  V3 up;    int pad1;
  V3 at;    int pad2;
  V3 pos;   int pad3;
} Mtx;
typedef struct Atomic { int type; void *parent; } Atomic;
typedef struct Rec74 {
  int     f0;        /* +0x00 */
  int     f4;        /* +0x04 */
  Atomic *a08;       /* +0x08 */
  int     f0c;       /* +0x0c */
  int     f10;       /* +0x10 */
  Atomic *a14;       /* +0x14 */
  int     f18;       /* +0x18 */
  Atomic *part[9];   /* +0x1c */
  int     f40[4];    /* +0x40 */
  V3      rot;       /* +0x50 */
  int     f5c;       /* +0x5c */
  void   *frame;     /* +0x60 */
  int     idx;       /* +0x64 */
  unsigned int flags;/* +0x68 */
  int     active;    /* +0x6c */
  float   t;         /* +0x70 */
} Rec74;

#ifndef MATCH_PORT
extern int   DAT_0063c8c8;
extern int   DAT_0063c8a0;
extern unsigned int DAT_0063cab0;
extern int   DAT_0063c8a4[];
extern int   DAT_0063c8b4[];
extern int   DAT_0063c88c[];
extern int   DAT_0063c878[];
extern int   DAT_0063caa0[];
extern V3    DAT_005f3304[];
extern Rec74 DAT_0063c8d0[4];
extern float DAT_005d757c;
double sin(double x);
double fabs(double x);
#pragma intrinsic(sin, fabs)
int  FUN_0040b420(int i);
int  FUN_0040b6d0(int i);
int  FUN_0040b890(void);
void FUN_004c13e0(void *frame, V3 *s, int c);
void FUN_004c1480(void *frame, Mtx *m, int c);
void FUN_004c51a0(Mtx *m, V3 *r, int c);
#endif

static void FUN_0041ae60(Rec74 *rec, V3 *s)
{
  if (rec->flags & 1)    FUN_004c13e0(rec->part[0]->parent, s, 0);
  if (rec->flags & 2)    FUN_004c13e0(rec->part[1]->parent, s, 0);
  if (rec->flags & 4)    FUN_004c13e0(rec->part[2]->parent, s, 0);
  if (rec->flags & 8)    FUN_004c13e0(rec->part[3]->parent, s, 0);
  if (rec->flags & 0x10) FUN_004c13e0(rec->part[4]->parent, s, 0);
  if (rec->flags & 0x20) FUN_004c13e0(rec->part[5]->parent, s, 0);
}

static void FUN_0041af00(Rec74 *rec, V3 *s)
{
  if (rec->flags & 0x2000) FUN_004c13e0(rec->part[6]->parent, s, 0);
  if (rec->flags & 0x4000) FUN_004c13e0(rec->part[7]->parent, s, 0);
  if (rec->flags & 0x8000) FUN_004c13e0(rec->part[8]->parent, s, 0);
}

static void FUN_0041af50(Rec74 *rec)
{
  V3 s;
  int num;
  int den;
  int i;
  Mtx m;
  Atomic *a;
  float v;

  s.x = 1.0f;
  s.y = 1.0f;
  s.z = 1.0f;
  num = FUN_0040b6d0(rec->idx);
  den = FUN_0040b890();
  i = rec->idx;
  if (DAT_0063c8a4[i] != 0) rec->flags |= 0x40;
  else rec->flags &= ~0x40;
  if (DAT_0063c8b4[i] != 0 && DAT_0063c8c8 == 0) rec->flags |= 0x80;
  else rec->flags &= ~0x80;
  if (DAT_0063c88c[i] != 0 && DAT_0063c8a0 == 0) rec->flags |= 0x100;
  else rec->flags &= ~0x100;
  if (DAT_0063c878[i] != 0) rec->flags |= 0x200;
  else rec->flags &= ~0x200;
  if (DAT_0063caa0[i] != 0) rec->flags |= 0x400;
  else rec->flags &= ~0x400;
  if ((rec->flags & 0x40) && (rec->flags & 0x100))
    rec->flags &= ~0x100;
  if (rec->flags & 0x400)
    rec->flags &= ~0x3c0;
  rec->flags &= ~0xe000;
  if (rec->flags & 0x400) {
    switch (FUN_0040b420(i)) {
    case -2: rec->flags |= 0x8000; break;
    case -1: rec->flags |= 0x4000; break;
    case 0:  rec->flags |= 0x2000; break;
    }
    rec->t = rec->t + 1.0f;
  }
  if (den == 8)
    rec->flags = (rec->flags & ~0x1000) | 0x800;
  else if (den == 12)
    rec->flags = (rec->flags & ~0x800) | 0x1000;
  if (fabs(rec->rot.x) > fabs(DAT_005f3304[rec->idx].x)) {
    if (rec->idx & 1)
      rec->rot.x = rec->rot.x + 0.015f;
    else
      rec->rot.x = rec->rot.x - 0.015f;
  }
  m.flags |= 0x20003;
  m.at.z = 1.0f;
  m.up.y = 1.0f;
  m.right.x = 1.0f;
  m.up.x = 0.0f;
  m.right.z = 0.0f;
  m.right.y = 0.0f;
  m.at.y = 0.0f;
  m.at.x = 0.0f;
  m.up.z = 0.0f;
  m.pos.z = 0.0f;
  m.pos.y = 0.0f;
  m.pos.x = 0.0f;
  FUN_004c51a0(&m, &rec->rot, 0);
  FUN_004c1480(rec->frame, &m, 0);
  s.x = s.x * 1.125f;
  s.y = s.y * 1.125f;
  s.z = s.z * 1.125f;
  FUN_004c13e0(rec->frame, &s, 1);
  s.x = (float)num / (float)den;
  s.z = 1.0f;
  s.y = 1.0f;
  if (rec->flags & 0x800) {
    a = rec->a14;
    FUN_004c13e0(a->parent, &s, 0);
  } else if (rec->flags & 0x1000) {
    a = rec->a08;
    FUN_004c13e0(a->parent, &s, 0);
  }
  if ((rec->flags & 0x80) && !(rec->flags & 0x40)) {
    v = (float)sin((float)DAT_0063cab0 * 0.2f) * 0.2f + 1.0f;
    s.y = v;
    s.x = v;
  } else {
    s.y = 1.0f;
    s.x = 1.0f;
  }
  s.z = 1.0f;
  FUN_0041ae60(rec, &s);
  if (rec->flags & 0x400) {
    v = 1.0f - rec->t * 0.04f;
    if (DAT_005d757c >= v)
      v = DAT_005d757c;
    s.y = v;
    s.x = v;
    s.z = 1.0f;
    FUN_0041ae60(rec, &s);
    v = (rec->t - 25.0f) * 0.04f;
    if (!(DAT_005d757c >= v) && !(1.0f > v))
      v = 1.0f;
    else if (DAT_005d757c >= v)
      v = DAT_005d757c;
    s.y = v;
    s.x = v;
    s.z = 1.0f;
    FUN_0041af00(rec, &s);
  }
}

/* stand-in for the loop at 0x0041b5fc..0x0041b616 (not matched): EAX = rec */
void caller_0041b5fc(void)
{
  int i;
  for (i = 0; i < 4; i++)
    if (DAT_0063c8d0[i].active != 0)
      FUN_0041af50(&DAT_0063c8d0[i]);
}
