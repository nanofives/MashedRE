// FLAGS: /O2 /GS /Ob1
/* a1 0x004011f0 (325 B). Best: 100.0% BYTES-OK. Start (g2s) 59.6%.
   Camera + light frame setup (FUN_004c1xxx = RwFrame translate/rotate/scale/transform family).
   Fixes: globals are object pointers whose +4 is the frame; FUN_00429300 returns float and 0x004a2c48 is the
   (int) cast (__ftol); constants 0x005cc314..0x005cc324 are literals 1.7f/0.6f/3.0f/1.0f/0.033333335f;
   `v2 = pos;` struct copy then per-component adjust; pos.z must be two statements (`... * 3.0f;` then
   `pos.z += 3.0f;`): as one expression 9176 folds a*3+3 into (a+1)*3. */
typedef struct { float x, y, z; } V3;
typedef struct RwFrame RwFrame;
typedef struct RwObj { int type; RwFrame *frame; } RwObj;
extern RwObj *DAT_00636564;
extern RwObj *DAT_00636568;
extern RwObj *DAT_0063656c;
extern float DAT_00636574;
extern V3 DAT_006146fc;
float FUN_00429300(void);
void FUN_004b42c0(RwFrame *f, void *m, V3 *v);
void FUN_004c0e50(RwFrame *f);
RwFrame *FUN_004c1340(RwFrame *f, V3 *v, int op);
RwFrame *FUN_004c13e0(RwFrame *f, V3 *v, int op);
RwFrame *FUN_004c1480(RwFrame *f, void *m, int op);
RwFrame *FUN_004c1520(RwFrame *f, V3 *axis, float angle, int op);
void FUN_004c15c0(RwFrame *f);
// MATCH: 0x004011f0 FUN_004011f0
void FUN_004011f0(void)
{
  RwFrame *f;
  RwFrame *lf;
  char *m;
  int n;
  V3 pos;
  V3 v2;
  V3 scl;
  f = DAT_00636564->frame;
  pos.x = -1.5f; pos.y = -1.2f; pos.z = 3.0f;
  scl.x = 1.5f; scl.y = 1.5f; scl.z = 1.5f;
  lf = DAT_0063656c->frame;
  m = (char *)DAT_00636568->frame + 0x10;
  n = (int)FUN_00429300();
  if (n > 30) n = 30;
  pos.z = (1.0f - (float)n * 0.033333335f) * 3.0f;
  pos.z += 3.0f;
  FUN_004c15c0(f);
  FUN_004c1340(f, &pos, 0);
  FUN_004c1520(f, &DAT_006146fc, DAT_00636574, 1);
  FUN_004c13e0(f, &scl, 1);
  FUN_004c1480(f, m, 2);
  FUN_004c0e50(f);
  v2 = pos;
  v2.x = v2.x - 1.0f;
  v2.y = v2.y + 0.6f;
  v2.z = v2.z - 1.7f;
  FUN_004c1340(lf, &v2, 0);
  FUN_004c1480(lf, m, 2);
  FUN_004b42c0(lf, (char *)f + 0x40, &DAT_006146fc);
}
