// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d2340 (h4): 37.3% (regs 43.5%), 408/453 instructions. Raster create (raw converter did not
   compile: C2143). Sets cType/cFlags from flags, clears pixels/palette and the plugin extension (raster +
   DAT_00911ae4; +0xb = 0xff), picks the RW format and depth by type: 0/4 by D3D format checks (FUN_004d38b0 /
   FUN_004d3950, 0x29 for palettised with 0x00911fdc sign, table 0x005d8bf0 8-byte {d3dFormat, depth}) and the
   autoMipMap bit via FUN_004d3a10; 1 by the display depth 0x00911f9c (0x700/0x900); 2 from the back-buffer
   format map 0x00911b00; 5 via FUN_004d38f0 / FUN_004d3a60; RWERROR 0x8000000d otherwise. Then cFormat, the
   zero-size case, the per-type create (FUN_004d2980 / FUN_004d2e30 / FUN_004d2c80, camera: GetClientRect size
   check "Camera raster is too big" 0x00618310, depth by back-buffer format) and links the raster into the list at
   0x007d46e0 (block from the free list 0x007d46e4).
   Residual (b, out of budget): the original keeps the first cType/cFlags stores (ours drops them as dead), gives
   every RWERROR its own stack slot (frame 0x30, ours 0x18) and epilogue, and lays the outer cases out as 0/4, 5, 2,
   1; ours tail-merges the depth stores. Tried: bit tests on format instead of flags (36.2 -> 37.1), case order
   0/4, 5, 2, 1 (34.2, regs 46.6), permuter 1555 compiles (37.7 with `<= -1` spellings; the statement moves kept). */
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
typedef struct RwRaster {
  struct RwRaster *parent; unsigned char *cpPixels; unsigned char *palette; int width; int height; int depth;
  int stride; short nOffsetX; short nOffsetY; unsigned char cType; signed char cFlags; unsigned char privateFlags;
  unsigned char cFormat; int field_24; int originalWidth; int originalHeight;
} RwRaster;
typedef struct RasterExt {
  void *texture; void *palette; unsigned char alpha; unsigned char cube; unsigned char autoMipMap; unsigned char face;
  int field_c; int field_10; int field_14; int format; void *swapChain;
} RasterExt;
typedef struct FormatInfo { int d3dFormat; unsigned char depth; unsigned char pad[3]; } FormatInfo;
typedef struct FormatMap { unsigned short field_0; unsigned short rwFormat; } FormatMap;
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct RECT { long left; long top; long right; long bottom; } RECT;
typedef struct RasterLink { RwRaster *raster; struct RasterLink *next; } RasterLink;
typedef void *(*AllocFromListFn)(void *list, int hint);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(int, DAT_00911f94, 0x00911f94);
MGLOBAL(int, DAT_00911f9c, 0x00911f9c);
MGLOBAL(signed char, DAT_00911fdc, 0x00911fdc);
MGLOBAL(int, DAT_009120e8, 0x009120e8);
MGLOBALA(FormatInfo, DAT_005d8bf0, 16, 0x005d8bf0);
MGLOBALA(FormatMap, DAT_00911b00, 128, 0x00911b00);
MGLOBAL(void *, DAT_007d4104, 0x007d4104);
MGLOBAL(RasterLink *, DAT_007d46e0, 0x007d46e0);
MGLOBAL(void *, DAT_007d46e4, 0x007d46e4);
MGLOBALA(char, s_Camera_raster_is_too_big__00618310, 4, 0x00618310);
__declspec(dllimport) int __stdcall GetClientRect(void *hWnd, RECT *lpRect);
int FUN_004d38b0(int format);
int FUN_004d38f0(int format);
int FUN_004d3950(int format);
int FUN_004d3a10(int format);
int FUN_004d3a60(int format);
int FUN_004d2980(RwRaster *raster, RasterExt *ext);
int FUN_004d2c80(RwRaster *raster, RasterExt *ext);
int FUN_004d2e30(RwRaster *raster, RasterExt *ext);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);

#define RWERRORINVFORMAT() \
  { RwError err; err.pluginID = 1; err.errorCode = FUN_004d7ff0(0x8000000d); FUN_004d8480(&err); return 0; }

// MATCH: 0x004d2340 FUN_004d2340
int FUN_004d2340(void *device, RwRaster *raster, unsigned int flags)
{
  RasterExt *ext = (RasterExt *)((unsigned char *)raster + DAT_00911ae4);
  unsigned int format;
  raster->cFlags = (signed char)(flags & 0xf8);
  raster->cType = (unsigned char)(flags & 7);
  raster->cpPixels = 0;
  raster->palette = 0;
  ext->texture = 0;
  ext->palette = 0;
  ext->alpha = 0;
  ext->autoMipMap = 0;
  ext->cube = 0;
  ext->face = 0xff;
  ext->field_c = 0;
  ext->format = 0;
  raster->cFlags = (signed char)(flags & 0xf8);
  raster->cType = (unsigned char)(flags & 7);
  format = flags & 0xff00;
  switch (flags & 7) {
  case 0:
  case 4:
    if ((format & 0xf00) == 0) {
      if (format & 0x2000) {
        if (!FUN_004d38b0(0x29)) {
          return 0;
        }
        if (DAT_00911fdc < 0) {
          format |= 0x500;
          raster->depth = 8;
        } else {
          format |= 0x600;
          raster->depth = 8;
        }
      } else if (DAT_00911f94 > 0x10 && FUN_004d38b0(0x15)) {
        format |= 0x500;
        raster->depth = 0x20;
      } else if (FUN_004d38b0(0x1a)) {
        format |= 0x300;
        raster->depth = 0x10;
      } else {
        format |= 0x100;
        raster->depth = 0x10;
      }
    } else {
      if (format & 0x4000) {
        RWERRORINVFORMAT();
      }
      if (format & 0x2000) {
        if (!FUN_004d38b0(0x29)) {
          return 0;
        }
        if ((format & 0xf00) != 0x600) {
          if ((format & 0xf00) != 0x500 || DAT_00911fdc >= 0) {
            return 0;
          }
        }
        raster->depth = 8;
      } else {
        int d3dFormat = DAT_005d8bf0[(format >> 8) & 0xf].d3dFormat;
        if (d3dFormat >= 0x46 && d3dFormat <= 0x50) {
          if (!FUN_004d3950(d3dFormat)) {
            return 0;
          }
        } else if (!FUN_004d38b0(d3dFormat)) {
          return 0;
        }
        raster->depth = DAT_005d8bf0[(format >> 8) & 0xf].depth;
      }
    }
    if (!(format & 0x2000) && (flags & 0x9000) == 0x9000 &&
        FUN_004d3a10(DAT_005d8bf0[(format >> 8) & 0xf].d3dFormat)) {
      ext->autoMipMap = (unsigned char)((ext->autoMipMap & 0xf1) | 1);
    }
    break;
  case 1:
    if ((flags & 0xf00) == 0) {
      if (DAT_00911f9c == 0x10) {
        raster->depth = 0x10;
        format = 0x700;
      } else if (DAT_00911f9c == 0x20) {
        format = 0x900;
        raster->depth = 0x20;
      }
    } else if ((flags & 0xf00) == 0x700 && DAT_00911f9c == 0x10) {
      raster->depth = 0x10;
    } else if ((flags & 0xf00) == 0x900 && DAT_00911f9c == 0x20) {
      raster->depth = 0x20;
    } else {
      RWERRORINVFORMAT();
    }
    break;
  case 2:
    if ((flags & 0xf00) == 0) {
      format = DAT_00911b00[DAT_009120e8].rwFormat;
    } else if (DAT_00911b00[DAT_009120e8].rwFormat != format) {
      RWERRORINVFORMAT();
    }
    raster->depth = DAT_00911f94;
    break;
  case 5:
    if ((flags & 0xf00) == 0) {
      if (DAT_00911f94 != 0x10 && FUN_004d38f0(0x16)) {
        format |= 0x600;
        raster->depth = 0x20;
      } else {
        format |= 0x200;
        raster->depth = 0x10;
      }
    } else {
      unsigned char depth = DAT_005d8bf0[(format >> 8) & 0xf].depth;
      if (!FUN_004d38f0(DAT_005d8bf0[(format >> 8) & 0xf].d3dFormat)) {
        RWERRORINVFORMAT();
      }
      raster->depth = depth;
    }
    if ((flags & 0x9000) == 0x9000 && FUN_004d3a60(DAT_005d8bf0[(format >> 8) & 0xf].d3dFormat)) {
      ext->autoMipMap = (unsigned char)((ext->autoMipMap & 0xf1) | 1);
    }
    break;
  default:
    RWERRORINVFORMAT();
  }
  raster->cFormat = (unsigned char)(format >> 8);
  if (raster->width == 0 || raster->height == 0) {
    raster->cFlags = (signed char)0x80;
    raster->stride = 0;
    if (raster->depth == 0) {
      raster->depth = 0x10;
    }
    if (raster->cType != 2) {
      return 1;
    }
  } else {
    switch (raster->cType & 7) {
    case 0:
    case 4:
      if (raster->cFlags < 0) {
        return 1;
      }
      return FUN_004d2980(raster, ext) != 0;
    case 1:
      if (raster->cFlags < 0) {
        return 1;
      }
      if (!FUN_004d2e30(raster, ext)) {
        return 0;
      }
      break;
    case 2: {
      RECT rect;
      GetClientRect(DAT_007d4104, &rect);
      if (raster->cFlags >= 0 && (rect.right < raster->width || rect.bottom < raster->height)) {
        RwError err;
        err.errorCode = FUN_004d7ff0(2, s_Camera_raster_is_too_big__00618310);
        err.pluginID = 1;
        FUN_004d8480(&err);
        return 0;
      }
      if (DAT_009120e8 == 0x15 || DAT_009120e8 == 0x16 || DAT_009120e8 == 0x23) {
        raster->depth = 0x20;
      } else if (DAT_009120e8 == 0x17 || DAT_009120e8 == 0x18 || DAT_009120e8 == 0x19) {
        raster->depth = 0x10;
      } else {
        raster->depth = 0;
      }
      raster->stride = 0;
      raster->cpPixels = 0;
      raster->cFlags = (signed char)0x80;
      raster->originalWidth = raster->width;
      raster->originalHeight = raster->height;
      ext->format = DAT_009120e8;
      break;
    }
    case 5:
      if (raster->cFlags < 0) {
        return 1;
      }
      if (!FUN_004d2c80(raster, ext)) {
        return 0;
      }
      break;
    default:
      return 1;
    }
  }
  {
    RasterLink *link = (RasterLink *)(*(AllocFromListFn *)(DAT_007d3ff8 + 0x118))(DAT_007d46e4, 0x30411);
    link->raster = raster;
    link->next = DAT_007d46e0;
    DAT_007d46e0 = link;
  }
  return 1;
}
