// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x004430e0: 100% by hand (h7). Raw 47.6%. Color is an RwRGBA passed by value, packed a,r,g,b; loop body
   order color, z, rhw. Three RwIm2DVertex records at 0x00828320 (7 dwords each),
   RwGlobals (DAT_007d3ff8) device slots +0x18 zBufferNear, +0x20 RenderStateSet, +0x34 Im2DRenderIndexedPrimitive. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct {
  float x;
  float y;
  float z;
  float rhw;
  uint color;
  float u;
  float v;
} Im2DVertex_004430e0;
typedef struct {
  uchar r;
  uchar g;
  uchar b;
  uchar a;
} RGBA_004430e0;
typedef struct {
  void *curCamera;
  void *curWorld;
  ushort renderFrame;
  ushort lightFrame;
  ushort pad[2];
  float gammaCorrection;
  void *fpSystem;
  float zBufferNear;
  float zBufferFar;
  int (*fpRenderStateSet)(int, int);
  int (*fpRenderStateGet)(int, void *);
  void *fpIm2DRenderLine;
  void *fpIm2DRenderTriangle;
  void *fpIm2DRenderPrimitive;
  int (*fpIm2DRenderIndexedPrimitive)(int, Im2DVertex_004430e0 *, int, ushort *, int);
} RwGlobals_004430e0;
MGLOBAL(RwGlobals_004430e0 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(Im2DVertex_004430e0, DAT_00828320, 3, 0x00828320);
// MATCH: 0x004430e0 FUN_004430e0
void FUN_004430e0(float x0, float y0, float x1, float y1, float x2, float y2, RGBA_004430e0 col)
{
  ushort idx[3];
  int i;

  DAT_00828320[0].x = x0;
  DAT_00828320[0].y = y0;
  DAT_00828320[1].x = x1;
  DAT_00828320[1].y = y1;
  DAT_00828320[2].x = x2;
  DAT_00828320[2].y = y2;
  idx[0] = 0;
  idx[1] = 1;
  idx[2] = 2;
  for (i = 0; i < 3; i++) {
    DAT_00828320[i].color = ((uint)col.a << 24) | ((uint)col.r << 16) | ((uint)col.g << 8) | (uint)col.b;
    DAT_00828320[i].z = DAT_007d3ff8->zBufferNear;
    DAT_00828320[i].rhw = 1.0f;
  }
  DAT_007d3ff8->fpRenderStateSet(1, 0);
  DAT_007d3ff8->fpRenderStateSet(7, 2);
  DAT_007d3ff8->fpRenderStateSet(6, 0);
  DAT_007d3ff8->fpRenderStateSet(8, 0);
  DAT_007d3ff8->fpIm2DRenderIndexedPrimitive(3, DAT_00828320, 3, idx, 3);
  DAT_007d3ff8->fpRenderStateSet(6, 1);
  DAT_007d3ff8->fpRenderStateSet(8, 1);
}
