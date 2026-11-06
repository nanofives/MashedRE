// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00472b10 (h7): 100% (whole symbol byte-identical). Textured screen-space Im2D quad into the 4-vertex
   buffer at 0x898a20 (0x1c-byte RwIm2DVertex): v0 gets u/v then z/rhw, v1..v3 x/y/u/v, then a loop sets
   colour/z/rhw on all four; RwRGBA by value packed to ARGB; render states through RwEngineInstance (0x7d3ff8).
   The x/y/w/h floats are one rect struct passed by value (four float params gave x+w loaded x first, 95.2%,
   permuter no gain). Residual: none. */
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
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA_00472b10;
typedef struct {
    float x, y, z;            /* 0x00 */
    float rhw;                /* 0x0c */
    unsigned int color;       /* 0x10 */
    float u, v;               /* 0x14 */
} Im2DVertex_00472b10;        /* 0x1c */
typedef struct {
    char pad0[0x18];
    float zBufferNear;                                  /* 0x18 */
    float zBufferFar;                                   /* 0x1c */
    int (*fpRenderStateSet)(int state, void *value);    /* 0x20 */
    int (*fpRenderStateGet)(int state, void *value);    /* 0x24 */
    int (*fpIm2DRenderLine)(void *v, int n, int a, int b); /* 0x28 */
    int (*fpIm2DRenderTriangle)(void *v, int n, int a, int b, int c); /* 0x2c */
    int (*fpIm2DRenderPrimitive)(int type, void *v, int n); /* 0x30 */
} RwGlobals_00472b10;
typedef struct { void *raster; } RwTexture_00472b10;
MGLOBAL(RwGlobals_00472b10 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(Im2DVertex_00472b10, DAT_00898a20, 4, 0x00898a20);
typedef struct { float x, y, w, h; } Rect_00472b10;
// MATCH: 0x00472b10 FUN_00472b10
void FUN_00472b10(RwTexture_00472b10 *param_1, Rect_00472b10 r, RwRGBA_00472b10 param_6)
{
  int i;
  DAT_00898a20[0].x = r.x;
  DAT_00898a20[0].y = r.y;
  DAT_00898a20[0].u = 0.0f;
  DAT_00898a20[0].v = 0.0f;
  DAT_00898a20[0].z = DAT_007d3ff8->zBufferNear;
  DAT_00898a20[0].rhw = 1.0f;
  DAT_00898a20[1].x = r.x + r.w;
  DAT_00898a20[1].y = r.y;
  DAT_00898a20[1].u = 1.0f;
  DAT_00898a20[1].v = 0.0f;
  DAT_00898a20[2].x = r.x;
  DAT_00898a20[2].y = r.y + r.h;
  DAT_00898a20[2].u = 0.0f;
  DAT_00898a20[2].v = 1.0f;
  DAT_00898a20[3].x = r.x + r.w;
  DAT_00898a20[3].y = r.y + r.h;
  DAT_00898a20[3].u = 1.0f;
  DAT_00898a20[3].v = 1.0f;
  for (i = 0; i < 4; i++) {
    DAT_00898a20[i].color = ((unsigned int)param_6.alpha << 24) | ((unsigned int)param_6.red << 16) |
                            ((unsigned int)param_6.green << 8) | (unsigned int)param_6.blue;
    DAT_00898a20[i].z = DAT_007d3ff8->zBufferNear;
    DAT_00898a20[i].rhw = 1.0f;
  }
  if (param_1 != 0) {
    DAT_007d3ff8->fpRenderStateSet(1, param_1->raster);
    DAT_007d3ff8->fpRenderStateSet(9, (void *)2);
  }
  else {
    DAT_007d3ff8->fpRenderStateSet(1, 0);
  }
  DAT_007d3ff8->fpRenderStateSet(0xc, (void *)1);
  DAT_007d3ff8->fpIm2DRenderPrimitive(4, DAT_00898a20, 4);
}
