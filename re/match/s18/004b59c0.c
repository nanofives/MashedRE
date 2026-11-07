// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b59c0 (h3): hand, 100% first try. Two-vertex RwIm3D line: RwIm3DVertex-shaped locals (36 B), render-state
   calls through RwGlobals (DAT_007d3ff8) +0x20. */
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
typedef struct { float x, y, z; } V3_4b59c0;
typedef struct { unsigned char red, green, blue, alpha; } RGBA_4b59c0;
typedef struct { V3_4b59c0 pos; V3_4b59c0 normal; unsigned int color; float u, v; } Vtx_4b59c0;
typedef int (*RenderStateSet_4b59c0)(int state, int value);
typedef struct { unsigned char pad[0x20]; RenderStateSet_4b59c0 renderStateSet; } Globals_4b59c0;
MGLOBAL(Globals_4b59c0 *, DAT_007d3ff8, 0x007d3ff8);
void *FUN_004cd070(Vtx_4b59c0 *verts, int n, void *matrix, unsigned int flags);
int FUN_004cd430(int prim, int p1);
int FUN_004cd140(void);
// MATCH: 0x004b59c0 FUN_004b59c0
void FUN_004b59c0(V3_4b59c0 *param_1, V3_4b59c0 *param_2, RGBA_4b59c0 *param_3)
{
  RGBA_4b59c0 white;
  V3_4b59c0 zero0;
  V3_4b59c0 zero1;
  Vtx_4b59c0 v[2];
  unsigned int flags;
  unsigned int c;

  zero0.x = 0.0f;
  zero0.y = 0.0f;
  zero0.z = 0.0f;
  zero1.x = 0.0f;
  zero1.y = 0.0f;
  zero1.z = 0.0f;
  white.red = 0xff;
  white.green = 0xff;
  white.blue = 0xff;
  white.alpha = 0xff;
  flags = 0x10;
  if (param_1 == 0) {
    param_1 = &zero0;
  }
  if (param_2 == 0) {
    param_2 = &zero1;
  }
  if (param_3 == 0) {
    param_3 = &white;
  }
  if (param_3->alpha == 0xff) {
    flags = 0x12;
  }
  v[0].pos = *param_1;
  v[1].pos = *param_2;
  c = ((unsigned int)param_3->alpha << 24) | ((unsigned int)param_3->red << 16) | ((unsigned int)param_3->green << 8) | param_3->blue;
  v[0].color = c;
  v[1].color = c;
  if (param_3->alpha != 0xff) {
    DAT_007d3ff8->renderStateSet(0xc, 1);
  }
  DAT_007d3ff8->renderStateSet(1, 0);
  if (FUN_004cd070(v, 2, 0, flags)) {
    FUN_004cd430(0, 1);
    FUN_004cd140();
  }
  if (param_3->alpha != 0xff) {
    DAT_007d3ff8->renderStateSet(0xc, 0);
  }
}
