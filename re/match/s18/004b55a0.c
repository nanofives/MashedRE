// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b55a0 (h4): 83.7%, regs 92.7%, ops 95.9% (g2s 28.5%). By hand: RwIm2D line draw (2 RwIm2DVertex of 0x1c B, fpIm2DRenderLine at RwGlobals+0x28 called (v, 2, 0, 1), fpRenderStateSet at +0x20, zBufferNear at +0x18), defaults for NULL position/size/colour, raster offset from FUN_004c75e0 into two shorts (the compiler packs them into the dead param_2/param_3 slots). What moved the score: the right render call (v, 2, 0, 1) and two separate float[2] default arrays plus float[2] wh/off arrays (frame 0x5c), nearp local before the 1.0f / nearp divide (fdivr). Residual: the original loads RwGlobals into ECX after storing the y offset (and builds the colour in EAX); 9176 loads it into EAX right after the x store, which swaps ECX/EAX through the colour block. Tried: typed RwGlobals struct, offset conversion forms and order, recip placement, /G5 (76.0%), permuter 552 compiles (82.9% before the nearp change). Class (b). */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct { float x, y, z, rhw; unsigned int color; float u, v; } V2D_004b55a0;
typedef struct { char pad[0xc]; int width; int height; } Raster_004b55a0;
typedef struct { char pad[0x60]; Raster_004b55a0 *frameBuffer; char pad2[0x1c]; float nearPlane; } Camera_004b55a0;
typedef int (*RenderStateSetFn)(int, int);
typedef int (*Im2DRenderLineFn)(V2D_004b55a0 *, int, int, int);
typedef struct {
  char pad0[0x18];
  float zBufferNear;
  float zBufferFar;
  RenderStateSetFn fpRenderStateSet;
  void *fpRenderStateGet;
  Im2DRenderLineFn fpIm2DRenderLine;
} RwGlobals_004b55a0;
MGLOBAL(RwGlobals_004b55a0 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
void FUN_004c75e0(Raster_004b55a0 *p0, short *p1, short *p2);

// MATCH: 0x004b55a0 FUN_004b55a0
void FUN_004b55a0(Camera_004b55a0 *param_1, float *param_2, float *param_3, unsigned char *param_4)
{
  float defpos[2];
  float defsize[2];
  unsigned char defcol[4];
  float wh[2];
  short ox, oy;
  float off[2];
  float recip;
  float nearp;
  unsigned int col;
  Raster_004b55a0 *r;
  V2D_004b55a0 v[2];

  defpos[0] = 0.0f;
  defpos[1] = 0.0f;
  defsize[0] = 1.0f;
  defsize[1] = 1.0f;
  defcol[0] = 0xff;
  defcol[1] = 0xff;
  defcol[2] = 0xff;
  defcol[3] = 0xff;
  if (param_2 == 0) param_2 = defpos;
  if (param_3 == 0) param_3 = defsize;
  if (param_4 == 0) param_4 = defcol;
  r = param_1->frameBuffer;
  wh[0] = (float)r->width;
  wh[1] = (float)r->height;
  FUN_004c75e0(r, &ox, &oy);
  off[0] = (float)ox;
  off[1] = (float)oy;
  nearp = param_1->nearPlane;
  recip = 1.0f / nearp;
  v[0].x = wh[0] * param_2[0] + off[0];
  v[0].y = wh[1] * param_2[1] + off[1];
  v[0].z = DAT_007d3ff8->zBufferNear;
  v[0].rhw = recip;
  v[1].x = wh[0] * param_3[0] + off[0];
  v[1].y = wh[1] * param_3[1] + off[1];
  v[1].z = DAT_007d3ff8->zBufferNear;
  v[1].rhw = recip;
  col = (((param_4[3] << 8) | param_4[0]) << 8 | param_4[1]) << 8 | param_4[2];
  v[0].color = col;
  v[1].color = col;
  DAT_007d3ff8->fpRenderStateSet(7, 1);
  DAT_007d3ff8->fpRenderStateSet(1, 0);
  if (param_4[3] != 0xff) {
    DAT_007d3ff8->fpRenderStateSet(0xc, 1);
  }
  DAT_007d3ff8->fpIm2DRenderLine(v, 2, 0, 1);
  if (param_4[3] != 0xff) {
    DAT_007d3ff8->fpRenderStateSet(0xc, 0);
  }
  DAT_007d3ff8->fpRenderStateSet(7, 2);
}
