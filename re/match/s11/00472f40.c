// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00472f40 (h7): 100% (whole symbol byte-identical). Same quad as 0x00472c60 (rect struct by value, /640 /480
   scaling, colour loop), then vertices 0 and 1 get colour 0xff000000 before the render-state calls.
   Residual: none. */
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
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA_00472f40;
typedef struct {
    float x, y, z;            /* 0x00 */
    float rhw;                /* 0x0c */
    unsigned int color;       /* 0x10 */
    float u, v;               /* 0x14 */
} Im2DVertex_00472f40;        /* 0x1c */
typedef struct {
    char pad0[0x18];
    float zBufferNear;                                  /* 0x18 */
    float zBufferFar;                                   /* 0x1c */
    int (*fpRenderStateSet)(int state, void *value);    /* 0x20 */
    int (*fpRenderStateGet)(int state, void *value);    /* 0x24 */
    int (*fpIm2DRenderLine)(void *v, int n, int a, int b); /* 0x28 */
    int (*fpIm2DRenderTriangle)(void *v, int n, int a, int b, int c); /* 0x2c */
    int (*fpIm2DRenderPrimitive)(int type, void *v, int n); /* 0x30 */
} RwGlobals_00472f40;
typedef struct { void *raster; } RwTexture_00472f40;
MGLOBAL(RwGlobals_00472f40 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(Im2DVertex_00472f40, DAT_00898a20, 4, 0x00898a20);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
typedef struct { float x, y, w, h; } Rect_00472f40;
/* port (MATCH-S11 swap): `* (1.0f / 640.0f)` / `* (1.0f / 480.0f)` is the multiply by 0x5cd5a8 / 0x5cc560 that 9176
   emits for `/ 640.0f` / `/ 480.0f` (bytes unchanged); VS2022 /fp:precise keeps a real divide, which rounds differently. */
// MATCH: 0x00472f40 FUN_00472f40
void FUN_00472f40(Rect_00472f40 r, RwRGBA_00472f40 param_5)
{
  int i;
  r.x = (float)FUN_0042b8b0() * r.x * (1.0f / 640.0f);
  r.y = (float)FUN_0042b8c0() * r.y * (1.0f / 480.0f);
  r.w = (float)FUN_0042b8b0() * r.w * (1.0f / 640.0f);
  r.h = (float)FUN_0042b8c0() * r.h * (1.0f / 480.0f);
  DAT_00898a20[0].x = r.x;
  DAT_00898a20[0].y = r.y;
  DAT_00898a20[1].x = r.x + r.w;
  DAT_00898a20[1].y = r.y;
  DAT_00898a20[2].x = r.x;
  DAT_00898a20[2].y = r.y + r.h;
  DAT_00898a20[3].x = r.x + r.w;
  DAT_00898a20[3].y = r.y + r.h;
  for (i = 0; i < 4; i++) {
    DAT_00898a20[i].color = ((unsigned int)param_5.alpha << 24) | ((unsigned int)param_5.red << 16) |
                            ((unsigned int)param_5.green << 8) | (unsigned int)param_5.blue;
    DAT_00898a20[i].z = DAT_007d3ff8->zBufferNear;
    DAT_00898a20[i].rhw = 1.0f;
  }
  DAT_00898a20[0].color = 0xff000000;
  DAT_00898a20[1].color = 0xff000000;
  DAT_007d3ff8->fpRenderStateSet(1, 0);
  DAT_007d3ff8->fpRenderStateSet(0xc, (void *)1);
  DAT_007d3ff8->fpRenderStateSet(10, (void *)5);
  DAT_007d3ff8->fpRenderStateSet(0xb, (void *)6);
  DAT_007d3ff8->fpIm2DRenderPrimitive(4, DAT_00898a20, 4);
}
