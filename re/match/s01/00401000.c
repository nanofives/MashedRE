// FLAGS: /O2 /GS /Ob1
/* a1 0x00401000 (482 B). Best: 100.0% BYTES-OK. Start (g2s) 72.1%.
   Fixes: locals as real typed objects (V3 min/max, RGBAf ambient/colour, 24-byte BBox, char[256] name; the
   declaration order above gives the original frame layout); float prototypes for RpLightSetConeAngle /
   FUN_004e48e0 (immediate float pushes); sprintf-style FUN_004a2b60 varargs; DAT_00636574 is a float;
   the FUN_004b4080 result through a named temp (nested call pushes the 0 first). /GS cookie tail-jump
   to 0x004a2be9 comes out by itself. */
typedef struct { float x, y, z; } V3;
typedef struct { float r, g, b, a; } RGBAf;
typedef struct { V3 sup, inf; } BBox;
typedef struct RpLight { unsigned char type, subType, flags, privateFlags; void *parent; } RpLight;
extern int DAT_00636568;
extern int DAT_00636560;
extern int DAT_00636564;
extern int DAT_00636ad0;
extern float DAT_00636574;
extern RpLight *DAT_00636570;
extern RpLight *DAT_0063656c;
int FUN_004671a0(int p0);
int FUN_004a2b60(char *buf, const char *fmt, ...);
int FUN_004b3bf0(char *name, int a, int b);
int FUN_004b4080(int clump);
void FUN_004b5320(int clump, int a, int b);
void FUN_004c0740(RpLight *l, int frame);
int FUN_004c0b30(void);
BBox *FUN_004c1800(BBox *b, V3 *v);
BBox *FUN_004c1840(BBox *b, V3 *v);
RpLight *FUN_004e48e0(RpLight *l, float r);
RpLight *FUN_004e4900(RpLight *l, RGBAf *c);
int FUN_0053fbb0(int a, int b);
RpLight *RpLightCreate(int type);
RpLight *RpLightSetConeAngle(RpLight *l, float a);
int RpWorldAddLight(int w, RpLight *l);
int RpWorldCreate(BBox *b);
int RwFrameAddChild(int a, int b);
// MATCH: 0x00401000 FUN_00401000
void FUN_00401000(void)
{
  V3 vmin;
  V3 vmax;
  RGBAf amb;
  RGBAf col;
  BBox bb;
  char name[256];
  col.r = 1.0f; col.g = 1.0f; col.b = 1.0f; col.a = 1.0f;
  amb.r = 0.75f; amb.g = 0.75f; amb.b = 0.75f; amb.a = 1.0f;
  vmin.x = -100.0f; vmin.y = -100.0f; vmin.z = -100.0f;
  vmax.x = 100.0f; vmax.y = 100.0f; vmax.z = 100.0f;
  DAT_00636568 = FUN_004671a0(0);
  FUN_004c1800(&bb, &vmin);
  FUN_004c1840(&bb, &vmax);
  DAT_00636560 = RpWorldCreate(&bb);
  FUN_004a2b60(name, "cup.dff");
  DAT_00636564 = FUN_004b3bf0(name, 0, 0);
  FUN_004b5320(DAT_00636564, 0x40, 1);
  if (DAT_00636ad0 != 0) {
    int t = FUN_004b4080(DAT_00636564);
    FUN_0053fbb0(t, 0);
  }
  DAT_00636574 = 0.0f;
  DAT_00636570 = RpLightCreate(2);
  FUN_004e4900(DAT_00636570, &amb);
  DAT_0063656c = RpLightCreate(0x82);
  FUN_004c0740(DAT_0063656c, FUN_004c0b30());
  RpLightSetConeAngle(DAT_0063656c, 0.78539819f);
  DAT_0063656c->flags = 1;
  FUN_004e4900(DAT_0063656c, &col);
  FUN_004e48e0(DAT_0063656c, 100.0f);
  RpWorldAddLight(DAT_00636560, DAT_0063656c);
  RpWorldAddLight(DAT_00636560, DAT_00636570);
  RwFrameAddChild(DAT_00636560, DAT_00636564);
}
