// FLAGS: /O2 /GS /Ob1
/* a2 0x00403160 (230 B). Best: 100%.
   g2s 62.3%: cookie dropped, viewWindow copy as two dwords. Fixed: dead char array for the /GS cookie
   (frame is 2 x RwV2d + cookie, no room for a real buffer), RwV2d struct copy of camera +0x68,
   RwCamera End/Begin/SetViewWindow (0x004c19f0 / 0x004c1a00 / 0x004c1c80) as cdecl. */
typedef struct { float x, y; } RwV2d;
typedef struct RwCamera { char pad0[0x68]; RwV2d viewWindow; } RwCamera;
typedef struct {
  void *s[8];
  int (*RenderStateSet)(int state, int value); /* +0x20, cdecl */
} RwGlobalsT;
extern RwGlobalsT *DAT_007d3ff8;
extern int DAT_00771964;
void FUN_00402fb0(void);
void FUN_00428760(int p0, float p1, float p2, float p3, float p4, int p5);
RwCamera *FUN_004671a0(int p0);
RwCamera *FUN_004c19f0(RwCamera *cam);
RwCamera *FUN_004c1a00(RwCamera *cam);
RwCamera *FUN_004c1c80(RwCamera *cam, const RwV2d *vw);

// MATCH: 0x00403160 FUN_00403160
void FUN_00403160(void)
{
  char buf[64]; /* unused, only makes /GS emit the cookie (see 0x00403050) */
  RwV2d vw;
  RwV2d save;
  RwCamera *cam;
  (void)buf;
  vw.x = 0.8f;
  vw.y = 0.8f;
  cam = FUN_004671a0(0);
  FUN_004c19f0(cam);
  save = cam->viewWindow;
  FUN_004c1c80(cam, &vw);
  if (FUN_004c1a00(cam) != 0) {
    DAT_007d3ff8->RenderStateSet(6, 0);
    DAT_007d3ff8->RenderStateSet(8, 0);
    FUN_00402fb0();
    if (DAT_00771964 != 0) {
      FUN_00428760(DAT_00771964, 50.0f, 30.0f, 240.0f, 120.0f, 0);
    }
    DAT_007d3ff8->RenderStateSet(6, 1);
    DAT_007d3ff8->RenderStateSet(8, 1);
    FUN_004c19f0(cam);
  }
  FUN_004c1c80(cam, &save);
  FUN_004c1a00(cam);
}
