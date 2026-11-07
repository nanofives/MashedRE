// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b5750 (helper h1): 81.6% (regs 94.3%, ops 99.4%), 158/158 ins. Typed as an Im2D quad draw: RwRGBA-shaped default colour {ff,ff,ff,ff}, RwV2d-shaped default corners {0,0},{1,1}, raster size and offset as two RwV2d-shaped locals (separate floats: 50-53%), four 28-byte RwIm2DVertex-shaped vertices, colour packed as (a<<24)|(r<<16)|(g<<8)|b, render-state / Im2D triangle calls through typed function pointers at RwGlobals+0x20/+0x2c. recipZ = 1.0f / (camera+0x80): the literal 1.0f (the original's 0x005cc320 is the compiler constant 1.0) and the parenthesized divisor give fld/fdivr. Raw g2s 27.2%. Remaining: stack slot order of the two RwInt16 offset locals vs the int conversion temp (original: shorts at frame+4/+8, temp at +0xc; ours: temp at +4), which also flips eax/ecx for the RwGlobals pointer and the colour. Tried: all orders of the short/colour/corner declarations (no change), explicit int temp (80.4), inner-block shorts (55.1), shorts in a struct (72.8), conversion order, /G5 /G6 no-/GS, permuter 458 variants. Class (b). */
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
typedef struct { uchar red; uchar green; uchar blue; uchar alpha; } RwRGBA_;
typedef struct { float x; float y; } RwV2d_;
typedef struct { float x; float y; float z; float rhw; uint emissiveColor; float u; float v; } RwIm2DVertex_;
typedef int (*RwRenderStateSetFn_)(int state, int value);
typedef int (*RwIm2DRenderTriangleFn_)(RwIm2DVertex_ *verts, int numVerts, int vert1, int vert2, int vert3);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_004c75e0(int p0, short *p1, short *p2);
// MATCH: 0x004b5750 FUN_004b5750
void FUN_004b5750(int param_1,RwV2d_ *param_2,RwV2d_ *param_3,RwRGBA_ *param_4,int param_5)
{
  RwRGBA_ defCol;
  short offY;
  short offX;
  RwV2d_ size;
  RwV2d_ off;
  RwV2d_ defPos[2];
  RwIm2DVertex_ v[4];
  int raster;
  float recipZ;
  uint col;

  defPos[0].x = 0.0f;
  defPos[0].y = 0.0f;
  defPos[1].x = 1.0f;
  defPos[1].y = 1.0f;
  defCol.red = 0xff;
  defCol.green = 0xff;
  defCol.blue = 0xff;
  defCol.alpha = 0xff;
  if (param_2 == 0) {
    param_2 = &defPos[0];
  }
  if (param_3 == 0) {
    param_3 = &defPos[1];
  }
  if (param_4 == 0) {
    param_4 = &defCol;
  }
  raster = *(int *)(param_1 + 0x60);
  size.x = (float)*(int *)(raster + 0xc);
  size.y = (float)*(int *)(raster + 0x10);
  FUN_004c75e0(raster,&offX,&offY);
  off.x = (float)offX;
  off.y = (float)offY;
  recipZ = 1.0f / (*(float *)(param_1 + 0x80));
  v[0].x = size.x * param_2->x + off.x;
  v[0].y = size.y * param_2->y + off.y;
  v[0].z = *(float *)(DAT_007d3ff8 + 0x18);
  v[0].rhw = recipZ;
  v[0].u = 0.0f;
  v[0].v = 0.0f;
  v[1].x = v[0].x;
  v[1].y = size.y * param_3->y + off.y;
  v[1].z = *(float *)(DAT_007d3ff8 + 0x18);
  v[1].rhw = recipZ;
  v[1].u = 0.0f;
  v[1].v = 1.0f;
  v[2].x = size.x * param_3->x + off.x;
  v[2].y = v[1].y;
  v[2].z = *(float *)(DAT_007d3ff8 + 0x18);
  v[2].rhw = recipZ;
  v[2].u = 1.0f;
  v[2].v = 1.0f;
  v[3].x = v[2].x;
  v[3].y = v[0].y;
  v[3].z = *(float *)(DAT_007d3ff8 + 0x18);
  v[3].rhw = recipZ;
  v[3].u = 1.0f;
  v[3].v = 0.0f;
  col = ((uint)param_4->alpha << 24) | ((uint)param_4->red << 16) | ((uint)param_4->green << 8) | (uint)param_4->blue;
  v[0].emissiveColor = col;
  v[1].emissiveColor = col;
  v[2].emissiveColor = col;
  v[3].emissiveColor = col;
  (*(RwRenderStateSetFn_ *)(DAT_007d3ff8 + 0x20))(7,1);
  (*(RwRenderStateSetFn_ *)(DAT_007d3ff8 + 0x20))(1,param_5);
  if (param_4->alpha != 0xff) {
    (*(RwRenderStateSetFn_ *)(DAT_007d3ff8 + 0x20))(0xc,1);
  }
  (*(RwIm2DRenderTriangleFn_ *)(DAT_007d3ff8 + 0x2c))(v,4,0,1,2);
  (*(RwIm2DRenderTriangleFn_ *)(DAT_007d3ff8 + 0x2c))(v,4,2,3,0);
  if (param_4->alpha != 0xff) {
    (*(RwRenderStateSetFn_ *)(DAT_007d3ff8 + 0x20))(0xc,0);
  }
  (*(RwRenderStateSetFn_ *)(DAT_007d3ff8 + 0x20))(7,2);
}
