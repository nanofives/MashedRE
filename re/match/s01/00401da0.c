// FLAGS: /O2 /GS /Ob1
/* a1 0x00401da0 (309 B). Best: 93.5% (start g2s 15.2%). Trophy clump frame: RwMatrixSetIdentity macro
   (chained 1.0 / 0.0 stores, flags |= 0x20003), translate(centre), rotate(&m->right,-30), rotate(&f->modelling.up,
   angle DAT_00636ac4 += 1), scale, translate, FUN_004c1480 frame transform. Fixed: typed RW frame/matrix,
   scale vector as three separate `= slot->scale` stores then `*= 0.7f`, literals for 0x005cc320/0x005cc340.
   Residual (b), 4 instruction slots: original stores the angle (fstp 0x00636ac4) after the 1.0 stores and pushes
   the translate args (reload of DAT_00636ac0, +4) BEFORE the zero stores; 9176 pushes after. Tried angle
   update at 6 positions, flags statement at 4 positions, do/while(0) and {} macro blocks, comma-expression and
   __inline SetIdentity as the call argument, a hoisted centre pointer; permuter 317 compiles, no gain. */
typedef struct { float x, y, z; } V3;
typedef struct RwMatrix { V3 right; unsigned int flags; V3 up; int pad1; V3 at; int pad2; V3 pos; int pad3; } RwMatrix;
typedef struct RwFrame { int obj[2]; int link[2]; RwMatrix modelling; } RwFrame;
typedef struct RpClump { int type; RwFrame *frame; } RpClump;
typedef struct Slot {
  RpClump *clump;   /* +0x00 */
  V3 centre;        /* +0x04 */
  V3 bmin;          /* +0x10 */
  V3 bmax;          /* +0x1c */
  V3 ext;           /* +0x28 */
  float scale;      /* +0x34 */
  int id;           /* +0x38 */
  int pad3c;
  int a[4];         /* +0x40 */
  int b[6];         /* +0x50 */
} Slot;             /* 0x68 */
extern Slot *DAT_00636ac0;
extern float DAT_00636ac4;
void FUN_00426cf0(void);
RwMatrix *FUN_00467210(int p0);
RwFrame *FUN_004c1480(RwFrame *f, RwMatrix *m, int op);
RwMatrix *FUN_004c4d20(RwMatrix *m, V3 *axis, float angle, int op);
RwMatrix *FUN_004c5010(RwMatrix *m, V3 *v, int op);
RwMatrix *FUN_004c51a0(RwMatrix *m, V3 *v, int op);
// MATCH: 0x00401da0 FUN_00401da0
void FUN_00401da0(void)
{
  V3 scl;
  V3 pos;
  RwFrame *f;
  RwMatrix *m;
  RwMatrix *cm;
  pos.x = 0.4f; pos.y = -0.2f; pos.z = 1.0f;
  scl.x = 2.0f; scl.y = 2.0f; scl.z = 2.0f;
  FUN_00426cf0();
  if (DAT_00636ac0->clump != 0) {
    f = DAT_00636ac0->clump->frame;
    m = &f->modelling;
    scl.x = DAT_00636ac0->scale;
    scl.y = DAT_00636ac0->scale;
    scl.z = DAT_00636ac0->scale;
    scl.x = scl.x * 0.7f;
    scl.y = scl.y * 0.7f;
    scl.z = scl.z * 0.7f;
    cm = FUN_00467210(0);
    DAT_00636ac4 = DAT_00636ac4 + 1.0f;
    m->right.x = m->up.y = m->at.z = 1.0f;
    m->right.y = m->right.z = m->up.x = 0.0f;
    m->up.z = m->at.x = m->at.y = 0.0f;
    m->pos.x = m->pos.y = m->pos.z = 0.0f;
    m->flags = m->flags | 0x20003;
    FUN_004c51a0(m, &DAT_00636ac0->centre, 0);
    FUN_004c4d20(m, &m->right, -30.0f, 2);
    FUN_004c4d20(m, &f->modelling.up, DAT_00636ac4, 2);
    FUN_004c5010(m, &scl, 2);
    FUN_004c51a0(m, &pos, 2);
    FUN_004c1480(f, cm, 2);
  }
}
