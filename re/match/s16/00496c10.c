// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00496c10 (h3): by hand, 0x00496c10 100% (regs 100%). The file also holds its register-arg callee
   0x004969a0 (RwIm2D textured full-screen quad) as a `static` function defined first (pilot finding 1):
   parameter order (uv0, color, camera, uv1, raster) gives the original ECX=uv0, EAX=color, EBX=camera,
   then two stack args. 0x004969a0 best 66.0% (regs 78.0%, ops 98.1%): the instruction stream is right, the
   frame is 0x9c vs 0xa0 (the original keeps one dedicated int->float temp slot and separate fx/fy slots),
   and the original loads RwGlobals into ECX before the fx/fy stores and computes 1/nearPlane as
   fld [cam+0x80]; fdivr [1.0] after them, where ours schedules `push 3` early and uses fld 1.0; fdiv.
   Tried: declaration orders, plain vs [2] shorts, explicit int temp, RwV2d size/offset struct locals (59%),
   per-vertex 1/nearPlane (65.2%), permuter 846 compiles (its only gain read fx before assignment, rejected).
   0x004969a0 class (b). 1.0f = the float at 0x005cc320. */
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
typedef struct RwRGBA { uchar red, green, blue, alpha; } RwRGBA;
typedef struct RwV2d { float x, y; } RwV2d;
typedef struct RwRaster { char pad0[0xc]; int width; int height; } RwRaster;
typedef struct RwCamera { char pad0[0x60]; RwRaster *frameBuffer; char pad64[0x1c]; float nearPlane; } RwCamera;
typedef struct RwIm2DVertex { float x, y, z, rhw; uint emissiveColor; float u, v; } RwIm2DVertex;
typedef int (__cdecl *PFN_RenderStateSet)(int state, void *value);
typedef int (__cdecl *PFN_Im2DRenderTriangle)(RwIm2DVertex *verts, int n, int i1, int i2, int i3);
typedef struct RwGlobals {
  RwCamera *curCamera; char pad04[0x14]; float zBufferNear; char pad1c[4];
  PFN_RenderStateSet fpRenderStateSet; char pad24[8]; PFN_Im2DRenderTriangle fpIm2DRenderTriangle;
} RwGlobals;
typedef int (__stdcall *PFN_GetSurfaceLevel)(int *self, uint level, int **out);
typedef int (__stdcall *PFN_GetBackBuffer)(int *self, uint sc, uint bb, int type, int **out);
typedef int (__stdcall *PFN_StretchRect)(int *self, int *src, void *srect, int *dst, void *drect, int filter);
typedef uint (__stdcall *PFN_Release)(int *self);
MGLOBAL(RwGlobals *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int *, DAT_00636b70, 0x00636b70);
MGLOBAL(int *, DAT_00773078, 0x00773078);
MGLOBAL(RwRaster *, DAT_0077307c, 0x0077307c);
void FUN_004c75e0(RwRaster *raster, short *x, short *y);
void FUN_004938e0(RwRaster *raster, int *tex);

#define RWRGBALONG(r, g, b, a) ((uint)(((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

// MATCH: 0x004969a0 FUN_004969a0
static void FUN_004969a0(RwV2d *uv0, RwRGBA *color, RwCamera *camera, RwV2d *uv1, RwRaster *raster)
{
  RwRGBA white;
  short ox[2];
  short oy[2];
  float w, h, fx, fy;
  RwV2d def[2];
  RwIm2DVertex v[4];
  float rhw;
  RwRaster *fb;
  uint c;

  def[0].x = 0.0f;
  def[0].y = 0.0f;
  def[1].x = 1.0f;
  def[1].y = 1.0f;
  white.red = 0xff;
  white.green = 0xff;
  white.blue = 0xff;
  white.alpha = 0xff;
  if (uv0 == 0) {
    uv0 = &def[0];
  }
  if (uv1 == 0) {
    uv1 = &def[1];
  }
  if (color == 0) {
    color = &white;
  }
  fb = camera->frameBuffer;
  w = (float)fb->width;
  h = (float)fb->height;
  FUN_004c75e0(fb, ox, oy);
  fx = (float)ox[0];
  fy = (float)oy[0];
  rhw = 1.0f / camera->nearPlane;
  v[0].x = w * uv0->x + fx;
  v[0].y = h * uv0->y + fy;
  v[0].z = DAT_007d3ff8->zBufferNear;
  v[0].rhw = rhw;
  v[0].u = 0.0f;
  v[0].v = 0.0f;
  v[1].x = w * uv0->x + fx;
  v[1].y = h * uv1->y + fy;
  v[1].z = DAT_007d3ff8->zBufferNear;
  v[1].rhw = rhw;
  v[1].u = 0.0f;
  v[1].v = 1.0f;
  v[2].x = w * uv1->x + fx;
  v[2].y = h * uv1->y + fy;
  v[2].z = DAT_007d3ff8->zBufferNear;
  v[2].rhw = rhw;
  v[2].u = 1.0f;
  v[2].v = 1.0f;
  v[3].x = w * uv1->x + fx;
  v[3].y = h * uv0->y + fy;
  v[3].z = DAT_007d3ff8->zBufferNear;
  v[3].rhw = rhw;
  v[3].u = 1.0f;
  v[3].v = 0.0f;
  c = RWRGBALONG(color->red, color->green, color->blue, color->alpha);
  v[0].emissiveColor = c;
  v[1].emissiveColor = c;
  v[2].emissiveColor = c;
  v[3].emissiveColor = c;
  DAT_007d3ff8->fpRenderStateSet(2, (void *)3);
  DAT_007d3ff8->fpRenderStateSet(7, (void *)1);
  DAT_007d3ff8->fpRenderStateSet(1, raster);
  if (color->alpha != 0xff) {
    DAT_007d3ff8->fpRenderStateSet(0xc, (void *)1);
  }
  DAT_007d3ff8->fpIm2DRenderTriangle(v, 4, 0, 1, 2);
  DAT_007d3ff8->fpIm2DRenderTriangle(v, 4, 2, 3, 0);
  if (color->alpha != 0xff) {
    DAT_007d3ff8->fpRenderStateSet(0xc, (void *)0);
  }
  DAT_007d3ff8->fpRenderStateSet(7, (void *)2);
}

// MATCH: 0x00496c10 FUN_00496c10
void FUN_00496c10(int param_1, int alpha)
{
  RwRGBA color;
  int *surf;
  int *back;

  surf = 0;
  back = 0;
  if (DAT_00636b70 != 0) {
    color.red = 0xff;
    color.green = 0xff;
    color.blue = 0xff;
    color.alpha = (uchar)alpha;
    FUN_004969a0(0, &color, DAT_007d3ff8->curCamera, 0, DAT_0077307c);
    (*(PFN_GetSurfaceLevel *)(*DAT_00636b70 + 0x48))(DAT_00636b70, 0, &surf);
    (*(PFN_GetBackBuffer *)(*DAT_00773078 + 0x48))(DAT_00773078, 0, 0, 0, &back);
    (*(PFN_StretchRect *)(*DAT_00773078 + 0x88))(DAT_00773078, back, 0, surf, 0, 0);
    FUN_004938e0(DAT_0077307c, DAT_00636b70);
    (*(PFN_Release *)(*surf + 8))(surf);
    (*(PFN_Release *)(*back + 8))(back);
  }
}
