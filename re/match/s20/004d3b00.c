// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d3b00 (h4): 30.6% (regs 55.6%), 192/216 instructions. Raster format re-selection: creates a 32-bit
   image of the raster size (FUN_004cdca0), allocates pixels, reads the raster into it (FUN_004de1b0), picks the
   format by alpha (ext+8): with alpha 8888 (0x500) when the display depth (0x00911f94) > 16 and D3DFMT 0x15 is
   usable, else 4444 (0x300) when 0x1a is usable, else 1555 (0x100); without alpha 888 (0x600) for 0x16, else 565
   (0x200). Usability by raster type: 0/4 CheckDeviceFormat(usage 0, cube (ext+9 low nibble) ? 5 : 3), 5 ->
   FUN_004d39b0 / FUN_004d38f0 (cube / not). Then FUN_004d3090, FUN_004d2340(0, raster, type | format | 0x9000),
   FUN_004df120(raster, image), image destroy.
   The check is a __forceinline helper here (plain __inline is not inlined by 9176: the original inlines it three
   times). Residual: (1) the original also inlines FUN_004d39b0's body in the first (0x15) expansion only and calls
   it in the others (an inline-budget effect: an __inline FUN_004d39b0 definition in this file is never inlined,
   __forceinline would inline all three); (2) raster/ext live in edi/esi in the original, esi/edi here; (3) the
   original hoists the cube nibble and cType loads above the depth test. Tried: helper with (raster), (raster, cube),
   (raster, ext) params, a cube local, the macro everywhere instead of an ext local, permuter 117 compiles.
   Class (b). */
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
typedef struct IDirect3D9 IDirect3D9;
typedef struct IDirect3D9Vtbl {
  void *QueryInterface; void *AddRef; void *Release; void *RegisterSoftwareDevice; void *GetAdapterCount;
  void *GetAdapterIdentifier; void *GetAdapterModeCount; void *EnumAdapterModes; void *GetAdapterDisplayMode;
  void *CheckDeviceType;
  long (__stdcall *CheckDeviceFormat)(IDirect3D9 *self, unsigned int adapter, int deviceType, int adapterFormat,
                                      unsigned long usage, int rType, int checkFormat);
  void *CheckDeviceMultiSampleType;
  long (__stdcall *CheckDepthStencilMatch)(IDirect3D9 *self, unsigned int adapter, int deviceType,
                                           int adapterFormat, int renderTargetFormat, int depthStencilFormat);
} IDirect3D9Vtbl;
struct IDirect3D9 { IDirect3D9Vtbl *lpVtbl; };
MGLOBAL(IDirect3D9 *, DAT_007d4108, 0x007d4108);
MGLOBAL(unsigned int, DAT_007d410c, 0x007d410c);
MGLOBAL(int, DAT_006181b0, 0x006181b0);
MGLOBAL(int, DAT_00911f90, 0x00911f90);
typedef struct RwRaster {
  struct RwRaster *parent; unsigned char *cpPixels; unsigned char *palette; int width; int height; int depth;
  int stride; short nOffsetX; short nOffsetY; unsigned char cType; unsigned char cFlags; unsigned char privateFlags;
  unsigned char cFormat;
} RwRaster;
typedef struct RasterExt {
  void *texture; void *palette; unsigned char alpha; unsigned char cube : 4; unsigned char face : 4;
} RasterExt;
typedef struct RwImage RwImage;
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(int, DAT_00911f94, 0x00911f94);
RwImage *FUN_004cdca0(int width, int height, int depth);
int FUN_004cdd00(RwImage *image);
RwImage *FUN_004cdd60(RwImage *image);
int FUN_004d2340(int a, RwRaster *raster, int flags);
int FUN_004d3090(int a, RwRaster *raster, int b);
int FUN_004d38f0(int format);
int FUN_004d39b0(int format);
RwImage *FUN_004de1b0(RwImage *image, RwRaster *raster, int b);
RwRaster *FUN_004df120(RwRaster *raster, RwImage *image, int b);
#define RASTEREXT(r) ((RasterExt *)((unsigned char *)(r) + DAT_00911ae4))

static __forceinline int CheckRasterFormat(RwRaster *raster, RasterExt *ext, int format)
{
  switch (raster->cType & 7) {
  case 0:
  case 4:
    return DAT_007d4108->lpVtbl->CheckDeviceFormat(DAT_007d4108, DAT_007d410c, DAT_006181b0, DAT_00911f90, 0,
                                                   ext->cube ? 5 : 3, format) >= 0;
  case 5:
    if (ext->cube) {
      return FUN_004d39b0(format);
    }
    return FUN_004d38f0(format);
  }
  return 0;
}

// MATCH: 0x004d3b00 FUN_004d3b00
void FUN_004d3b00(RwRaster *raster)
{
  RwImage *image;
  RasterExt *ext;
  int format;
  image = FUN_004cdca0(raster->width, raster->height, 0x20);
  if (!image) {
    return;
  }
  ext = RASTEREXT(raster);
  FUN_004cdd60(image);
  FUN_004de1b0(image, raster->parent, 0);
  if (ext->alpha) {
    if (DAT_00911f94 > 0x10 && CheckRasterFormat(raster, ext, 0x15)) {
      format = 0x500;
    } else if (CheckRasterFormat(raster, ext, 0x1a)) {
      format = 0x300;
    } else {
      format = 0x100;
    }
  } else {
    if (DAT_00911f94 > 0x10 && CheckRasterFormat(raster, ext, 0x16)) {
      format = 0x600;
    } else {
      format = 0x200;
    }
  }
  FUN_004d3090(0, raster, 0);
  FUN_004d2340(0, raster, (raster->cType & 7) | format | 0x9000);
  FUN_004df120(raster, image, 0);
  FUN_004cdd00(image);
}
