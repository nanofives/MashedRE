// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00483a70 (h12): 100% (regs 100%), first hand pass. param_1 is a struct holding five RwMatrix-shaped
   0x40-byte blocks from +0x28 (pos at +0x30 of each). Five RwRGBA locals (two as a 2-element array indexed i&1),
   render state 6 off/on through RwGlobals (0x007d3ff8) +0x20, 0x004b5af0 per matrix (scale 0.25f), then 10
   segments of 0x00482ae0 interpolation and 0x004b59c0 line draws. Vector copies are struct assignments. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct RwV3d483a70 { float x, y, z; } RwV3d483a70;
typedef struct RwRGBA483a70 { uchar r, g, b, a; } RwRGBA483a70;
typedef struct Mat483a70 { RwV3d483a70 right; int flags; RwV3d483a70 up; int pad1; RwV3d483a70 at; int pad2;
  RwV3d483a70 pos; int pad3; } Mat483a70;
typedef struct Tube483a70 { char pad[0x28]; Mat483a70 m[5]; } Tube483a70;
typedef int (*RenderState483a70)(int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, DAT_005cc56c, 0x005cc56c);
void FUN_00482ae0(float t, RwV3d483a70 *out, RwV3d483a70 *p0, RwV3d483a70 *p1, RwV3d483a70 *p2, RwV3d483a70 *p3);
void FUN_004b59c0(RwV3d483a70 *a, RwV3d483a70 *b, RwRGBA483a70 *col);
void FUN_004b5af0(Mat483a70 *m, RwRGBA483a70 *col, float scale);
// MATCH: 0x00483a70 FUN_00483a70
void FUN_00483a70(Tube483a70 *param_1)
{
  RwRGBA483a70 c0;
  RwRGBA483a70 c1;
  RwRGBA483a70 c2;
  RwRGBA483a70 cols[2];
  RwV3d483a70 a;
  RwV3d483a70 b;
  RwV3d483a70 p3;
  RwV3d483a70 p2;
  RwV3d483a70 p1;
  RwV3d483a70 p0;
  int i;

  c0.r = 0x20; c0.g = 0x20; c0.b = 0xc0; c0.a = 0xff;
  c1.r = 0xff; c1.g = 0xff; c1.b = 0xff; c1.a = 0xff;
  c2.r = 0xff; c2.g = 0x20; c2.b = 0xc0; c2.a = 0xff;
  cols[0].r = 0xc0; cols[0].g = 0xc0; cols[0].b = 0xc0; cols[0].a = 0xff;
  cols[1].r = 0x40; cols[1].g = 0x40; cols[1].b = 0x40; cols[1].a = 0xff;
  ((RenderState483a70)*(int *)(DAT_007d3ff8 + 0x20))(6, 0);
  FUN_004b5af0(&param_1->m[1], 0, 0.25f);
  FUN_004b5af0(&param_1->m[2], 0, 0.25f);
  FUN_004b5af0(&param_1->m[0], &c0, 0.25f);
  FUN_004b5af0(&param_1->m[3], &c1, 0.25f);
  FUN_004b5af0(&param_1->m[4], &c2, 0.25f);
  b = param_1->m[1].pos;
  a = param_1->m[2].pos;
  FUN_004b59c0(&b, &a, 0);
  p0 = param_1->m[4].pos;
  p1 = param_1->m[1].pos;
  p2 = param_1->m[2].pos;
  p3 = param_1->m[3].pos;
  for (i = 0; i < 10; i++) {
    FUN_00482ae0((float)i * DAT_005cc56c, &b, &p0, &p1, &p2, &p3);
    FUN_00482ae0((float)(i + 1) * DAT_005cc56c, &a, &p0, &p1, &p2, &p3);
    FUN_004b59c0(&b, &a, &cols[i & 1]);
  }
  ((RenderState483a70)*(int *)(DAT_007d3ff8 + 0x20))(6, 1);
}
