// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d1e30 (h1): 87.5% (regs 87.5%). Raw converter did not compile. Moved by: case
   order 0/4, 5, 1, 2 as laid out in the binary and the 0x004d2e30 depth probe as a __forceinline helper (48.8% ->
   84.9%), permuter (next-pointer and swapChain-clear placement, 87.5%). Residual: the original keeps
   `(cFormat << 8) & mask` as `xor; mov ah/dh, [raster+0x23]` (no byte narrowing) in cases 0/4 and 5, ours tests the
   byte (`test byte [esi+0x23], 0x60`, `shr 7; not`); plus scheduling in the swap-chain block. Tried: enum,
   int, unsigned and RW's masked GetFormat macro forms (no change). Class (b). Walks the video-memory raster list at
   DAT_007d46e0 ({raster, next}) and re-creates D3D resources for top-level rasters (parent == self), by raster type
   (+0x20): 0/4 palettised (format bits 0x6000) -> SetPaletteEntries (+0x11c) when ext+4 is set, else depth formats
   0x46..0x50 -> CreateTexture (+0x5c, levels 1, usage 2); 1 -> without ext+0 the depth-format probe of 0x004d2e30
   and CreateDepthStencilSurface (+0x74, sizes clamped to >= 1), with ext+0 GetClientRect(DAT_007d4104) sizes,
   ext+0 = DAT_007d4114, ext+0x18 = DAT_00912108; 2 -> with ext+0x1c a new additional swap chain (+0x34) from the
   DAT_009120e0 template for window ext+0x20 and its back buffer (+0x14, released), else the size rescale by the
   template / DAT_00911f84/88 and the format DAT_009120e8; 5 -> without ext+0 CreateCubeTexture (+0x64) or
   CreateTexture (+0x5c) as a render target, retried without the render-target usage in format DAT_009120e8 unless
   hr is 0x8876017c. Any failed hr clears the result (initially 1). FLAGS /G5 (RW core). */
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
typedef struct IUnk { void **v; } IUnk;
typedef int (__stdcall *UnkFn0)(IUnk *self);
typedef int (__stdcall *CheckFmtFn)(IUnk *self, int adapter, int type, int adapterFmt, unsigned int usage, int rtype, int fmt);
typedef int (__stdcall *CheckDSFn)(IUnk *self, int adapter, int type, int adapterFmt, int rtFmt, int dsFmt);
typedef int (__stdcall *CreateTexFn)(IUnk *self, unsigned int w, unsigned int h, unsigned int levels, unsigned int usage, int fmt, int pool, IUnk **out, void *shared);
typedef int (__stdcall *CreateCubeFn)(IUnk *self, unsigned int edge, unsigned int levels, unsigned int usage, int fmt, int pool, IUnk **out, void *shared);
typedef int (__stdcall *CreateDSFn)(IUnk *self, unsigned int w, unsigned int h, int fmt, int ms, unsigned int msq, int discard, IUnk **out, void *shared);
typedef int (__stdcall *SetPalFn)(IUnk *self, unsigned int index, void *entries);
typedef int (__stdcall *CreateChainFn)(IUnk *self, void *pp, IUnk **out);
typedef int (__stdcall *GetBackBufFn)(IUnk *self, unsigned int a, int b, IUnk **out);
typedef enum RwRasterFormat { rwRASTERFORMATDEFAULT = 0, rwRASTERFORMATMIPMAP = 0x8000 } RwRasterFormat;
#define RwRasterGetFormat(r) ((RwRasterFormat)(((r)->cFormat) << 8))
typedef struct Rect4 { int left; int top; int right; int bottom; } Rect4;
typedef struct Present14 { unsigned int width; unsigned int height; int format; unsigned int v3[4]; void *window; unsigned int v8; unsigned int autoDepth; unsigned int v10[4]; } Present14;
typedef struct FmtInfo { unsigned char a; unsigned char depth; unsigned char b; unsigned char c; } FmtInfo;
typedef struct Palette { unsigned int entries[256]; unsigned int globalindex; } Palette;
typedef struct RasExt {
  IUnk *texture; Palette *palette; unsigned char alpha; unsigned char cube; unsigned char automipmapgen; unsigned char lockedMipLevel;
  IUnk *lockedSurface; int pitch; void *bits; int d3dFormat; IUnk *swapChain; void *window;
} RasExt;
typedef struct RwRaster {
  struct RwRaster *parent; unsigned char *cpPixels; void *palette; unsigned int width; unsigned int height; int depth; int stride;
  short nOffsetX; short nOffsetY; unsigned char cType; unsigned char cFlags; unsigned char privateFlags; unsigned char cFormat;
} RwRaster;
typedef struct VidRaster { RwRaster *raster; struct VidRaster *next; } VidRaster;
MGLOBAL(VidRaster *, DAT_007d46e0, 0x007d46e0);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(IUnk *, DAT_007d4110, 0x007d4110);
MGLOBAL(IUnk *, DAT_007d4108, 0x007d4108);
MGLOBAL(int, DAT_007d410c, 0x007d410c);
MGLOBAL(int, DAT_006181b0, 0x006181b0);
MGLOBAL(int, DAT_00911f90, 0x00911f90);
MGLOBAL(int, DAT_00911f9c, 0x00911f9c);
MGLOBAL(unsigned int, DAT_00911f84, 0x00911f84);
MGLOBAL(unsigned int, DAT_00911f88, 0x00911f88);
MGLOBAL(Present14, DAT_009120e0, 0x009120e0);
MGLOBAL(int, DAT_009120f0, 0x009120f0);
MGLOBAL(unsigned int, DAT_009120f4, 0x009120f4);
MGLOBAL(void *, DAT_007d4104, 0x007d4104);
MGLOBAL(IUnk *, DAT_007d4114, 0x007d4114);
MGLOBAL(int, DAT_00912108, 0x00912108);
MGLOBALA(FmtInfo, DAT_00911b00, 0x80, 0x00911b00);
__declspec(dllimport) int __stdcall GetClientRect(void *hWnd, Rect4 *lpRect);

static __forceinline int CheckZFormat(int fmt)
{
  int adapterFmt = DAT_00911f90;
  if (((CheckFmtFn)DAT_007d4108->v[0x28 / 4])(DAT_007d4108, DAT_007d410c, DAT_006181b0, adapterFmt, 2, 1, fmt) >= 0 &&
      ((CheckDSFn)DAT_007d4108->v[0x30 / 4])(DAT_007d4108, DAT_007d410c, DAT_006181b0, adapterFmt, adapterFmt, fmt) >= 0) {
    return 1;
  }
  return 0;
}

#define RASEXT(r) ((RasExt *)((char *)(r) + DAT_00911ae4))

// MATCH: 0x004d1e30 FUN_004d1e30
int FUN_004d1e30(void)
{
  int result = 1;
  int hr = 0;
  VidRaster *cur = DAT_007d46e0;
  while (cur != 0) {
    RwRaster *raster = cur->raster;
    RasExt *rasExt = RASEXT(raster);
    VidRaster *next = cur->next;
    if (raster->parent == raster) {
      switch (raster->cType) {
      case 0:
      case 4:
        if (!(RwRasterGetFormat(raster) & 0x6000)) {
          if (rasExt->d3dFormat >= 0x46 && rasExt->d3dFormat <= 0x50) {
            hr = ((CreateTexFn)DAT_007d4110->v[0x5c / 4])(DAT_007d4110, raster->width, raster->height, 1, 2, rasExt->d3dFormat, 0, &rasExt->texture, 0);
          }
        } else if (rasExt->palette != 0) {
          hr = ((SetPalFn)DAT_007d4110->v[0x11c / 4])(DAT_007d4110, rasExt->palette->globalindex, rasExt->palette->entries);
        }
        break;
      case 5:
        if (rasExt->texture == 0) {
          unsigned int levels = (RwRasterGetFormat(raster) & 0x8000) ? 0 : 1;
          if (rasExt->cube & 0xf) {
            hr = ((CreateCubeFn)DAT_007d4110->v[0x64 / 4])(DAT_007d4110, raster->width, levels, ((rasExt->automipmapgen & 0xf) ? 0x400 : 0) | 1, rasExt->d3dFormat, 0, &rasExt->texture, 0);
          } else {
            hr = ((CreateTexFn)DAT_007d4110->v[0x5c / 4])(DAT_007d4110, raster->width, raster->height, levels, ((rasExt->automipmapgen & 0xf) ? 0x400 : 0) | 1, rasExt->d3dFormat, 0, &rasExt->texture, 0);
          }
          if (hr >= 0) {
            goto next_raster;
          }
          if (hr != (int)0x8876017c) {
            if (rasExt->cube & 0xf) {
              hr = ((CreateCubeFn)DAT_007d4110->v[0x64 / 4])(DAT_007d4110, raster->width, levels, (rasExt->automipmapgen & 0xf) ? 0x400 : 0, DAT_009120e0.format, 0, &rasExt->texture, 0);
            } else {
              hr = ((CreateTexFn)DAT_007d4110->v[0x5c / 4])(DAT_007d4110, raster->width, raster->height, levels, (rasExt->automipmapgen & 0xf) ? 0x400 : 0, DAT_009120e0.format, 0, &rasExt->texture, 0);
            }
            raster->cFormat = DAT_00911b00[DAT_009120e0.format].c;
            raster->depth = DAT_00911b00[DAT_009120e0.format].depth;
            rasExt->d3dFormat = DAT_009120e0.format;
            rasExt->alpha = DAT_00911b00[DAT_009120e0.format].a;
          }
        }
        break;
      case 1:
        if (rasExt->texture == 0) {
          int w;
          int h;
          if (DAT_00911f9c == 16) {
            if (CheckZFormat(0x49)) {
              rasExt->d3dFormat = 0x49;
            } else {
              rasExt->d3dFormat = 0x50;
            }
          } else if (DAT_00911f9c == 32) {
            if (CheckZFormat(0x4b)) {
              rasExt->d3dFormat = 0x4b;
            } else if (CheckZFormat(0x4f)) {
              rasExt->d3dFormat = 0x4f;
            } else if (CheckZFormat(0x47)) {
              rasExt->d3dFormat = 0x47;
            } else {
              rasExt->d3dFormat = 0x4d;
            }
          }
          h = ((int)raster->height < 1) ? 1 : (int)raster->height;
          w = ((int)raster->width < 1) ? 1 : (int)raster->width;
          hr = ((CreateDSFn)DAT_007d4110->v[0x74 / 4])(DAT_007d4110, w, h, rasExt->d3dFormat, DAT_009120f0, DAT_009120f4, 0, &rasExt->texture, 0);
        } else {
          Rect4 rect;
          GetClientRect(DAT_007d4104, &rect);
          raster->width = rect.right;
          raster->height = rect.bottom;
          rasExt->texture = DAT_007d4114;
          rasExt->d3dFormat = DAT_00912108;
        }
        break;
      case 2:
        if (rasExt->swapChain != 0) {
          Present14 present;
          Rect4 rect;
          void *window = rasExt->window;
          present = DAT_009120e0;
          present.window = window;
          GetClientRect(window, &rect);
          present.width = rect.right == 0;
          present.height = rect.bottom == 0;
          rasExt->swapChain = 0;
          present.autoDepth = 0;
          if (((CreateChainFn)DAT_007d4110->v[0x34 / 4])(DAT_007d4110, &present, &rasExt->swapChain) == 0) {
            hr = ((GetBackBufFn)rasExt->swapChain->v[0x14 / 4])(rasExt->swapChain, 0, 0, &rasExt->texture);
            ((UnkFn0)rasExt->texture->v[8 / 4])(rasExt->texture);
            rasExt->d3dFormat = present.format;
          }
        } else {
          if (DAT_00911f84 != 0 && raster->width != DAT_009120e0.width) {
            raster->width = raster->width * DAT_009120e0.width / DAT_00911f84;
          }
          if (DAT_00911f88 != 0 && raster->height != DAT_009120e0.height) {
            raster->height = raster->height * DAT_009120e0.height / DAT_00911f88;
          }
          raster->depth = DAT_00911b00[DAT_009120e0.format].depth;
          raster->cFormat = DAT_00911b00[DAT_009120e0.format].c;
          rasExt->d3dFormat = DAT_009120e0.format;
          rasExt->alpha = DAT_00911b00[DAT_009120e0.format].a;
        }
        break;
      }
    }
    if (hr < 0) {
      result = 0;
    }
  next_raster:
    cur = next;
  }
  return result;
}
