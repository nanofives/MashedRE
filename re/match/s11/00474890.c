// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00474890 (h9): by hand, 45.2% (regs 50.8%, ops 78.7%). Ghidra size stops before the 5-entry
   jump table at 0x00474d18. Draws a textured quad as two half-width RwIm2D tristrips (second half: x moved to
   the midpoint, u from u1/2 to u1). mode 1..5/default pick the u/v window from frame/2048.
   What moved the score: one temp per case so case 1/default and cases 2/3 are not tail-merged (the original
   merges only 4 into 5); v0 = 0.0625f before the frame product in cases 2/3.
   Residual: frame is 0x18 in the original (6 slots: v0 [0], u1 [4], x-mid [8], y-bottom [0xc] and [0x14],
   right x [0x10]) vs 0xc here: the original copies those floats to the vertex array through integer moves,
   which reads like an inlined helper's by-value params; __forceinline helper tried (42.3%), plain __inline is
   not inlined under /Ob1 at this size. The permuter reached 55.1% only with a swap that changes the second
   quad's x (rejected). Case-body assignment orders tried (6 variants, 39-43%). Class (b). */
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
typedef struct { float x, y, z, rhw; unsigned int emissiveColor; float u, v; } RwIm2DVertex;
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct { void *raster; } RwTexture;
typedef int (*RwRenderStateSetFn)(int state, void *value);
typedef int (*RwIm2DRenderPrimitiveFn)(int primType, RwIm2DVertex *verts, int numVerts);
typedef struct {
  void *curCamera; void *curWorld; unsigned short renderFrame, lightFrame; unsigned short pad[2];
  float gammaCorrection; void *fpSystem; float zBufferNear; float zBufferFar;
  RwRenderStateSetFn fpRenderStateSet; void *fpRenderStateGet; void *fpIm2DRenderLine;
  void *fpIm2DRenderTriangle; RwIm2DRenderPrimitiveFn fpIm2DRenderPrimitive;
} RwGlobals;
MGLOBAL(RwGlobals *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(RwIm2DVertex, DAT_00898a20, 4, 0x00898a20);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
// MATCH: 0x00474890 FUN_00474890
void FUN_00474890(RwTexture *tex, float x, float y, float w, float h, RwRGBA col, int frame, int mode,
                  int flag)
{
  float t, t1, t3, u0, u1, v0, v1, x1, y1;
  unsigned int c;
  int k;

  x = (float)FUN_0042b8b0() * x * 0.0015625f;
  y = (float)FUN_0042b8c0() * y * 0.0020833334f;
  w = (float)FUN_0042b8b0() * w * 0.0015625f;
  h = (float)FUN_0042b8c0() * h * 0.0020833334f;
  switch (mode) {
  case 1:
    t1 = 0.125f - (float)frame * 0.00048828125f;
    u0 = t1; v0 = u0; u1 = 1.0f - t1; v1 = u1;
    break;
  case 2:
    v0 = 0.0625f;
    t = (float)frame * 0.00048828125f;
    u0 = 0.125f - t; u1 = 1.0f - t; v1 = 0.9375f;
    break;
  case 3:
    v0 = 0.0625f;
    t3 = 0.125f - (float)frame * 0.00048828125f;
    u0 = 0.125f - t3; u1 = 1.0f - t3; v1 = 0.9375f;
    break;
  case 4:
    t = 0.125f - (float)frame * 0.00048828125f;
    u0 = 0.0625f; u1 = 0.9375f; v0 = 0.125f - t; v1 = 1.0f - t;
    break;
  case 5:
    t = (float)frame * 0.00048828125f;
    u0 = 0.0625f; u1 = 0.9375f; v0 = 0.125f - t; v1 = 1.0f - t;
    break;
  default:
    t = (float)frame * 0.00048828125f;
    u0 = t; v0 = u0; u1 = 1.0f - t; v1 = u1;
    break;
  }
  u1 = u1 * 0.5f;
  w = w * 0.5f;
  x1 = w + x;
  y1 = h + y;
  DAT_00898a20[0].x = x;  DAT_00898a20[0].y = y;
  DAT_00898a20[1].x = x1; DAT_00898a20[1].y = y;
  DAT_00898a20[2].x = x;  DAT_00898a20[2].y = y1;
  DAT_00898a20[3].x = x1; DAT_00898a20[3].y = y1;
  DAT_00898a20[0].u = u0; DAT_00898a20[0].v = v0;
  DAT_00898a20[1].u = u1; DAT_00898a20[1].v = v0;
  DAT_00898a20[2].u = u0; DAT_00898a20[2].v = v1;
  DAT_00898a20[3].u = u1; DAT_00898a20[3].v = v1;
  c = ((unsigned int)col.alpha << 24) | ((unsigned int)col.red << 16) | ((unsigned int)col.green << 8) | (unsigned int)col.blue;
  for (k = 0; k < 4; k++) {
    DAT_00898a20[k].emissiveColor = c;
    DAT_00898a20[k].z = DAT_007d3ff8->zBufferNear;
    DAT_00898a20[k].rhw = 1.0f;
  }
  if (flag) {
    DAT_00898a20[0].emissiveColor = ((unsigned int)col.red << 16) | ((unsigned int)col.green << 8) | (unsigned int)col.blue;
    DAT_00898a20[2].emissiveColor = ((unsigned int)col.red << 16) | ((unsigned int)col.green << 8) | (unsigned int)col.blue;
  }
  if (tex) {
    DAT_007d3ff8->fpRenderStateSet(1, tex->raster ? tex->raster : 0);
    DAT_007d3ff8->fpRenderStateSet(9, (void *)2);
  } else {
    DAT_007d3ff8->fpRenderStateSet(1, 0);
  }
  DAT_007d3ff8->fpRenderStateSet(0xc, (void *)1);
  DAT_007d3ff8->fpRenderStateSet(10, (void *)5);
  DAT_007d3ff8->fpRenderStateSet(0xb, (void *)6);
  DAT_007d3ff8->fpIm2DRenderPrimitive(4, DAT_00898a20, 4);
  x = x1;
  u0 = u1;
  x1 = w * 2.0f - w + x;
  u1 = u1 * 2.0f;
  DAT_00898a20[0].x = x;  DAT_00898a20[0].y = y;
  DAT_00898a20[1].x = x1; DAT_00898a20[1].y = y;
  DAT_00898a20[2].x = x;  DAT_00898a20[2].y = y1;
  DAT_00898a20[3].x = x1; DAT_00898a20[3].y = y1;
  DAT_00898a20[0].u = u0; DAT_00898a20[0].v = v0;
  DAT_00898a20[1].u = u1; DAT_00898a20[1].v = v0;
  DAT_00898a20[2].u = u0; DAT_00898a20[2].v = v1;
  DAT_00898a20[3].u = u1; DAT_00898a20[3].v = v1;
  for (k = 0; k < 4; k++) {
    DAT_00898a20[k].emissiveColor = c;
    DAT_00898a20[k].z = DAT_007d3ff8->zBufferNear;
    DAT_00898a20[k].rhw = 1.0f;
  }
  if (tex) {
    DAT_007d3ff8->fpRenderStateSet(1, tex->raster ? tex->raster : 0);
    DAT_007d3ff8->fpRenderStateSet(9, (void *)2);
  } else {
    DAT_007d3ff8->fpRenderStateSet(1, 0);
  }
  DAT_007d3ff8->fpRenderStateSet(0xc, (void *)1);
  DAT_007d3ff8->fpRenderStateSet(10, (void *)5);
  DAT_007d3ff8->fpRenderStateSet(0xb, (void *)6);
  DAT_007d3ff8->fpIm2DRenderPrimitive(4, DAT_00898a20, 4);
}
