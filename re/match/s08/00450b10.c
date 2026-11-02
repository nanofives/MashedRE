// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00450b10: 86.6% (regs 85.7%) by hand + permuter (raw 20.2%). An RwIm2D quad: four 0x1c-byte
   vertices at 0x00898a20, RwGlobals (DAT_007d3ff8) zBufferNear +0x18, fpRenderStateSet +0x20,
   fpIm2DRenderPrimitive +0x30, the colour as RWRGBALONG of an RwRGBA passed by value. What moved it: the four uv
   floats read into locals before any store (the original loads all of param_7 first and spills v1 into
   param_7's slot) 57->64%, x+w / y+h as locals (y+h spills into param_2's slot), then the permuter's statement
   order (the vertex stores below are its order, not a natural one) 64->86.6%. Residual (b): x87 scheduling of
   the u0/v0 stores and the two fadds against the integer moves, and where the vertex-0 rhw = 1.0 store lands.
   Tried: natural vertex order, original store order, fadd operand swaps, two permuter runs (2300 compiles). */
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
typedef struct { float x, y, z, rhw; unsigned int color; float u, v; } Im2DVertex_00450b10;
typedef struct { unsigned char r, g, b, a; } RGBA_00450b10;
typedef int (*RenderStateSet_00450b10)(int state, int value);
typedef int (*Im2DRenderPrimitive_00450b10)(int type, Im2DVertex_00450b10 *verts, int n);
typedef struct {
  char pad00[0x18];
  float zBufferNear;                        /* 0x18 */
  char pad1c[4];
  RenderStateSet_00450b10 fpRenderStateSet; /* 0x20 */
  char pad24[0xc];
  Im2DRenderPrimitive_00450b10 fpIm2DRenderPrimitive; /* 0x30 */
} RwGlobals_00450b10;
MGLOBAL(RwGlobals_00450b10 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(Im2DVertex_00450b10, DAT_00898a20, 4, 0x00898a20);
// MATCH: 0x00450b10 FUN_00450b10
void FUN_00450b10(int param_1, float param_2, float param_3, float param_4, float param_5, RGBA_00450b10 param_6,
                  float *param_7)
{
  int i;
  float u0, v0, u1, v1;
  float x1, y1;

  u0 = param_7[0];
  u1 = param_7[2];
  v0 = param_7[1];
  DAT_00898a20[0].rhw = 1.0f;
  v1 = param_7[3];
  DAT_00898a20[0].x = param_2;
  DAT_00898a20[0].y = param_3;
  DAT_00898a20[0].z = DAT_007d3ff8->zBufferNear;
  DAT_00898a20[0].u = u0;
  DAT_00898a20[1].y = param_3;
  DAT_00898a20[2].x = param_2;
  DAT_00898a20[0].v = v0;
  DAT_00898a20[1].u = u1;
  x1 = param_4 + param_2;
  DAT_00898a20[1].x = x1;
  DAT_00898a20[1].v = v0;
  DAT_00898a20[2].u = u0;
  DAT_00898a20[3].u = u1;
  y1 = param_3 + param_5;
  DAT_00898a20[2].y = y1;
  DAT_00898a20[2].v = v1;
  DAT_00898a20[3].y = y1;
  DAT_00898a20[3].x = x1;
  DAT_00898a20[3].v = v1;
  for (i = 0; i < 4; i++) {
    DAT_00898a20[i].color = ((unsigned int)param_6.a << 24) | ((unsigned int)param_6.r << 16) |
                            ((unsigned int)param_6.g << 8) | (unsigned int)param_6.b;
    DAT_00898a20[i].z = DAT_007d3ff8->zBufferNear;
    DAT_00898a20[i].rhw = 1.0f;
  }
  if (param_1 != 0) {
    DAT_007d3ff8->fpRenderStateSet(1, param_1);
    DAT_007d3ff8->fpRenderStateSet(9, 2);
  }
  else {
    DAT_007d3ff8->fpRenderStateSet(1, 0);
  }
  DAT_007d3ff8->fpRenderStateSet(8, 0);
  DAT_007d3ff8->fpRenderStateSet(6, 0);
  DAT_007d3ff8->fpRenderStateSet(0xc, 1);
  DAT_007d3ff8->fpIm2DRenderPrimitive(4, DAT_00898a20, 4);
  DAT_007d3ff8->fpRenderStateSet(8, 1);
  DAT_007d3ff8->fpRenderStateSet(6, 1);
}
