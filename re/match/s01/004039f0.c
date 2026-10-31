// FLAGS: /O2 /GS /Ob1
/* a2 0x004039f0 (611 B). Best: 97.1%.
   g2s 56.2%. Fixed: float globals (30.0f at 0x005ea0ac is .data, 0.0f at 0x005d757c is the shared .rdata
   zero), 6 x 0x14 bonus records at 0x00636b88, the six `.done = 0` stores written out followed by i = 6
   (a loop is not unrolled by 9176, 72.5%), the second loop continuing from i. EBX = 0 throughout came for
   free once the stores were explicit. floor = 0x004a3090 (not intrinsic), _ftol 0x004a2c48.
   Residual class (b): scheduling of the 12-byte RwV3d copy of matrix+0x30 (original stores pos.y/pos.z
   after the size constants). Tried: size first (95.9), field-by-field (61.6), pointer temp (97.1), size
   field order (95.9), permuter (no gain). */
typedef struct { float x, y, z; } RwV3d;
typedef struct { RwV3d right; int f0; RwV3d up; int f1; RwV3d at; int f2; RwV3d pos; int f3; } RwMatrix;
typedef struct {
  float bonus;    /* 0x00636b88 + i*0x14 */
  int   time;     /* +0x04 */
  int   done;     /* +0x08 */
  int   active;   /* +0x0c */
  float scale;    /* +0x10 */
} Bonus14;
extern float DAT_007f0fe4;
extern float DAT_007f100c;
extern float DAT_005ea0ac;
extern float DAT_005ea0a8;
extern int DAT_005ea0b0;
extern Bonus14 DAT_00636b88[6];
extern int DAT_007f0fe0;
extern RwV3d DAT_006146fc;
float FUN_00408a50(int p0);
float FUN_00417730(int p0);
RwMatrix *FUN_0041f290(int p0, int p1);
void FUN_004215c0(int p0, float p1, int p2);
int FUN_00422fd0(int p0);
void FUN_004633d0(int p0, float p1);
void FUN_00476860(RwV3d *p0, RwV3d *p1, float p2);
int FUN_00480720(int p0);
void FUN_00481750(int p0, RwV3d *p1, RwV3d *p2);
void FUN_00486460(RwV3d *p0, float p1);
double floor(double);

// MATCH: 0x004039f0 FUN_004039f0
void FUN_004039f0(void)
{
  RwMatrix *m;
  int i;
  float t;
  RwV3d pos;
  RwV3d size;

  i = 0;
  m = FUN_0041f290(0, 0x16);
  DAT_007f0fe4 = DAT_007f0fe4 - DAT_007f100c;
  DAT_005ea0a8 = DAT_007f0fe4 / DAT_005ea0ac;
  if (DAT_005ea0a8 > 1.0f) {
    DAT_005ea0a8 = 1.0f;
  } else if (DAT_005ea0a8 < 0.0f) {
    DAT_005ea0a8 = 0.0f;
    FUN_004633d0(3, 0.0f);
  }
  FUN_004633d0(2, DAT_005ea0a8);
  if (floor(FUN_00417730(0)) != DAT_005ea0b0) {
    DAT_00636b88[0].done = 0;
    DAT_00636b88[1].done = 0;
    DAT_00636b88[2].done = 0;
    DAT_00636b88[3].done = 0;
    DAT_00636b88[4].done = 0;
    DAT_00636b88[5].done = 0;
    i = 6;
    DAT_005ea0b0 = (int)floor(FUN_00417730(0));
  }
  DAT_00636b88[0].active = DAT_005ea0b0 > 0;
  if (DAT_007f0fe4 <= 0.0f) {
    pos = m->pos;
    size.x = 50.0f; size.y = 1000.0f; size.z = 50.0f;
    FUN_004215c0(DAT_007f0fe0, 50.0f, 0);
    FUN_004215c0(DAT_007f0fe0, 50.0f, 1);
    FUN_00476860(&pos, &DAT_006146fc, 360.0f);
    FUN_00480720(DAT_007f0fe0);
    pos.z += 0.03f;
    pos.x += 0.03f;
    FUN_00481750(DAT_007f0fe0, &size, &pos);
    FUN_00486460(&m->pos, 2.0f);
    FUN_00486460(&m->pos, 1.0f);
    FUN_00422fd0(0);
  }
  for (; i < 6; i++) {
    if (DAT_00636b88[i].active != 0 && DAT_00636b88[i].done == 0) {
      t = FUN_00408a50(0);
      if (t >= DAT_00636b88[i].time && t < DAT_00636b88[i].time + 1.0f) {
        DAT_007f0fe4 += DAT_00636b88[i].bonus;
        DAT_00636b88[i].done = 1;
        DAT_00636b88[i].scale = 1.6f;
        FUN_004633d0(1, 0.0f);
      }
    }
  }
}
