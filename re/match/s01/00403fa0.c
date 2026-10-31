// FLAGS: /O2 /GS /Ob1
/* a3 MATCH-S01: 0x00403fa0 (529 B) 100% (g2s 32.7%) and its only caller 0x00404320 (802 B) BYTES-OK
   (98.3% exact: our epilogue pop edi/pop esi/add esp/ret runs past Ghidra's size; class (c) boundary).
   - 0x00403fa0 takes its Entry* in EAX: `static`, defined before 0x00404320 in this file, /Ob1; the int index
     stays on the stack. Entry = 0x14-byte record (float at +0x10), array DAT_00636b88[6] (ends 0x00636c00).
   - Its y values are the second float of two 8-byte locals (RwV2d-shaped); with plain floats the frame is 0x10
     not 0x18. The shadow colour lands in the dead slot of the stack parameter (compiler stack packing).
   - Statement order pos.y=80, colA, colB, shadow, then t / e->t picks cl=0xd8, dl=0xc8, bl=0 (brute force of
     the 120 orders of those 5 statements: 3 hit 100%, this is the natural one).
   - Every _DAT_005cc3xx/5cc7xx here is an .rdata float literal (0.625, 0.5, 1.0, 110, 80, 3, 30, 60).
   - 0x00404320 is an if/else-if chain on DAT_007f0fd0 (9, 8, 5, 10), not a switch (switch = jump table). The
     loop end test is a signed pointer compare: `(int)e < (int)(DAT_00636b88 + 6)` gives jl (plain e < end: jb).
   - Callees 0x00403c60/0x00403d30/0x00403db0/0x00403ed0/0x004041c0 extern here (403ed0/4041c0 are matched in
     their own files). DAT_007d3ff8 = RwGlobals*, slot [8] = RenderStateSet. */
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct { int a, b, c, d; float t; } Entry;
typedef struct { float x, y; } RwV2d;
typedef void (*RenderStateSetFn)(int, int);
extern float DAT_007f100c;
extern int DAT_005ea0b0;
extern int DAT_005ea0b4;
extern int DAT_007f0fd0;
extern int *DAT_007d3ff8;
extern Entry DAT_00636b88[6];
void FUN_00427e00(int p0, float p1, float p2, RwRGBA p3, float p4, int p5);
void FUN_00403c60(void);
void FUN_00403d30(void);
void FUN_00403db0(void);
void FUN_00403ed0(void);
void FUN_004041c0(void);
int FUN_0040e350(void);
void FUN_004c19f0(int p0);
int FUN_004c1a00(int p0);
int FUN_004c1c80(int p0, float *p1);
// MATCH: 0x00403fa0 FUN_00403fa0
static void FUN_00403fa0(Entry *e, int i)
{
  float t;
  RwV2d pos;
  RwV2d psh;
  RwRGBA colA;
  RwRGBA colB;
  RwRGBA shadow;

  pos.y = 80.0f;
  colA.red = 5; colA.green = 0xd8; colA.blue = 0; colA.alpha = 200;
  colB.red = 0xd8; colB.green = 0xd8; colB.blue = 0; colB.alpha = 200;
  shadow.red = 0; shadow.green = 0; shadow.blue = 0; shadow.alpha = 0xff;
  t = e->t * 0.625f;
  e->t = e->t - DAT_007f100c;
  if (t < 0.5f) {
    t = 1.0f - t;
    pos.y = (1.0f - (t + t)) * 110.0f + 80.0f;
  }
  psh.y = pos.y + 3.0f;
  FUN_00427e00(0x226, 323.0f, psh.y, shadow, 1.2f, 2);
  FUN_00427e00(0x226, 320.0f, pos.y, colA, 1.2f, 2);
  FUN_00427e00(0x227, 323.0f, psh.y + 30.0f, shadow, 0.9f, 2);
  FUN_00427e00(0x227, 320.0f, pos.y + 30.0f, colA, 0.9f, 2);
  if (i == 0 && (float)DAT_005ea0b0 == 1.0f) {
    FUN_00427e00(0x228, 323.0f, psh.y + 60.0f, shadow, 0.9f, 2);
    FUN_00427e00(0x228, 320.0f, pos.y + 60.0f, colB, 0.9f, 2);
  }
  if (i == DAT_005ea0b4 - 1 && (float)DAT_005ea0b0 == 1.0f) {
    FUN_00427e00(0x229, 323.0f, psh.y + 60.0f, shadow, 0.9f, 2);
    FUN_00427e00(0x229, 320.0f, pos.y + 60.0f, colB, 0.9f, 2);
  }
}
// MATCH: 0x00404320 FUN_00404320
void FUN_00404320(void)
{
  int cam;
  int i;
  Entry *e;
  float vw[2];
  float saved[2];

  i = 0;
  if (DAT_007f0fd0 == 9) {
    cam = *DAT_007d3ff8;
    FUN_004c19f0(cam);
    saved[0] = *(float *)(cam + 0x68);
    saved[1] = *(float *)(cam + 0x6c);
    vw[0] = 0.8f;
    vw[1] = 0.8f;
    FUN_004c1c80(cam, vw);
    if (FUN_004c1a00(cam) == 0) return;
    ((RenderStateSetFn)DAT_007d3ff8[8])(6, 0);
    ((RenderStateSetFn)DAT_007d3ff8[8])(8, 0);
    if (FUN_0040e350() == 5 || FUN_0040e350() == 4 || FUN_0040e350() == 3) {
      FUN_00403d30();
    } else if (FUN_0040e350() == 6) {
      FUN_004041c0();
    }
    ((RenderStateSetFn)DAT_007d3ff8[8])(6, 1);
    ((RenderStateSetFn)DAT_007d3ff8[8])(8, 1);
    FUN_004c19f0(cam);
    FUN_004c1c80(cam, saved);
    FUN_004c1a00(cam);
  } else if (DAT_007f0fd0 == 8) {
    cam = *DAT_007d3ff8;
    FUN_004c19f0(cam);
    saved[0] = *(float *)(cam + 0x68);
    saved[1] = *(float *)(cam + 0x6c);
    vw[0] = 0.8f;
    vw[1] = 0.8f;
    FUN_004c1c80(cam, vw);
    if (FUN_004c1a00(cam) == 0) return;
    ((RenderStateSetFn)DAT_007d3ff8[8])(6, 0);
    ((RenderStateSetFn)DAT_007d3ff8[8])(8, 0);
    if (FUN_0040e350() == 5 || FUN_0040e350() == 4 || FUN_0040e350() == 3) {
      FUN_00403db0();
    } else if (FUN_0040e350() == 6) {
      FUN_004041c0();
    }
    ((RenderStateSetFn)DAT_007d3ff8[8])(6, 1);
    ((RenderStateSetFn)DAT_007d3ff8[8])(8, 1);
    FUN_004c19f0(cam);
    FUN_004c1c80(cam, saved);
    FUN_004c1a00(cam);
  } else if (DAT_007f0fd0 == 5) {
    cam = *DAT_007d3ff8;
    FUN_004c19f0(cam);
    saved[0] = *(float *)(cam + 0x68);
    saved[1] = *(float *)(cam + 0x6c);
    vw[0] = 0.8f;
    vw[1] = 0.8f;
    FUN_004c1c80(cam, vw);
    if (FUN_004c1a00(cam) == 0) return;
    ((RenderStateSetFn)DAT_007d3ff8[8])(6, 0);
    ((RenderStateSetFn)DAT_007d3ff8[8])(8, 0);
    if (FUN_0040e350() == 5 || FUN_0040e350() == 4 || FUN_0040e350() == 3) {
      FUN_00403ed0();
    }
    ((RenderStateSetFn)DAT_007d3ff8[8])(6, 1);
    ((RenderStateSetFn)DAT_007d3ff8[8])(8, 1);
    FUN_004c19f0(cam);
    FUN_004c1c80(cam, saved);
    FUN_004c1a00(cam);
  } else if (DAT_007f0fd0 == 10) {
    cam = *DAT_007d3ff8;
    FUN_004c19f0(cam);
    saved[0] = *(float *)(cam + 0x68);
    saved[1] = *(float *)(cam + 0x6c);
    vw[0] = 0.8f;
    vw[1] = 0.8f;
    FUN_004c1c80(cam, vw);
    if (FUN_004c1a00(cam) == 0) return;
    ((RenderStateSetFn)DAT_007d3ff8[8])(6, 0);
    ((RenderStateSetFn)DAT_007d3ff8[8])(8, 0);
    if (FUN_0040e350() == 5 || FUN_0040e350() == 4 || FUN_0040e350() == 3) {
      FUN_00403c60();
    } else if (FUN_0040e350() == 6) {
      for (e = DAT_00636b88; (int)e < (int)(DAT_00636b88 + 6); e++, i++) {
        if (e->t > 0.0f) FUN_00403fa0(e, i);
      }
    }
    ((RenderStateSetFn)DAT_007d3ff8[8])(6, 1);
    ((RenderStateSetFn)DAT_007d3ff8[8])(8, 1);
    FUN_004c19f0(cam);
    FUN_004c1c80(cam, saved);
    FUN_004c1a00(cam);
  }
}
