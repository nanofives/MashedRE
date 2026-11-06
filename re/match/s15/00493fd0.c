// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00493fd0 (h8): best 80.1% (regs 94.4%, ops 100%). Screen-space textured quad: four 28-byte Im2D
   vertices (x, y, z, rhw, color, u, v) from the camera's frame-buffer raster size (+0x60 -> +0xc/+0x10), the
   FUN_004c75e0 raster offset (2 short outs), rhw = 1/camera+0x80, RwGlobals (DAT_007d3ff8) render-state set
   (+0x20) and triangle (+0x2c) calls. Instruction stream is identical in opcode; size/offset as V2d structs
   brought the frame to 0xa0 (separate float vars: 49%), `r = near; r = 1.0f / r` gives the fdivr (direct form
   emits fld 1.0; fdiv). Residual (b): (1) the two short outs and the int conversion temp sit at +0xc/+8/+4 in
   ours versus +4/+8/+0xc in the original (declaration order, explicit int temp, short array, int outs tried);
   (2) RwGlobals is held in eax and the colour in ecx where the original uses ecx/eax (also makes `mov eax,[g]`
   1 byte shorter, so the two later je displacements differ). Tried colour expression forms and position,
   store orders, a local RwGlobals pointer, permuter (no gain). */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct { float x, y, z, rhw; unsigned int color; float u, v; } Im2DVertex;
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct { float x, y; } V2d;
typedef int (*RenderStateSetFn)(int state, int value);
typedef int (*Im2DRenderTriangleFn)(Im2DVertex *verts, int n, int v1, int v2, int v3);
typedef struct {
  char pad0[0x18];
  float zBufferNear;                          /* +0x18 */
  float zBufferFar;                           /* +0x1c */
  RenderStateSetFn fpRenderStateSet;          /* +0x20 */
  void *fpRenderStateGet;                     /* +0x24 */
  void *fpIm2DRenderLine;                     /* +0x28 */
  Im2DRenderTriangleFn fpIm2DRenderTriangle;  /* +0x2c */
} RwGlobals_493fd0;
typedef struct { char pad0[0xc]; int width; int height; } Raster_493fd0;
typedef struct { char pad0[0x60]; Raster_493fd0 *frameBuffer; char pad64[0x1c]; float nearPlane; } Camera_493fd0;
MGLOBAL(RwGlobals_493fd0 *, DAT_007d3ff8, 0x007d3ff8);
void FUN_004c75e0(Raster_493fd0 *raster, short *x, short *y);
// MATCH: 0x00493fd0 FUN_00493fd0
void FUN_00493fd0(Camera_493fd0 *camera, V2d *tl, V2d *br, RGBA *col, int texture, float u1, float v1)
{
  RGBA white;
  short oy;
  short ox;
  V2d size;
  V2d off;
  V2d def[2];
  Im2DVertex v[4];
  Raster_493fd0 *raster;
  float recipZ;
  unsigned int c;

  def[0].x = 0.0f;
  def[0].y = 0.0f;
  def[1].x = 1.0f;
  def[1].y = 1.0f;
  white.r = 0xff;
  white.g = 0xff;
  white.b = 0xff;
  white.a = 0xff;
  if (tl == 0) {
    tl = &def[0];
  }
  if (br == 0) {
    br = &def[1];
  }
  if (col == 0) {
    col = &white;
  }
  raster = camera->frameBuffer;
  size.x = (float)raster->width;
  size.y = (float)raster->height;
  FUN_004c75e0(raster, &oy, &ox);
  off.x = (float)ox;
  off.y = (float)oy;
  recipZ = camera->nearPlane;
  recipZ = 1.0f / recipZ;

  v[0].x = size.x * tl->x + off.x;
  v[0].y = size.y * tl->y + off.y;
  v[0].z = DAT_007d3ff8->zBufferNear;
  v[0].rhw = recipZ;
  v[0].u = 0.0f;
  v[0].v = v1;

  v[1].x = v[0].x;
  v[1].y = size.y * br->y + off.y;
  v[1].z = DAT_007d3ff8->zBufferNear;
  v[1].rhw = recipZ;
  v[1].u = 0.0f;
  v[1].v = 0.0f;

  v[2].x = size.x * br->x + off.x;
  v[2].y = v[1].y;
  v[2].z = DAT_007d3ff8->zBufferNear;
  v[2].rhw = recipZ;
  v[2].u = u1;
  v[2].v = 0.0f;

  v[3].x = v[2].x;
  v[3].y = v[0].y;
  v[3].z = DAT_007d3ff8->zBufferNear;
  v[3].rhw = recipZ;
  v[3].u = u1;
  v[3].v = v1;

  c = ((unsigned int)col->a << 24) | ((unsigned int)col->r << 16) | ((unsigned int)col->g << 8) | col->b;
  v[0].color = c;
  v[1].color = c;
  v[2].color = c;
  v[3].color = c;

  DAT_007d3ff8->fpRenderStateSet(7, 1);
  DAT_007d3ff8->fpRenderStateSet(1, texture);
  if (col->a != 0xff) {
    DAT_007d3ff8->fpRenderStateSet(0xc, 1);
  }
  DAT_007d3ff8->fpIm2DRenderTriangle(v, 4, 0, 1, 2);
  DAT_007d3ff8->fpIm2DRenderTriangle(v, 4, 2, 3, 0);
  if (col->a != 0xff) {
    DAT_007d3ff8->fpRenderStateSet(0xc, 0);
  }
  DAT_007d3ff8->fpRenderStateSet(7, 2);
}
