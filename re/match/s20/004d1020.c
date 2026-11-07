// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d1020 (h3): 42.1% (regs 57.6%, ops 77.3%); jump table at 0x004d15d4 past Ghidra's size. Raster clear-rect on the current raster 0x007d46c8 by cType: 2 camera (colour converted through FUN_004ddfb0/FUN_004ddd60 unless depth 32, FUN_004c7a20(0, 0x007d4118), FUN_004c79f0(0x007d4114), IDirect3DDevice9::Clear(1, rect, 1, colour, 1.0f, 0)); 0/4 normal/texture (GetLevelDesc on the parent's texture: D3DPOOL_DEFAULT -> GetSurfaceLevel or GetCubeMapSurface by the ext byte +9, ColorFill, Release; else lock with FUN_004cff20, fill 32/16-bit pixels or the nearest palette index for 8-bit, unlock FUN_004d0290, RWERROR 0x8000000d when the lock fails or the depth is unsupported); 5 camera texture (top raster surface, ColorFill); 1 z-buffer (FUN_004c79f0(top ext surface), Clear(1, rect, 2, 0, 1.0f, 0), restore); default RWERROR 0x8000000d. Ext = raster + DAT_00911ae4 (texture +0, palette +4, cube/face byte +9). From 22.1% (raw converter had unaff_* registers). Residual: the original packs the address-taken locals of every case (colour temps, surface, D3DRECT, D3DSURFACE_DESC, lock pointer, RwError) into a 0x48 frame with per-case slots; ours keeps them apart (0x5c function scope, 0x68 block-scoped per case), so every stack offset differs, plus the code is 585 vs 502 instructions (the 8-bit palette search and the depth-32 colour copy are laid out differently). Tried: function-scope and per-case block-scope locals. Ran out of budget (1460 B). Class (b). */
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
typedef struct RwRect { int x; int y; int w; int h; } RwRect;
typedef struct D3DRECT { int x1; int y1; int x2; int y2; } D3DRECT;
typedef struct D3DSURFACE_DESC { int Format; int Type; unsigned int Usage; int Pool; int MultiSampleType; unsigned int MultiSampleQuality; unsigned int Width; unsigned int Height; } D3DSURFACE_DESC;
typedef struct RwRaster {
  struct RwRaster *parent;
  unsigned char *cpPixels;
  unsigned char *palette;
  int width;
  int height;
  int depth;
  int stride;
  short nOffsetX;
  short nOffsetY;
  unsigned char cType;
  unsigned char cFlags;
  unsigned char privateFlags;
  unsigned char cFormat;
} RwRaster;
typedef struct ISurf { struct ISurfVtbl *lpVtbl; } ISurf;
typedef struct ISurfVtbl { void *QueryInterface; void *AddRef; unsigned long (__stdcall *Release)(ISurf *self); } ISurfVtbl;
typedef struct ITex { struct ITexVtbl *lpVtbl; } ITex;
typedef struct ITexVtbl {
  void *pad[17];
  long (__stdcall *GetLevelDesc)(ITex *self, unsigned int level, D3DSURFACE_DESC *desc);
  long (__stdcall *GetSurfaceLevel)(ITex *self, unsigned int level, ISurf **surf);
} ITexVtbl;
typedef struct ICubeVtbl {
  void *pad[18];
  long (__stdcall *GetCubeMapSurface)(ITex *self, unsigned int face, unsigned int level, ISurf **surf);
} ICubeVtbl;
typedef struct IDev { struct IDevVtbl *lpVtbl; } IDev;
typedef struct IDevVtbl {
  void *pad[35];
  long (__stdcall *ColorFill)(IDev *self, ISurf *surf, const D3DRECT *rect, unsigned int color);
  void *pad2[7];
  long (__stdcall *Clear)(IDev *self, unsigned int count, const D3DRECT *rects, unsigned int flags, unsigned int color, float z, unsigned int stencil);
} IDevVtbl;
typedef struct RasterExt { ITex *texture; unsigned char *palette; char pad; unsigned char cube; } RasterExt;
MGLOBAL(RwRaster *, DAT_007d46c8, 0x007d46c8);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(IDev *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_007d4114, 0x007d4114);
MGLOBAL(int, DAT_007d4118, 0x007d4118);
void FUN_004c79f0(int surface);
int FUN_004c7a20(int index, int surface);
int FUN_004cff20(unsigned char **pixels, RwRaster *raster, int flags);
int FUN_004d0290(unsigned char *pixels, RwRaster *raster, int flags);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
int FUN_004ddd60(unsigned int *out, unsigned int *in, int format);
int FUN_004ddfb0(unsigned int *out, unsigned int *in, int format);
#define EXT(r) ((RasterExt *)((char *)(r) + DAT_00911ae4))

// MATCH: 0x004d1020 FUN_004d1020
int FUN_004d1020(RwRect *rect, unsigned int pixelValue)
{
  RwRaster *top;
  RasterExt *ext;
  int x;
  int y;

  switch (DAT_007d46c8->cType) {
  case 2:
  {
    ISurf *surface;
    D3DRECT r;
    D3DSURFACE_DESC desc;
    unsigned int tmp;
    unsigned int color;
    unsigned char *pixels;
    int ok;
    if (DAT_007d46c8->depth == 32) {
      color = pixelValue;
    } else {
      FUN_004ddfb0(&tmp, &pixelValue, DAT_007d46c8->cFormat << 8);
      FUN_004ddd60(&color, &tmp, 0x500);
    }
    FUN_004c7a20(0, DAT_007d4118);
    FUN_004c79f0(DAT_007d4114);
    r.x1 = rect->x + DAT_007d46c8->nOffsetX;
    r.y1 = rect->y + DAT_007d46c8->nOffsetY;
    r.x2 = r.x1 + rect->w;
    r.y2 = r.y1 + rect->h;
    DAT_007d4110->lpVtbl->Clear(DAT_007d4110, 1, &r, 1, color, 1.0f, 0);
    return 1;
  }
  case 0:
  case 4:
  {
    ISurf *surface;
    D3DRECT r;
    D3DSURFACE_DESC desc;
    unsigned int tmp;
    unsigned int color;
    unsigned char *pixels;
    int ok;
    ext = EXT(DAT_007d46c8->parent);
    ext->texture->lpVtbl->GetLevelDesc(ext->texture, 0, &desc);
    if (desc.Pool == 0) {
      FUN_004ddfb0(&tmp, &pixelValue, DAT_007d46c8->cFormat << 8);
      FUN_004ddd60(&color, &tmp, 0x500);
      if (ext->cube & 0xf) {
        ((ICubeVtbl *)ext->texture->lpVtbl)->GetCubeMapSurface(ext->texture, ext->cube >> 4, 0, &surface);
      } else {
        ext->texture->lpVtbl->GetSurfaceLevel(ext->texture, 0, &surface);
      }
      r.x1 = rect->x + DAT_007d46c8->nOffsetX;
      r.y1 = rect->y + DAT_007d46c8->nOffsetY;
      r.x2 = r.x1 + rect->w;
      r.y2 = r.y1 + rect->h;
      DAT_007d4110->lpVtbl->ColorFill(DAT_007d4110, surface, &r, color);
      surface->lpVtbl->Release(surface);
      return 1;
    }
    ok = 0;
    if (FUN_004cff20(&pixels, DAT_007d46c8, 1)) {
      if (DAT_007d46c8->depth == 32) {
        for (y = 0; y < rect->h; y++) {
          unsigned int *p = (unsigned int *)(pixels + (rect->y + y) * DAT_007d46c8->stride) + rect->x;
          for (x = 0; x < rect->w; x++) {
            p[x] = pixelValue;
          }
        }
        ok = 1;
      } else if (DAT_007d46c8->depth == 16) {
        for (y = 0; y < rect->h; y++) {
          unsigned short *p = (unsigned short *)(pixels + (rect->y + y) * DAT_007d46c8->stride) + rect->x;
          for (x = 0; x < rect->w; x++) {
            p[x] = (unsigned short)pixelValue;
          }
        }
        ok = 1;
      } else if (DAT_007d46c8->depth == 8) {
        unsigned char *pal;
        unsigned int best;
        unsigned int dist;
        unsigned int i;
        unsigned char idx;
        int c0, c1, c2, c3;
        top = DAT_007d46c8;
        do {
          top = top->parent;
        } while (top != top->parent);
        c1 = ((unsigned char *)&pixelValue)[1];
        c2 = ((unsigned char *)&pixelValue)[2];
        pal = EXT(top)->palette;
        c0 = ((unsigned char *)&pixelValue)[0];
        c3 = ((unsigned char *)&pixelValue)[3];
        best = 0xffffffff;
        idx = 0;
        for (i = 0; i < 256; i++) {
          int d0 = pal[i * 4 + 0] - c0;
          int d1 = pal[i * 4 + 1] - c1;
          int d2 = pal[i * 4 + 2] - c2;
          int d3 = pal[i * 4 + 3] - c3;
          dist = d3 * d3 + d2 * d2 + d1 * d1 + d0 * d0;
          if (dist < best) {
            idx = (unsigned char)i;
            best = dist;
            if (!dist) {
              break;
            }
          }
        }
        for (y = 0; y < rect->h; y++) {
          unsigned char *p = pixels + (rect->y + y) * DAT_007d46c8->stride + rect->x;
          for (x = 0; x < rect->w; x++) {
            p[x] = idx;
          }
        }
        ok = 1;
      }
      FUN_004d0290(pixels, DAT_007d46c8, 1);
      if (ok) {
        return 1;
      }
    }
    {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x8000000d);
      FUN_004d8480(&err);
    }
    return 0;
  }
  case 5:
  {
    ISurf *surface;
    D3DRECT r;
    D3DSURFACE_DESC desc;
    unsigned int tmp;
    unsigned int color;
    unsigned char *pixels;
    int ok;
    if (DAT_007d46c8->depth == 32) {
      color = pixelValue;
    } else {
      FUN_004ddfb0(&tmp, &pixelValue, DAT_007d46c8->cFormat << 8);
      FUN_004ddd60(&color, &tmp, 0x500);
    }
    top = DAT_007d46c8;
    do {
      top = top->parent;
    } while (top != top->parent);
    ext = EXT(top);
    if (ext->cube & 0xf) {
      ((ICubeVtbl *)ext->texture->lpVtbl)->GetCubeMapSurface(ext->texture, ext->cube >> 4, 0, &surface);
    } else {
      ext->texture->lpVtbl->GetSurfaceLevel(ext->texture, 0, &surface);
    }
    r.x1 = rect->x + DAT_007d46c8->nOffsetX;
    r.y1 = rect->y + DAT_007d46c8->nOffsetY;
    r.x2 = r.x1 + rect->w;
    r.y2 = r.y1 + rect->h;
    DAT_007d4110->lpVtbl->ColorFill(DAT_007d4110, surface, &r, color);
    surface->lpVtbl->Release(surface);
    return 1;
  }
  case 1:
  {
    ISurf *surface;
    D3DRECT r;
    D3DSURFACE_DESC desc;
    unsigned int tmp;
    unsigned int color;
    unsigned char *pixels;
    int ok;
    top = DAT_007d46c8;
    do {
      top = top->parent;
    } while (top != top->parent);
    FUN_004c79f0(*(int *)EXT(top));
    r.x1 = rect->x + DAT_007d46c8->nOffsetX;
    r.y1 = rect->y + DAT_007d46c8->nOffsetY;
    r.x2 = r.x1 + rect->w;
    r.y2 = r.y1 + rect->h;
    DAT_007d4110->lpVtbl->Clear(DAT_007d4110, 1, &r, 2, 0, 1.0f, 0);
    FUN_004c79f0(DAT_007d4114);
    return 1;
  }
  default: {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x8000000d);
    FUN_004d8480(&err);
    return 0;
  }
  }
}
