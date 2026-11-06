// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004744a0 (h9): by hand, 99.6% (regs 99.6%, ops 99.6%). 2 passes x 3 rows x 7 columns of
   sine-displaced 21x21 quads drawn with RwIm2DRenderPrimitive(4 = tristrip) through RwEngineInstance
   (DAT_007d3ff8: zBufferNear +0x18, fpRenderStateSet +0x20, fpIm2DRenderPrimitive +0x30).
   What moved the score: the four corners as separate RwV2d locals (scalars 50%, array 63.6%), the cell origin
   as one RwV2d o (its unused .y slot is the 4 extra frame bytes: 63.9 -> 96.7%), RwGlobals.pad as
   RwUInt16[2], `c1.red = c1.green = c1.blue = 128` (store order blue, green, red).
   Residual (1 slot): the last `fstp [esp+0x38]` (p3.y) is scheduled after `test eax,eax` in ours. Tried:
   !pass / pass==0, merged override assignments, 2.0f / 2.0 / uncast products, w2 = wave*2.0f, permuter
   (326 compiles, no gain). Class (b). */
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
typedef int (*RwRenderStateSetFn)(int state, int value);
typedef int (*RwIm2DRenderPrimitiveFn)(int primType, RwIm2DVertex *verts, int numVerts);
typedef struct {
  void *curCamera; void *curWorld; unsigned short renderFrame, lightFrame; unsigned short pad[2];
  float gammaCorrection; void *fpSystem; float zBufferNear; float zBufferFar;
  RwRenderStateSetFn fpRenderStateSet; void *fpRenderStateGet; void *fpIm2DRenderLine;
  void *fpIm2DRenderTriangle; RwIm2DRenderPrimitiveFn fpIm2DRenderPrimitive;
} RwGlobals;
MGLOBAL(RwGlobals *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(RwIm2DVertex, DAT_00898a20, 4, 0x00898a20);
MGLOBAL(float, DAT_007f1010, 0x007f1010);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
double sin(double);
#pragma intrinsic(sin)
typedef struct { float x, y; } RwV2d;
// MATCH: 0x004744a0 FUN_004744a0
void FUN_004744a0(void)
{
  RwRGBA c1, c0, col;
  int pass, i, j, k;
  RwV2d o; float w2;
  RwV2d p0, p1, p2, p3;

  c1.red = c1.green = c1.blue = 128;
  c0.red = 0; c0.green = 0; c0.blue = 0; c0.alpha = 0;
  c1.alpha = 255;
  for (pass = 0; pass < 2; pass++) {
    for (i = 0; i < 3; i++) {
      for (j = 0; j < 7; j++) {
        o.x = (float)((i & 1) * 21 + (j * 5 + 45) * 8);
        o.y = (float)(i * 21);
        if (pass) o.y += 416.0f;
        p0.x = o.x; p0.y = o.y;
        p1.x = o.x + 21.0f; p1.y = o.y;
        p2.x = o.x; p2.y = o.y + 21.0f;
        p3.x = o.x + 21.0f; p3.y = o.y + 21.0f;
        w2 = DAT_007f1010 + DAT_007f1010;
        p0.x += (float)(sin((float)(i + j) + w2) * 4.0);
        p1.x += (float)(sin((float)(i + j + 1) + w2) * 4.0);
        p2.x += (float)(sin((float)(i + j + 1) + w2) * 4.0);
        p3.x += (float)(sin((float)(i + j + 2) + w2) * 4.0);
        p0.y += (float)(sin((float)(i * 2 + j) + w2) * 2.0);
        p1.y += (float)(sin((float)(i * 2 + j + 1) + w2) * 2.0);
        p2.y += (float)(sin((float)(i * 2 + j + 2) + w2) * 2.0);
        p3.y += (float)(sin((float)(i * 2 + j + 3) + w2) * 2.0);
        if (pass == 0) {
          if (i == 2) { p3.y = 64.0f; p2.y = 64.0f; }
        } else if (i == 0) {
          p1.y = 416.0f; p0.y = 416.0f;
        }
        col = c1;
        if (j == 0 && (i & 1) == 0) col = c0;
        p0.x = (float)FUN_0042b8b0() * p0.x * 0.0015625f;
        p0.y = (float)FUN_0042b8c0() * p0.y * 0.0020833334f;
        p1.x = (float)FUN_0042b8b0() * p1.x * 0.0015625f;
        p1.y = (float)FUN_0042b8c0() * p1.y * 0.0020833334f;
        p2.x = (float)FUN_0042b8b0() * p2.x * 0.0015625f;
        p2.y = (float)FUN_0042b8c0() * p2.y * 0.0020833334f;
        p3.x = (float)FUN_0042b8b0() * p3.x * 0.0015625f;
        p3.y = (float)FUN_0042b8c0() * p3.y * 0.0020833334f;
        DAT_00898a20[0].x = p0.x; DAT_00898a20[0].y = p0.y;
        DAT_00898a20[1].x = p1.x; DAT_00898a20[1].y = p1.y;
        DAT_00898a20[2].x = p2.x; DAT_00898a20[2].y = p2.y;
        DAT_00898a20[3].x = p3.x; DAT_00898a20[3].y = p3.y;
        DAT_00898a20[0].emissiveColor = ((unsigned int)col.alpha << 24) | ((unsigned int)col.red << 16) | ((unsigned int)col.green << 8) | (unsigned int)col.blue;
        DAT_00898a20[1].emissiveColor = 0xff808080;
        DAT_00898a20[2].emissiveColor = ((unsigned int)col.alpha << 24) | ((unsigned int)col.red << 16) | ((unsigned int)col.green << 8) | (unsigned int)col.blue;
        DAT_00898a20[3].emissiveColor = 0xff808080;
        for (k = 0; k < 4; k++) {
          DAT_00898a20[k].z = DAT_007d3ff8->zBufferNear;
          DAT_00898a20[k].rhw = 1.0f;
        }
        DAT_007d3ff8->fpRenderStateSet(1, 0);
        DAT_007d3ff8->fpRenderStateSet(0xc, 1);
        DAT_007d3ff8->fpRenderStateSet(10, 5);
        DAT_007d3ff8->fpRenderStateSet(0xb, 6);
        DAT_007d3ff8->fpIm2DRenderPrimitive(4, DAT_00898a20, 4);
      }
    }
  }
}
