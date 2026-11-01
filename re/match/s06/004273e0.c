// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004273e0 (234 B): best 90.4% exact (regs 90.4, ops 90.4), 73/71 ins. Raw g2s was 33.8%.
   Shape: two RwIm2DVertex (7 dwords) on the stack, colour packed (a<<24|r<<16|g<<8|b) from an RGBA byte struct,
   z = RwGlobals(0x007d3ff8)+0x18, rhw 1.0f, then 4 render-state sets via slot +0x20 and one primitive via +0x30.
   All 71 original instructions are present. Residual: the int/x87 interleave around the second scale
   (`mov [esp+0xc],ecx` / `fild` / `fmul [0x5cc560]` sit 1-2 slots later in ours) plus the 2 epilogue
   instructions past the Ghidra size. Tried: colour in a local before the stores (x87 order becomes exact but
   colour/counter swap ECX/EDX, 80.8%, regs 93.2), all 24 declaration orders, 4 loop forms x 6 body orders,
   short temps, separate r/g/b/a temps, inline second scale, operand order (canonicalised), all 24 orders
   of the 4 coordinate stores (both forms), permuter 149 + 182
   compiles. Class (b): the source form that gives both orders together was not found. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
typedef int (*RenderStateSetFn)(int state, void *value);
typedef int (*Im2DRenderPrimitiveFn)(int prim, Im2DVertex *verts, int n);
typedef struct {
  char pad0[0x18];
  float zBufferNear;                          /* +0x18 */
  float zBufferFar;                           /* +0x1c */
  RenderStateSetFn fpRenderStateSet;          /* +0x20 */
  void *fpRenderStateGet;                     /* +0x24 */
  void *fpIm2DRenderLine;                     /* +0x28 */
  void *fpIm2DRenderTriangle;                 /* +0x2c */
  Im2DRenderPrimitiveFn fpIm2DRenderPrimitive; /* +0x30 */
} S06_RwGlobals_4273e0;
MGLOBAL(float, DAT_005cd5a8, 0x005cd5a8);
MGLOBAL(S06_RwGlobals_4273e0 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, DAT_005cc560, 0x005cc560);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);

// MATCH: 0x004273e0 FUN_004273e0
void FUN_004273e0(float x1, float y1, float x2, float y2, RGBA *c)
{
  Im2DVertex v[2];
  float sx, sy;
  int i;

  sx = (float)FUN_0042b8b0() * DAT_005cd5a8;
  sy = (float)FUN_0042b8c0() * DAT_005cc560;
  v[0].x = sx * x1;
  v[0].y = y1 * sy;
  v[1].x = sx * x2;
  v[1].y = sy * y2;
  for (i = 0; i < 2; i++) {
    v[i].color = (c->a << 24) | (c->r << 16) | (c->g << 8) | c->b;
    v[i].z = DAT_007d3ff8->zBufferNear;
    v[i].rhw = 1.0f;
  }
  DAT_007d3ff8->fpRenderStateSet(1, (void *)0);
  DAT_007d3ff8->fpRenderStateSet(0xc, (void *)1);
  DAT_007d3ff8->fpRenderStateSet(10, (void *)5);
  DAT_007d3ff8->fpRenderStateSet(0xb, (void *)6);
  DAT_007d3ff8->fpIm2DRenderPrimitive(1, v, 2);
}
