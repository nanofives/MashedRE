// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00473ee0 (h9): by hand, 66.3% (regs 71.5%, ops 88.6%; natural statement order 62.5%). Two FUN_004733b0 bars, a 15-step arc of FUN_00473220 bars
   (x from FUN_004c3b30(8100 - i*i), alpha from DAT_0086eccc*96/255, second bar alpha via __ftol), then the body
   of FUN_004744a0 written out in place (the original has it inlined; a plain __inline copy is not inlined under
   /Ob1, 16.4%; __forceinline 57.4%; written out with white/zero reused as c1/c0 and j as pass: 62.5%).
   Callees take RwV2d/RwRGBA by value (re/match/s11/00473220.c, 004733b0.c), which is why the float constants
   go through stack slots. Residual: frame 0x44 vs 0x3c (8 bytes), and the inlined ribbon's row loop keeps
   (i&1) in ebp/ebx in the original (no spill, no mid-loop entry) vs a spill here. Tried: pos as the ribbon
   origin (51%), c2 merged into black (61%), sqrt call inside the pos.x expression (+0.9), permuter (2540 compiles; its one call-order swap rejected, the store/statement moves kept). Class (b). */
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
MGLOBAL(int, DAT_0086ecc8, 0x0086ecc8);
MGLOBAL(int, DAT_0086eccc, 0x0086eccc);
void FUN_004733b0(RwV2d p, RwV2d s, RwRGBA c0, RwRGBA c1);
void FUN_00473220(RwV2d p, RwV2d s, RwRGBA c0, RwRGBA c1);
float FUN_004c3b30(float v);
// MATCH: 0x00473ee0 FUN_00473ee0
void FUN_00473ee0(int param_1, int param_2, float param_3)
{
  RwRGBA zero, black, white, c2, col;
  int pass, ii, jj;
  RwV2d o; float w2;
  RwV2d p0, p1, p2, p3;
  RwV2d pos, size;
  int i, j, k;
  float r;

  zero.red = 0; zero.green = 0; zero.blue = 0; zero.alpha = 0;
  FUN_0042b8b0();
  FUN_0042b8c0();
  FUN_0042b8b0();
  FUN_0042b8c0();
  pos.x = param_3 - 128.0f;
  black.red = black.green = black.blue = 0;
  black.alpha = 255;
  pos.y = 0.0f;
  size.x = 128.0f;
  size.y = 64.0f;
  FUN_004733b0(pos, size, black, zero);
  pos.y = 416.0f;
  FUN_004733b0(pos, size, black, zero);
  if (DAT_0086ecc8 > 255) DAT_0086ecc8 = 255;
  for (k = 0; k < 2; k++) {
    if (DAT_0086ecc8 > DAT_0086eccc) DAT_0086eccc++;
    if (DAT_0086ecc8 < DAT_0086eccc) DAT_0086eccc--;
  }
  size.y = 34.133335f;
  white.red = white.green = white.blue = 255;
  j = 0;
  i = 90;
  do {
    pos.y = (float)j * 2.8444445f;
    pos.x = param_3 - FUN_004c3b30(8100.0f - (float)i * (float)i) - 128.0f;
    white.alpha = (unsigned char)(DAT_0086eccc * 96 / 255);
    size.x = 128.0f;
    FUN_00473220(pos, size, white, zero);
    c2 = white;
    c2.alpha = (unsigned char)(white.alpha * 1.25f);
    size.x = pos.x;
    pos.x = 0.0f;
    FUN_00473220(pos, size, c2, white);
    j += 12;
    i -= 12;
  } while (i > -90);
  white.red = white.green = white.blue = 128;
  white.alpha = 255;
  for (j = 0; j < 2; j++) {
    for (ii = 0; ii < 3; ii++) {
      for (jj = 0; jj < 7; jj++) {
        o.y = (float)(ii * 21);
        o.x = (float)((ii & 1) * 21 + (jj * 5 + 45) * 8);
        if (j) o.y += 416.0f;
        p0.x = o.x; p0.y = o.y;
        p1.x = o.x + 21.0f; p1.y = o.y;
        p2.x = o.x; p2.y = o.y + 21.0f;
        p3.x = o.x + 21.0f; p3.y = o.y + 21.0f;
        w2 = DAT_007f1010 + DAT_007f1010;
        p0.x += (float)(sin((float)(ii + jj) + w2) * 4.0);
        p1.x += (float)(sin((float)(ii + jj + 1) + w2) * 4.0);
        p2.x += (float)(sin((float)(ii + jj + 1) + w2) * 4.0);
        p3.x += (float)(sin((float)(ii + jj + 2) + w2) * 4.0);
        p0.y += (float)(sin((float)(ii * 2 + jj) + w2) * 2.0);
        p1.y += (float)(sin((float)(ii * 2 + jj + 1) + w2) * 2.0);
        p2.y += (float)(sin((float)(ii * 2 + jj + 2) + w2) * 2.0);
        p3.y += (float)(sin((float)(ii * 2 + jj + 3) + w2) * 2.0);
        if (j == 0) {
          if (ii == 2) { p3.y = 64.0f; p2.y = 64.0f; }
        } else if (ii == 0) {
          p1.y = 416.0f; p0.y = 416.0f;
        }
        col = white;
        if (jj == 0 && (ii & 1) == 0) col = zero;
        p0.x = (float)FUN_0042b8b0() * p0.x * 0.0015625f;
        p0.y = (float)FUN_0042b8c0() * p0.y * 0.0020833334f;
        p1.x = (float)FUN_0042b8b0() * p1.x * 0.0015625f;
        p1.y = (float)FUN_0042b8c0() * p1.y * 0.0020833334f;
        p2.x = (float)FUN_0042b8b0() * p2.x * 0.0015625f;
        p2.y = (float)FUN_0042b8c0() * p2.y * 0.0020833334f;
        p3.x = (float)FUN_0042b8b0() * p3.x * 0.0015625f;
        DAT_00898a20[2].x = p2.x; DAT_00898a20[2].y = p2.y;
        p3.y = (float)FUN_0042b8c0() * p3.y * 0.0020833334f;
        DAT_00898a20[0].x = p0.x; DAT_00898a20[0].y = p0.y;
        DAT_00898a20[3].x = p3.x; DAT_00898a20[3].y = p3.y;
        DAT_00898a20[0].emissiveColor = ((unsigned int)col.alpha << 24) | ((unsigned int)col.red << 16) | ((unsigned int)col.green << 8) | (unsigned int)col.blue;
        DAT_00898a20[1].x = p1.x; DAT_00898a20[1].y = p1.y;
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
