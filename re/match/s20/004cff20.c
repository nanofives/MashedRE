// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* RWC-DRIVER 2026-11-11: 62.5% -> 62.5% (ops 91.9 -> 92.6%, 281 -> 283/283 ins). Behaviour fix only: both RWERROR blocks stored pluginID = 1 after the FUN_004d8480 call (a dead store the compiler dropped); it is now stored first, as the original does (mov [esp+0x20], 1 before the call). Prologue residual unchanged (same shrink-wrap class as 0x004d0290). */
/* MATCH-S20 0x004cff20 (h1): 62.5% (regs 68.2%). Raw converter 25.8%. Structure is right (ops 92.9%):
   the residual is the stack frame. The original frame is 0x44 with ebx/ebp/edi saved at entry and keeps the
   type-2/5 source surface in the raster parameter's slot and the texture pointer spilled there; ours is 0x50
   with ebx/ebp/edi shrink-wrapped past the cpPixels check, which shifts every stack offset. Tried: the surface
   in the parameter slot through a void* raster parameter (49.0%, aliasing spills), loop and ext-pointer forms
   (36.8% -> 55.8%), operand order (56.2%), permuter 916 compiles (RWERROR store order, cube-branch order).
   Class (b). Written in the shape of RW 3.6 _rwD3D9RasterLock(pixels, raster, accessMode):
   RWERROR 0x8000000e when raster+4 (cpPixels) is set; walks parents to the top raster; mip = accessMode >> 8;
   lock flags 0x800, 0x8810 without access bit 0. Raster type (byte +0x20 & 7): 0/4 -> top ext (raster +
   DAT_00911ae4) object vtable +0x48 into ext+0xc, then LockRect (+0x34) with NULL or the sub-rectangle;
   2/5 -> source surface (type 5: +0x48 level/face surface; 2: ext+0x1c ? ext+0 : DAT_007d4118), GetDesc (+0x30),
   device (DAT_007d4110) +0x90 offscreen surface into ext+0xc, +0x80 copy when access bit 1, LockRect, Release on
   failure and of the type-5 surface; others RWERROR 0x8000000e. Success sets privateFlags (+0x22) bits 2/4 and
   the pixels/size/stride fields (width/height >> mip, clamped to 1), ext+0xb = mip, *pixels = cpPixels.
   The jump table at 0x004d026c follows the Ghidra size. FLAGS /G5 (RW core). */
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
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct IUnk { void **v; } IUnk;
typedef int (__stdcall *UnkFn0)(IUnk *self);
typedef int (__stdcall *GetSurf3)(IUnk *self, unsigned int level, IUnk **out);
typedef int (__stdcall *GetSurf4)(IUnk *self, unsigned int face, unsigned int level, IUnk **out);
typedef int (__stdcall *LockRectFn)(IUnk *self, void *locked, void *rect, unsigned int flags);
typedef int (__stdcall *GetDescFn)(IUnk *self, void *desc);
typedef int (__stdcall *CreateOffFn)(IUnk *self, unsigned int w, unsigned int h, int fmt, int pool, IUnk **out, void *shared);
typedef int (__stdcall *GetRTDataFn)(IUnk *self, IUnk *src, IUnk *dst);
typedef struct Rect4 { int left; int top; int right; int bottom; } Rect4;
typedef struct SurfDesc { int format; int type; unsigned int usage; int pool; int ms; unsigned int msq; unsigned int width; unsigned int height; } SurfDesc;
typedef struct LockedRect { int pitch; unsigned char *bits; } LockedRect;
typedef struct RasExt {
  IUnk *texture; void *palette; unsigned char alpha; unsigned char cubeFace; unsigned char b10; unsigned char lockedMipLevel;
  IUnk *lockedSurface; LockedRect lockedRect; int d3dFormat; int swapChain;
} RasExt;
typedef struct RwRaster {
  struct RwRaster *parent; unsigned char *cpPixels; void *palette; int width; int height; int depth; int stride;
  short nOffsetX; short nOffsetY; unsigned char cType; unsigned char cFlags; unsigned char privateFlags; unsigned char cFormat;
  unsigned char *originalPixels; int originalWidth; int originalHeight; int originalStride;
} RwRaster;
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(IUnk *, DAT_007d4110, 0x007d4110);
MGLOBAL(IUnk *, DAT_007d4118, 0x007d4118);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);

#define RASEXT(r) ((RasExt *)((char *)(r) + DAT_00911ae4))

// MATCH: 0x004cff20 FUN_004cff20
int FUN_004cff20(unsigned char **pixels, RwRaster *raster, int accessMode)
{
  RwRaster *topRaster;
  RasExt *rasExt;
  unsigned char mipLevel;
  unsigned int flags;
  int hr;

  if (raster->cpPixels != 0) {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x8000000e);
    FUN_004d8480(&err);
    return 0;
  }
  rasExt = RASEXT(raster);
  topRaster = raster;
  do {
    topRaster = topRaster->parent;
  } while (topRaster != topRaster->parent);
  mipLevel = (unsigned char)(accessMode >> 8);
  flags = 0x800;
  if (!(accessMode & 1)) {
    flags |= 0x8010;
  }
  switch (raster->cType & 7) {
  case 0:
  case 4:
    if (RASEXT(topRaster)->cubeFace & 0xf) {
      hr = ((GetSurf4)RASEXT(topRaster)->texture->v[0x48 / 4])(RASEXT(topRaster)->texture, RASEXT(topRaster)->cubeFace >> 4, mipLevel, &rasExt->lockedSurface);
    } else {
      hr = ((GetSurf3)RASEXT(topRaster)->texture->v[0x48 / 4])(RASEXT(topRaster)->texture, mipLevel, &rasExt->lockedSurface);
    }
    if (hr >= 0) {
      if (topRaster == raster) {
        hr = ((LockRectFn)rasExt->lockedSurface->v[0x34 / 4])(rasExt->lockedSurface, &rasExt->lockedRect, 0, flags);
      } else {
        Rect4 rect;
        rect.left = raster->nOffsetX;
        rect.top = raster->nOffsetY;
        rect.right = rect.left + raster->width;
        rect.bottom = rect.top + raster->height;
        hr = ((LockRectFn)rasExt->lockedSurface->v[0x34 / 4])(rasExt->lockedSurface, &rasExt->lockedRect, &rect, flags);
      }
      if (hr >= 0) {
        if (accessMode & 2) {
          raster->privateFlags |= 2;
        }
        if (accessMode & 1) {
          raster->privateFlags |= 4;
        }
      }
    }
    break;
  case 2:
  case 5:
    {
      IUnk *surface;
      SurfDesc desc;
      if ((raster->cType & 7) == 5) {
        if (!(RASEXT(topRaster)->cubeFace & 0xf)) {
          hr = ((GetSurf3)RASEXT(topRaster)->texture->v[0x48 / 4])(RASEXT(topRaster)->texture, mipLevel, &surface);
        } else {
          hr = ((GetSurf4)RASEXT(topRaster)->texture->v[0x48 / 4])(RASEXT(topRaster)->texture, RASEXT(topRaster)->cubeFace >> 4, mipLevel, &surface);
        }
        if (hr < 0) {
          return 0;
        }
      } else if (RASEXT(topRaster)->swapChain != 0) {
        surface = RASEXT(topRaster)->texture;
      } else {
        surface = DAT_007d4118;
      }
      ((GetDescFn)surface->v[0x30 / 4])(surface, &desc);
      hr = ((CreateOffFn)DAT_007d4110->v[0x90 / 4])(DAT_007d4110, desc.width, desc.height, desc.format, 2, &rasExt->lockedSurface, 0);
      if (hr >= 0) {
        if (accessMode & 2) {
          ((GetRTDataFn)DAT_007d4110->v[0x80 / 4])(DAT_007d4110, surface, rasExt->lockedSurface);
        }
        if (topRaster == raster) {
          hr = ((LockRectFn)rasExt->lockedSurface->v[0x34 / 4])(rasExt->lockedSurface, &rasExt->lockedRect, 0, flags);
        } else {
          Rect4 rect;
          rect.left = raster->nOffsetX;
          rect.top = raster->nOffsetY;
          rect.right = rect.left + raster->width;
          rect.bottom = rect.top + raster->height;
          hr = ((LockRectFn)rasExt->lockedSurface->v[0x34 / 4])(rasExt->lockedSurface, &rasExt->lockedRect, &rect, flags);
        }
        if (hr >= 0) {
          if (accessMode & 2) {
            raster->privateFlags |= 2;
          }
          if (accessMode & 1) {
            raster->privateFlags |= 4;
          }
        } else {
          ((UnkFn0)rasExt->lockedSurface->v[8 / 4])(rasExt->lockedSurface);
        }
      }
      if ((raster->cType & 7) == 5) {
        ((UnkFn0)surface->v[8 / 4])(surface);
      }
    }
    break;
  default:
    {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x8000000e);
      FUN_004d8480(&err);
      return 0;
    }
  }
  if (hr < 0) {
    return 0;
  }
  raster->cpPixels = rasExt->lockedRect.bits;
  raster->originalWidth = raster->width;
  raster->originalHeight = raster->height;
  raster->width = raster->width >> mipLevel;
  raster->height = raster->height >> mipLevel;
  if (raster->width == 0) {
    raster->width = 1;
  }
  if (raster->height == 0) {
    raster->height = 1;
  }
  raster->stride = rasExt->lockedRect.pitch;
  rasExt->lockedMipLevel = mipLevel;
  *pixels = raster->cpPixels;
  return 1;
}
