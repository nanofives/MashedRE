// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d3510 (h2): 55.8% (regs 75.3%). Raw 28.2%. D3D9 texture raster create shape: RwRasterCreate
   0x004c77c0, CheckDeviceFormat (+0x28) / CheckDepthStencilMatch (+0x30) for the render-target, depth (0x46..0x50)
   and managed cases, CreateTexture (+0x5c), default-pool list node, DXT1..DXT5 handling, format table 0x00911b00,
   P8 palette free list. Residual (a)/(b): the original keeps usage, pool and the display format in the dead
   parameter homes (param_4, param_2, param_1) with no frame; ours makes an 8-byte frame (same 9176 limitation as
   0x004cc5e0). The auto-mipmap check is materialized as a bool (neg/sbb/inc; test; je) in the original, not found
   (tried !call, == 0 into a local, __inline and __forceinline helpers). The CreateTexture join sits after the
   render-target case in the original. Permuter reached 61.2% only with edits that change behaviour (rejected). */
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
typedef unsigned short ushort;
typedef struct RwRaster RwRaster;
struct RwRaster {
  RwRaster *parent; uchar *cpPixels; uchar *palette; int width; int height; int depth; int stride;
  short nOffsetX; short nOffsetY; uchar cType; uchar cFlags; uchar privateFlags; uchar cFormat;
};
typedef struct { uchar entries[0x400]; uint globalindex; } D3D9Palette;
typedef struct {
  int *texture; D3D9Palette *palette; uchar alpha; uchar cube : 4; uchar face : 4; uchar automipmapgen : 4;
  uchar compressed : 4; uchar lockedMipLevel; int *lockedSurface; int lockedRect[2]; uint d3dFormat;
} RasExt;
typedef struct { uchar b[4]; } FmtInfo;
typedef struct RasNode { RwRaster *raster; struct RasNode *next; } RasNode;
typedef long (__stdcall *chkfmt_fn)(int *, uint, uint, uint, uint, uint, uint);
typedef long (__stdcall *chkds_fn)(int *, uint, uint, uint, uint, uint);
typedef long (__stdcall *ctex_fn)(int *, uint, uint, uint, uint, uint, uint, void *, void *);
typedef void *(*flalloc_fn)(void *, uint);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(uint, DAT_00911fac, 0x00911fac);
MGLOBAL(uint, DAT_00911f90, 0x00911f90);
MGLOBAL(int *, DAT_007d4108, 0x007d4108);
MGLOBAL(uint, DAT_006181b0, 0x006181b0);
MGLOBAL(uint, DAT_007d410c, 0x007d410c);
MGLOBAL(uint, DAT_00912108, 0x00912108);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(void *, DAT_007d46e4, 0x007d46e4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(RasNode *, DAT_007d46e0, 0x007d46e0);
MGLOBAL(void *, DAT_007d46dc, 0x007d46dc);
MGLOBAL(int, DAT_007d46cc, 0x007d46cc);
MGLOBAL(ushort *, DAT_007d46d8, 0x007d46d8);
MGLOBAL(uint, DAT_007d46d4, 0x007d46d4);
MGLOBALA(FmtInfo, DAT_00911b00, 0x80, 0x00911b00);
void FUN_004c7650(RwRaster *p0);
RwRaster *FUN_004c77c0(int p0, int p1, int p2, uint p3);
void *FUN_004cc7f0(int p0, int p1, int p2, int p3);
#define EXT(r) ((RasExt *)((uchar *)(r) + DAT_00911ae4))
// MATCH: 0x004d3510 FUN_004d3510
RwRaster *FUN_004d3510(int param_1,int param_2,uint param_3,uint param_4)
{
  RwRaster *raster;
  RasExt *ext;
  uint usage;
  uint pool;
  uint displayFormat;

  if ((param_4 & 7) == 0 || (param_4 & 7) == 1) {
    param_4 &= ~0x9000;
  }
  raster = FUN_004c77c0(param_1, param_2, 0, param_4 | 0x80);
  if (raster == 0) return 0;
  ext = EXT(raster);
  if ((param_4 & 0x9000) == 0x9000 && (DAT_00911fac & 0x40000000) &&
      (*(chkfmt_fn *)(*DAT_007d4108 + 0x28))(DAT_007d4108, DAT_007d410c, DAT_006181b0, DAT_00911f90, 0x400, 3, param_3) == 0) {
    ext->automipmapgen = 1;
  }
  usage = ext->automipmapgen ? 0x400 : 0;
  if ((param_4 & 5) == 5) {
    displayFormat = DAT_00911f90;
    pool = 0;
    if ((*(chkfmt_fn *)(*DAT_007d4108 + 0x28))(DAT_007d4108, DAT_007d410c, DAT_006181b0, displayFormat, 1, 3, param_3) < 0) goto fail;
    if ((*(chkds_fn *)(*DAT_007d4108 + 0x30))(DAT_007d4108, DAT_007d410c, DAT_006181b0, displayFormat, param_3, DAT_00912108) < 0) goto fail;
    usage |= 1;
  } else if (param_3 >= 0x46 && param_3 <= 0x50) {
    displayFormat = DAT_00911f90;
    pool = 0;
    if ((*(chkfmt_fn *)(*DAT_007d4108 + 0x28))(DAT_007d4108, DAT_007d410c, DAT_006181b0, displayFormat, 2, 3, param_3) < 0) goto fail;
    if ((*(chkds_fn *)(*DAT_007d4108 + 0x30))(DAT_007d4108, DAT_007d410c, DAT_006181b0, displayFormat, displayFormat, param_3) < 0) goto fail;
    usage |= 2;
  } else {
    pool = 1;
    if ((*(chkfmt_fn *)(*DAT_007d4108 + 0x28))(DAT_007d4108, DAT_007d410c, DAT_006181b0, DAT_00911f90, 0, 3, param_3) < 0) goto fail;
  }
  if ((*(ctex_fn *)(*DAT_007d4110 + 0x5c))(DAT_007d4110, raster->width, raster->height, (~param_4 >> 15) & 1,
        usage, param_3, pool, ext, 0) < 0) {
fail:
    FUN_004c7650(raster);
    return 0;
  }
  if (pool == 0) {
    RasNode *node = (RasNode *)(*(flalloc_fn *)(DAT_007d3ff8 + 0x118))(DAT_007d46e4, 0x30411);
    node->raster = raster;
    node->next = DAT_007d46e0;
    DAT_007d46e0 = node;
  }
  raster->cFlags &= ~0x80;
  raster->cFormat &= 0xf0;
  ext->d3dFormat = param_3;
  if (param_3 >= 0x31545844 && param_3 <= 0x35545844) {
    ext->compressed = 1;
    if (param_3 == 0x31545844) {
      ext->alpha = 0;
      raster->cFormat |= 2;
    } else {
      ext->alpha = 1;
      raster->cFormat |= 3;
    }
    raster->depth = 16;
    return raster;
  }
  ext->compressed = 0;
  if (param_3 < 0x80) {
    ext->alpha = DAT_00911b00[param_3].b[0];
    raster->depth = DAT_00911b00[param_3].b[1];
    raster->cFormat |= DAT_00911b00[param_3].b[3];
    if (param_3 == 0x29) {
      if (DAT_007d46dc == 0) {
        DAT_007d46dc = FUN_004cc7f0(0x404, 0x40, 4, 0x30411);
      }
      ext->palette = (D3D9Palette *)(*(flalloc_fn *)(DAT_007d3ff8 + 0x118))(DAT_007d46dc, 0x30411);
      if (ext->palette) {
        if (DAT_007d46cc) {
          DAT_007d46cc--;
          ext->palette->globalindex = DAT_007d46d8[DAT_007d46cc];
        } else {
          ext->palette->globalindex = DAT_007d46d4++;
        }
      }
    }
  }
  return raster;
}
