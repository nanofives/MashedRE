// FLAGS: /O2 /GS /Ob1
/* a1 0x004019d0 (973 B). Best: 76.0% (start g2s 12.4%). Same frame-matrix setup as 0x00401da0 for the
   trophy clump, then per-player a[] clumps (frame transform by FUN_0046d4a0 matrix, scale 0.04/slot scale,
   material colour from a local RGBA[6] indexed by DAT_007f1a1c[i].colour, stride 0x10) and per-b[] clumps
   placed at the midpoint of FUN_00426d00(prev,0)/(prev,3), colour {ff,0,0,80}.
   Fixed: Slot struct, RGBA[6] (Ghidra showed them as floats), scale macro `v * (1.0f/slot->scale)` (fld 1;
   fdiv; fmul order), a+b add order, literals for 0x005cc320/0x005cc32c, block-scoped cols/c so they share
   frame slots, a/b/mid at function scope. Permuter 3138 compiles (+4.4; one variant broke semantics, fixed).
   Residual (b): frame layout. Original slots: i +0, p +4, n +8, pm +0xc, s +0x10, scl +0x1c, cols/mid +0x28,
   pos +0x40, a +0x4c, b +0x58 (with c sharing i's slot and list sharing pm's: lifetime packing); 9176 here
   puts pos/cols higher. Also (b) as in 0x00401da0: original hoists the &centre reload + translate pushes
   above the identity stores. Declaration order does not move 9176's layout; scope changes do. */
typedef struct { float x, y, z; } V3;
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct RwMatrix { V3 right; unsigned int flags; V3 up; int pad1; V3 at; int pad2; V3 pos; int pad3; } RwMatrix;
typedef struct RwFrame { int obj[2]; int link[2]; RwMatrix modelling; } RwFrame;
typedef struct RpClump { int type; RwFrame *frame; } RpClump;
typedef struct RpMaterial { int texture; RGBA color; } RpMaterial;
typedef struct Slot {
  RpClump *clump;   /* +0x00 */
  V3 centre;        /* +0x04 */
  V3 bmin;          /* +0x10 */
  V3 bmax;          /* +0x1c */
  V3 ext;           /* +0x28 */
  float scale;      /* +0x34 */
  int id;           /* +0x38 */
  int pad3c;
  RpClump *a[4];    /* +0x40 */
  RpClump *b[6];    /* +0x50 */
} Slot;             /* 0x68 */
typedef struct Player { int colour; int pad[3]; } Player;
extern Slot *DAT_00636ac0;
extern V3 DAT_006146f0;
extern V3 DAT_006146fc;
extern Player DAT_007f1a1c[];
int FUN_0040e340(void);
int FUN_00412130(void);
int *FUN_00426cf0(void);
V3 *FUN_00426d00(int p0, int p1);
RwMatrix *FUN_00467210(int p0);
int FUN_0046d4a0(RwMatrix **out, int i);
RpMaterial *FUN_004b4080(RpClump *c);
void FUN_004b42c0(RwFrame *f, V3 *a, V3 *b);
RwFrame *FUN_004c1340(RwFrame *f, V3 *v, int op);
RwFrame *FUN_004c13e0(RwFrame *f, V3 *v, int op);
RwFrame *FUN_004c1480(RwFrame *f, RwMatrix *m, int op);
RwMatrix *FUN_004c4d20(RwMatrix *m, V3 *axis, float angle, int op);
RwMatrix *FUN_004c5010(RwMatrix *m, V3 *v, int op);
RwMatrix *FUN_004c51a0(RwMatrix *m, V3 *v, int op);
// MATCH: 0x004019d0 FUN_004019d0
void FUN_004019d0(void)
{
  int i;
  int n;
  V3 pos;
  V3 a, b, mid;
  V3 scl;
  RwFrame *f;
  RwMatrix *m;
  RwMatrix *cm;
  pos.x = 1.7f; pos.y = -1.0f; pos.z = 4.0f;
  i = 0;
  scl.x = 2.0f; scl.y = 2.0f; scl.z = 2.0f;
  n = FUN_0040e340();
  FUN_00426cf0();
  if (DAT_00636ac0->clump != 0) {
    f = DAT_00636ac0->clump->frame;
    m = &f->modelling;
    scl.x = DAT_00636ac0->scale;
    scl.y = DAT_00636ac0->scale;
    scl.z = DAT_00636ac0->scale;
    cm = FUN_00467210(0);
    m->right.x = m->up.y = m->at.z = 1.0f;
    m->right.y = m->right.z = m->up.x = 0.0f;
    m->up.z = m->at.x = m->at.y = 0.0f;
    m->pos.x = m->pos.y = m->pos.z = 0.0f;
    m->flags = m->flags | 0x20003;
    FUN_004c51a0(m, &DAT_00636ac0->centre, 0);
    FUN_004c4d20(m, &DAT_006146f0, -90.0f, 2);
    FUN_004c5010(m, &scl, 2);
    FUN_004c51a0(m, &pos, 2);
    FUN_004c1480(f, cm, 2);
    if (n > 0) {
      RGBA cols[6];
      Player *p;
      RwMatrix *pm;
      V3 s2;
      cols[3].r = 0xff; cols[3].g = 0xff; cols[3].b = 0; cols[3].a = 200;
      cols[4].r = 0xff; cols[4].g = 0; cols[4].b = 0xff; cols[4].a = 200;
      cols[5].r = 0; cols[5].g = 0; cols[5].b = 0; cols[5].a = 200;
      cols[0].r = 0xff; cols[0].g = 0; cols[0].b = 0; cols[0].a = 200;
      cols[1].r = 0; cols[1].g = 0; cols[1].b = 0xff; cols[1].a = 200;
      cols[2].r = 0; cols[2].g = 0xff; cols[2].b = 0; cols[2].a = 200;
      for (; n > i; i++) {
        RwFrame *cf;
        s2.x = 0.04f; s2.y = 0.04f; s2.z = 0.04f;
        FUN_0046d4a0(&pm, i);
        cf = DAT_00636ac0->a[i]->frame;
        FUN_004c1480(cf, pm, 0);
        FUN_004c1480(cf, m, 2);
        s2.x = s2.x * (1.0f / DAT_00636ac0->scale);
        s2.y = s2.y * (1.0f / DAT_00636ac0->scale);
        s2.z = s2.z * (1.0f / DAT_00636ac0->scale);
        FUN_004c13e0(cf, &s2, 1);
        FUN_004b4080(DAT_00636ac0->a[i])->color = cols[DAT_007f1a1c[i].colour];
      }
    }
    {
      RGBA c;
      int *list;
      V3 s3;
      int j;
      c.r = 0xff; c.g = 0; c.b = 0; c.a = 0x80;
      list = FUN_00426cf0();
      s3.x = 0.01f; s3.y = 0.04f; s3.z = 0.01f;
      n = FUN_00412130();
      s3.x = s3.x * (1.0f / DAT_00636ac0->scale);
      s3.y = s3.y * (1.0f / DAT_00636ac0->scale);
      s3.z = s3.z * (1.0f / DAT_00636ac0->scale);
      for (j = 0; j < n; j++) {
        int prev;
        RwFrame *bf;
        if (j == 0) prev = 0; else prev = list[j - 1];
        a = *FUN_00426d00(prev, 0);
        b = *FUN_00426d00(prev, 3);
        mid.z = a.z + b.z;
        mid.x = a.x + b.x;
        mid.y = a.y + b.y;
        mid.x = mid.x * 0.5f;
        mid.y = mid.y * 0.5f;
        mid.z = mid.z * 0.5f;
        bf = DAT_00636ac0->b[j]->frame;
        FUN_004c1340(bf, &mid, 0);
        FUN_004b42c0(bf, &a, &DAT_006146fc);
        FUN_004c1480(bf, m, 2);
        FUN_004c13e0(bf, &s3, 1);
        FUN_004b4080(DAT_00636ac0->b[j])->color = c;
      }
    }
  }
}
