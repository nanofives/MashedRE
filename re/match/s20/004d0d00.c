// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d0d00 (h4): 35.5% (regs 67.8%), 242/242 instructions. Raster-to-camera blit: walks the source raster
   and the current camera raster (0x007d46c8) to their top parents, gets each D3D surface (camera type 2: ext+0 when
   ext+0x1c is set, else the back buffer 0x007d4118; otherwise GetSurfaceLevel / GetCubeMapSurface (+0x48) of the
   texture, face = high nibble of ext+9), builds the source RECT from the raster offset/size and the destination
   RECT from the camera offset + rect (scaled: rect w/h, else raster w/h), then StretchRect (+0x88, filter from caps
   0x00912094), UpdateSurface (+0x78) or GetRenderTargetData (+0x80) by the two raster types, releases the
   non-camera surfaces and returns hr >= 0. Raw converter output (unaff_EBP) rewritten by hand.
   Residual: register/stack assignment: the original keeps the raster in edi and pushes ebx/ebp only before the
   destination walk (late saves), ours saves all four at entry, and the destination surface lives in the raster
   parameter's stack slot in the original. Tried: if/else order of the camera test (23.6 -> 26.9), ext/texture
   locals (28.8), permuter 392 compiles (cube test negated for the destination: 35.5, kept). Class (b). */
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
typedef struct ComObj { void **lpVtbl; } ComObj;
typedef long (__stdcall *GetSurfaceLevelFn)(ComObj *self, unsigned int level, ComObj **surface);
typedef long (__stdcall *GetCubeMapSurfaceFn)(ComObj *self, int face, unsigned int level, ComObj **surface);
typedef long (__stdcall *UpdateSurfaceFn)(ComObj *self, ComObj *src, const void *srcRect, ComObj *dst, const void *dstPoint);
typedef long (__stdcall *GetRenderTargetDataFn)(ComObj *self, ComObj *src, ComObj *dst);
typedef long (__stdcall *StretchRectFn)(ComObj *self, ComObj *src, const void *srcRect, ComObj *dst, const void *dstRect, int filter);
typedef unsigned long (__stdcall *ReleaseFn)(ComObj *self);
#define VCALL(T, o, i) ((T)(o)->lpVtbl[(i)])
typedef struct RwRaster {
  struct RwRaster *parent; unsigned char *cpPixels; unsigned char *palette; int width; int height; int depth;
  int stride; short nOffsetX; short nOffsetY; unsigned char cType; unsigned char cFlags; unsigned char privateFlags;
  unsigned char cFormat;
} RwRaster;
typedef struct RasterExt {
  ComObj *texture; void *palette; unsigned char alpha; unsigned char cube : 4; unsigned char face : 4;
  unsigned char pad[2]; int field_c; int field_10; int field_14; int format; void *swapChain;
} RasterExt;
typedef struct RwRect { int x; int y; int w; int h; } RwRect;
typedef struct RECT { long left; long top; long right; long bottom; } RECT;
typedef struct POINT { long x; long y; } POINT;
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(RwRaster *, DAT_007d46c8, 0x007d46c8);
MGLOBAL(ComObj *, DAT_007d4110, 0x007d4110);
MGLOBAL(ComObj *, DAT_007d4118, 0x007d4118);
MGLOBAL(unsigned int, DAT_00912094, 0x00912094);
#define RASTEREXT(r) ((RasterExt *)((unsigned char *)(r) + DAT_00911ae4))

// MATCH: 0x004d0d00 FUN_004d0d00
int FUN_004d0d00(RwRaster *raster, RwRect *rect, int scaled)
{
  RwRaster *topSrc;
  RwRaster *topDst;
  ComObj *srcSurf;
  ComObj *dstSurf;
  RECT srcRect;
  RECT dstRect;
  POINT pt;
  long hr;

  topSrc = raster;
  while (topSrc->parent != topSrc) {
    topSrc = topSrc->parent;
  }
  if (topSrc->cType != 2) {
    RasterExt *ext = RASTEREXT(topSrc);
    ComObj *tex = ext->texture;
    if (ext->cube) {
      VCALL(GetCubeMapSurfaceFn, tex, 18)(tex, ext->face, 0, &srcSurf);
    } else {
      VCALL(GetSurfaceLevelFn, tex, 18)(tex, 0, &srcSurf);
    }
  } else if (RASTEREXT(topSrc)->swapChain) {
    srcSurf = RASTEREXT(topSrc)->texture;
  } else {
    srcSurf = DAT_007d4118;
  }

  topDst = DAT_007d46c8;
  while (topDst->parent != topDst) {
    topDst = topDst->parent;
  }
  if (topDst->cType != 2) {
    RasterExt *ext = RASTEREXT(topDst);
    ComObj *tex = ext->texture;
    if (!ext->cube) {
      VCALL(GetSurfaceLevelFn, tex, 18)(tex, 0, &dstSurf);
    } else {
      VCALL(GetCubeMapSurfaceFn, tex, 18)(tex, ext->face, 0, &dstSurf);
    }
  } else if (RASTEREXT(topDst)->swapChain) {
    dstSurf = RASTEREXT(topDst)->texture;
  } else {
    dstSurf = DAT_007d4118;
  }

  srcRect.left = raster->nOffsetX;
  srcRect.top = raster->nOffsetY;
  srcRect.right = srcRect.left + raster->width;
  srcRect.bottom = srcRect.top + raster->height;
  if (scaled) {
    dstRect.left = DAT_007d46c8->nOffsetX + rect->x;
    dstRect.top = DAT_007d46c8->nOffsetY + rect->y;
    dstRect.right = dstRect.left + rect->w;
    dstRect.bottom = dstRect.top + rect->h;
  } else {
    dstRect.left = DAT_007d46c8->nOffsetX + rect->x;
    dstRect.top = DAT_007d46c8->nOffsetY + rect->y;
    dstRect.right = dstRect.left + raster->width;
    dstRect.bottom = dstRect.top + raster->height;
  }

  if (topDst->cType == 2 || topDst->cType == 5) {
    if (topSrc->cType == 2 || topSrc->cType == 5) {
      int filter = 0;
      if (scaled) {
        if (DAT_00912094 & 0x2000200) {
          filter = 2;
        }
      } else if (DAT_00912094 & 0x1000100) {
        filter = 1;
      }
      hr = VCALL(StretchRectFn, DAT_007d4110, 34)(DAT_007d4110, srcSurf, &srcRect, dstSurf, &dstRect, filter);
    } else {
      pt.x = srcRect.left;
      pt.y = srcRect.top;
      hr = VCALL(UpdateSurfaceFn, DAT_007d4110, 30)(DAT_007d4110, srcSurf, &srcRect, dstSurf, &pt);
    }
  } else if (topSrc->cType == 2 || topSrc->cType == 5) {
    hr = VCALL(GetRenderTargetDataFn, DAT_007d4110, 32)(DAT_007d4110, srcSurf, dstSurf);
  } else {
    pt.x = srcRect.left;
    pt.y = srcRect.top;
    hr = VCALL(UpdateSurfaceFn, DAT_007d4110, 30)(DAT_007d4110, srcSurf, &srcRect, dstSurf, &pt);
  }

  if (topDst->cType != 2) {
    VCALL(ReleaseFn, dstSurf, 2)(dstSurf);
  }
  if (topSrc->cType != 2) {
    VCALL(ReleaseFn, srcSurf, 2)(srcSurf);
  }
  return hr >= 0;
}
