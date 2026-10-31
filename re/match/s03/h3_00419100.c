// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00419100 _FUN_00419100
/* 0x00419100 (852 B). Best 85.9% (regs 85.9, ops 93.4), 213/208 ins, bytes not identical. Converter 39.9%.
   EAX = rec (static, caller 0x0041a1c5 walks the 2 x 0x54 records at 0x0063bf30, the pilot's Rec54).
   caller_0041a1be is a stand-in (not matched).
   What moved it: initialised V3 locals (r, axis, scale: their stores precede the NULL test), literal
   constants for the 0x005cXXXX floats, r.x / r.y computed as (..)*0.5f and then scaled by 1.2f / 0.9f in
   separate statements placed before the scale/matrix stores (79.8%), the angle k*90 put in a block temp
   before FUN_004c4d20 (85.4%), `i <= 3` for the final loop (the original's jle).
   Residual:
   1. the k clamp: the original keeps k on the x87 stack and puts the k = 0 block last. Ours places it
      first. Tried a temp, a ternary, a negated if and a goto.
   2. the case-1 / case-2..5 block order in the switch.
   3. in the final loop the original loads the table element first for x and z (fld [esi-4]; fmul s).
      Writing either operand order gives the same code in ours (cl canonicalises).
   Class: (b) for 1 and 2. 3 is an (a) candidate. */
typedef struct V3 { float x, y, z; } V3;
typedef struct RGBA { unsigned char r, g, b, a; } RGBA;
typedef struct Mtx {
  V3 right; unsigned int flags;
  V3 up;    int pad1;
  V3 at;    int pad2;
  V3 pos;   int pad3;
} Mtx;
typedef struct Obj { int type; struct Obj *parent; } Obj;
typedef struct Missile {
  float u;          /* +0x00 */
  float v;          /* +0x04 */
  int   pad08[2];
  RGBA  col;        /* +0x10 */
  int   pad14[8];
  int   f34;        /* +0x34 */
  int   pad38[8];
  int   f58;        /* +0x58 */
  float f5c;        /* +0x5c */
  int   f60;        /* +0x60 */
} Missile;
typedef struct Rec54 {
  int      vis[8];    /* +0x00 */
  Obj     *slot[8];   /* +0x20 */
  int      obj;       /* +0x40 */
  Obj     *obj4;      /* +0x44 */
  int      idx;       /* +0x48 */
  unsigned int flags; /* +0x4c */
  Missile *m;         /* +0x50 */
} Rec54;

#ifndef MATCH_PORT
extern float DAT_005d757c;
extern V3    DAT_005f32d0[4];
extern Rec54 DAT_0063bf30[2];
void FUN_004b5260(Obj *a, RGBA *c);
void FUN_004c1340(Obj *f, V3 *v, int c);
void FUN_004c1480(Obj *f, Mtx *m, int c);
void FUN_004c4d20(Mtx *m, V3 *axis, float angle, int c);
void FUN_004c5010(Mtx *m, V3 *s, int c);
void FUN_004c51a0(Mtx *m, V3 *r, int c);
#endif

static void FUN_00419100(Rec54 *rec)
{
  float k;
  float v2;
  V3 r = { 0.0f, 0.0f, 1.0f };
  V3 axis = { 0.0f, 0.0f, 1.0f };
  V3 scale = { 1.0f, 1.0f, 1.0f };
  V3 off;
  Mtx m;
  Missile *p;
  Obj *a;
  unsigned int fl;
  unsigned int v;
  int i;

  p = rec->m;
  if (p == 0)
    return;
  rec->flags = 0;
  if (p->f34 == 2)
    rec->flags = 1;
  if (p->f60 != 0)
    rec->flags |= 4;
  else if (p->f58 != 0)
    rec->flags |= 2;
  fl = rec->flags;
  for (i = 0; i < 8; i++) {
    a = rec->slot[i];
    rec->vis[i] = 0;
    if (a != 0 && (fl & 1)) {
      switch (i) {
      case 0:  v = 1; break;
      case 1:  v = fl & 4; break;
      case 2:
      case 3:
      case 4:
      case 5:  v = ~(fl >> 2) & 1; break;
      default: v = 0; break;
      }
      rec->vis[i] = v;
      FUN_004b5260(a, &rec->m->col);
    }
  }
  p = rec->m;
  v2 = p->f5c * 1.1111112f;
  if (1.0f <= v2 || v2 >= DAT_005d757c) {
    if (1.0f <= v2)
      k = 1.0f;
    else
      k = v2;
  } else {
    k = 0.0f;
  }
  r.x = ((1.0f - p->u) * 2.0f - 1.0f) * 0.5f;
  r.y = ((1.0f - p->v) * 2.0f - 1.0f) * 0.5f;
  m.flags |= 0x20003;
  scale.y = 1.0f;
  scale.x = 1.0f;
  m.at.z = 1.0f;
  m.up.y = 1.0f;
  m.right.x = 1.0f;
  m.up.x = 0.0f;
  m.right.z = 0.0f;
  m.right.y = 0.0f;
  r.x = r.x * 1.2f;
  r.y = r.y * 0.9f;
  m.at.y = 0.0f;
  m.at.x = 0.0f;
  m.up.z = 0.0f;
  m.pos.z = 0.0f;
  m.pos.y = 0.0f;
  m.pos.x = 0.0f;
  FUN_004c51a0(&m, &r, 0);
  FUN_004c5010(&m, &scale, 1);
  FUN_004c1480(rec->obj4, &m, 0);
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
  { float ang = k * 90.0f;
  FUN_004c4d20(&m, &axis, ang, 1); }
  FUN_004c1480(rec->slot[2]->parent->parent, &m, 0);
  k = (1.0f - k) * 0.025f;
  for (i = 0; i <= 3; i++) {
    off.x = DAT_005f32d0[i].x * k;
    off.y = k * DAT_005f32d0[i].y;
    off.z = DAT_005f32d0[i].z * k;
    FUN_004c1340(rec->slot[2 + i]->parent, &off, 0);
  }
}

/* stand-in for the loop at 0x0041a1be..0x0041a1d3 (not matched): EAX = rec */
void caller_0041a1be(void)
{
  int i;
  for (i = 0; i < 2; i++)
    FUN_00419100(&DAT_0063bf30[i]);
}
