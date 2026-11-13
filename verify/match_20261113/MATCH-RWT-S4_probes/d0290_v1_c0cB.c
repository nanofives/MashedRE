// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* RWC-DRIVER 2026-11-11: 73.4% -> 76.4%. Moved by: cases end in break with one 'return 1' after the switch (RW RWRETURN shape), +0xb = 0xff stored before the privateFlags clear in case 0/4 (permuter). Residual unchanged: the original saves ebp/edi in the prologue; ours sinks them past the cpPixels early exit (inverting the test to 'if (cpPixels != 0) {...}' gets the prologue right but moves the error block to the end, 67%). */
/* MATCH-S20 0x004d0290 (h2): 73.4% (regs 81.5%). Raw 19.3%. D3D9 raster unlock shape: RwRaster / raster-extension
   structs (cube:4/face:4 and automipmapgen:4/compressed:4 bitfields), UnlockRect +0x38 / Release +8,
   GetCubeMapSurface / GetSurfaceLevel +0x48, UpdateSurface +0x78 with RECT/POINT locals, mipmap regeneration
   through 0x004c5dd0. Moves: ext computed after the NULL check, top-raster walk as do { top = top->parent } while
   (top != top->parent), the top extension and its texture in locals. Residual (b): the original pushes ebp at entry
   and duplicates the entry RWERROR block (ours shares it with the default case), and the surface pointer lives in
   the dead param_1 home (see 0x004cc5e0). Permuter: no gain. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef struct { int pluginID; int errorCode; } RwError;
typedef struct { long left, top, right, bottom; } RECT_;
typedef struct { long x, y; } POINT_;
typedef struct RwRaster RwRaster;
struct RwRaster {
  RwRaster *parent; uchar *cpPixels; uchar *palette; int width; int height; int depth; int stride;
  short nOffsetX; short nOffsetY; uchar cType; uchar cFlags; uchar privateFlags; uchar cFormat;
  uchar *originalPixels; int originalWidth; int originalHeight; int originalStride;
};
typedef struct {
  int *texture; void *palette; uchar alpha; uchar cube : 4; uchar face : 4; uchar automipmapgen : 4;
  uchar compressed : 4; uchar lockedMipLevel; int *lockedSurface; int lockedRect[2]; int d3dFormat;
  void *swapChain; void *window;
} RasExt;
typedef long (__stdcall *com0_fn)(int *);
typedef long (__stdcall *getsurf_fn)(int *, uint, int **);
typedef long (__stdcall *getcube_fn)(int *, uint, uint, int **);
typedef long (__stdcall *upd_fn)(int *, int *, RECT_ *, int *, POINT_ *);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int *, DAT_007d4118, 0x007d4118);
int FUN_004c5dd0(RwRaster *p0, int p1);
int FUN_004d7ff0(int p0);
RwError *FUN_004d8480(RwError *p0);
#define EXT(r) ((RasExt *)((uchar *)(r) + DAT_00911ae4))

// MATCH: 0x004d0290 FUN_004d0290
int FUN_004d0290(void *param_1,RwRaster *param_2,int param_3)
{
  RasExt *ext;
  RwRaster *top;
  RasExt *topExt;
  int *tex;
  int *surface;
  long hr;

  if (param_2->cpPixels == 0) {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x80000011);
    FUN_004d8480(&err);
    return 0;
  }
  ext = EXT(param_2);
  switch (param_2->cType & 7) {
  case 0:
  case 4:
    (*(com0_fn *)(*ext->lockedSurface + 0x38))(ext->lockedSurface);
    (*(com0_fn *)(*ext->lockedSurface + 8))(ext->lockedSurface);
    param_2->width = param_2->originalWidth;
    param_2->height = param_2->originalHeight;
    param_2->stride = 0;
    param_2->cpPixels = 0;
    if ((param_2->privateFlags & 4) && (param_2->cFormat & 0x10) && ext->lockedMipLevel == 0 &&
        ext->automipmapgen == 0) {
      param_2->privateFlags &= ~6;
      ext->lockedMipLevel = 0xff;
      top = param_2;
      do {
        top = top->parent;
      } while (top != top->parent);
      FUN_004c5dd0(top, 0);
      break;
    }
    param_2->privateFlags &= ~6;
    break;
  case 2:
  case 5:
    param_2->width = param_2->originalWidth;
    param_2->height = param_2->originalHeight;
    param_2->stride = 0;
    param_2->cpPixels = 0;
    (*(com0_fn *)(*ext->lockedSurface + 0x38))(ext->lockedSurface);
    if (param_2->privateFlags & 4) {
      top = param_2;
      do {
        top = top->parent;
      } while (top != top->parent);
      topExt = EXT(top);
      if ((param_2->cType & 7) == 5) {
        uchar cube = topExt->cube;
        tex = topExt->texture;
        if (cube) {
          hr = (*(getcube_fn *)(*tex + 0x48))(tex, topExt->face, ext->lockedMipLevel, &surface);
        } else {
          hr = (*(getsurf_fn *)(*tex + 0x48))(tex, ext->lockedMipLevel, &surface);
        }
      } else {
        if (topExt->swapChain) surface = topExt->texture;
        else surface = DAT_007d4118;
        hr = 0;
      }
      if (hr >= 0) {
        if (top == param_2) {
          (*(upd_fn *)(*DAT_007d4110 + 0x78))(DAT_007d4110, ext->lockedSurface, 0, surface, 0);
        } else {
          RECT_ rect;
          POINT_ pt;
          rect.left = param_2->nOffsetX;
          rect.top = param_2->nOffsetY;
          rect.right = rect.left + param_2->width;
          rect.bottom = rect.top + param_2->height;
          pt.x = rect.left;
          pt.y = rect.top;
          (*(upd_fn *)(*DAT_007d4110 + 0x78))(DAT_007d4110, ext->lockedSurface, &rect, surface, &pt);
        }
        if ((param_2->cType & 7) == 5) {
          (*(com0_fn *)(*surface + 8))(surface);
        }
      }
      (*(com0_fn *)(*ext->lockedSurface + 8))(ext->lockedSurface);
      param_2->privateFlags &= ~6;
      if ((param_2->cType & 7) == 5 && (param_2->cFormat & 0x10) && ext->lockedMipLevel == 0) {
        ext->lockedMipLevel = 0xff;
        FUN_004c5dd0(top, 0);
      }
      break;
    }
    (*(com0_fn *)(*ext->lockedSurface + 8))(ext->lockedSurface);
    param_2->privateFlags &= ~6;
    break;
  default:
    {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x80000011);
      FUN_004d8480(&err);
    }
    return 0;
  }
  return 1;
}
