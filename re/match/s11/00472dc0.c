// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00472dc0 (h7): 100% (whole symbol byte-identical). Screen-space Im2D triangle: each coordinate scaled
   by the screen size from FUN_0042b8b0 / FUN_0042b8c0 and divided by 640 / 480 (the compiler turns the division
   into the multiply by 0x5cd5a8 / 0x5cc560; writing the multiply through MGLOBAL put the constant first), RwRGBA
   by value packed to ARGB, RwEngineInstance (0x7d3ff8) render-state calls and Im2DRenderPrimitive(4, v, 3).
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
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA_00472dc0;
typedef struct {
    float x, y, z;            /* 0x00 */
    float rhw;                /* 0x0c */
    unsigned int color;       /* 0x10 */
    float u, v;               /* 0x14 */
} Im2DVertex_00472dc0;        /* 0x1c */
typedef struct {
    char pad0[0x18];
    float zBufferNear;                                  /* 0x18 */
    float zBufferFar;                                   /* 0x1c */
    int (*fpRenderStateSet)(int state, void *value);    /* 0x20 */
    int (*fpRenderStateGet)(int state, void *value);    /* 0x24 */
    int (*fpIm2DRenderLine)(void *v, int n, int a, int b); /* 0x28 */
    int (*fpIm2DRenderTriangle)(void *v, int n, int a, int b, int c); /* 0x2c */
    int (*fpIm2DRenderPrimitive)(int type, void *v, int n); /* 0x30 */
} RwGlobals_00472dc0;
typedef struct { void *raster; } RwTexture_00472dc0;
MGLOBAL(RwGlobals_00472dc0 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(Im2DVertex_00472dc0, DAT_00898a20, 4, 0x00898a20);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
/* port (MATCH-S11 swap): `* (1.0f / 640.0f)` / `* (1.0f / 480.0f)` is the multiply by 0x5cd5a8 / 0x5cc560 that 9176
   emits for `/ 640.0f` / `/ 480.0f` (bytes unchanged); VS2022 /fp:precise keeps a real divide, which rounds differently. */
// MATCH: 0x00472dc0 FUN_00472dc0
void FUN_00472dc0(float param_1, float param_2, float param_3, float param_4, float param_5, float param_6,
                  RwRGBA_00472dc0 param_7)
{
  int i;
  param_1 = (float)FUN_0042b8b0() * param_1 * (1.0f / 640.0f);
  param_2 = (float)FUN_0042b8c0() * param_2 * (1.0f / 480.0f);
  param_3 = (float)FUN_0042b8b0() * param_3 * (1.0f / 640.0f);
  param_4 = (float)FUN_0042b8c0() * param_4 * (1.0f / 480.0f);
  param_5 = (float)FUN_0042b8b0() * param_5 * (1.0f / 640.0f);
  param_6 = (float)FUN_0042b8c0() * param_6 * (1.0f / 480.0f);
  DAT_00898a20[0].x = param_1;
  DAT_00898a20[0].y = param_2;
  DAT_00898a20[1].x = param_3;
  DAT_00898a20[1].y = param_4;
  DAT_00898a20[2].x = param_5;
  DAT_00898a20[2].y = param_6;
  for (i = 0; i < 3; i++) {
    DAT_00898a20[i].color = ((unsigned int)param_7.alpha << 24) | ((unsigned int)param_7.red << 16) |
                            ((unsigned int)param_7.green << 8) | (unsigned int)param_7.blue;
    DAT_00898a20[i].z = DAT_007d3ff8->zBufferNear;
    DAT_00898a20[i].rhw = 1.0f;
  }
  DAT_007d3ff8->fpRenderStateSet(1, 0);
  DAT_007d3ff8->fpRenderStateSet(0xc, (void *)1);
  DAT_007d3ff8->fpRenderStateSet(10, (void *)5);
  DAT_007d3ff8->fpRenderStateSet(0xb, (void *)6);
  DAT_007d3ff8->fpIm2DRenderPrimitive(4, DAT_00898a20, 3);
}
