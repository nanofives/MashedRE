// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWT-B 2026-11-12: 100% after declaring ext with its initializer before next (was 72.1%, regs 98.4%: DAT_00911ae4 load and lea placement). */
/* MATCH-S20 0x004d1d70 (h4): 72.1% (regs 98.4%). Walks the raster list at 0x007d46e0 (node: raster, next); for each
   top-level raster (parent == self) releases the D3D object in the plugin extension (raster + DAT_00911ae4) by
   cType: 0/4 when ext+0x18 is in 0x46..0x50, 5 always, 1 unless it is DAT_007d4114 (each also NULLs ext+0),
   2 releases ext+0x1c. What moved it: case order 0/4, 5, 1, 2 (source order of the blocks) and a separate next
   local (36 -> 72%). Residual: ours cross-jumps the identical Release + NULL tails of case 0/4 into case 1's copy;
   the original keeps three copies (with different vtable registers). Tried nested ifs, permuter 166 compiles (its
   77% variant moves the Release call: not equivalent, rejected). Class (b). */
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
typedef struct IUnk IUnk;
typedef struct IUnkVtbl {
  void *QueryInterface; void *AddRef; unsigned long (__stdcall *Release)(IUnk *self);
} IUnkVtbl;
struct IUnk { IUnkVtbl *lpVtbl; };
typedef struct RwRaster {
  struct RwRaster *parent; unsigned char *cpPixels; unsigned char *palette; int width; int height; int depth;
  int stride; short nOffsetX; short nOffsetY; unsigned char cType; unsigned char cFlags; unsigned char privateFlags;
  unsigned char cFormat;
} RwRaster;
typedef struct RasterExt {
  IUnk *texture; int field_4; int field_8; int field_c; int field_10; int field_14; int format; IUnk *surface;
} RasterExt;
typedef struct RasterLink { RwRaster *raster; struct RasterLink *next; } RasterLink;
MGLOBAL(RasterLink *, DAT_007d46e0, 0x007d46e0);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(IUnk *, DAT_007d4114, 0x007d4114);

// MATCH: 0x004d1d70 FUN_004d1d70
void FUN_004d1d70(void)
{
  RasterLink *cur = DAT_007d46e0;
  while (cur) {
    RwRaster *raster = cur->raster;
    RasterExt *ext = (RasterExt *)((unsigned char *)raster + DAT_00911ae4);
    RasterLink *next = cur->next;
    if (raster->parent == raster) {
      switch (raster->cType) {
      case 0:
      case 4:
        if (ext->texture && ext->format >= 0x46 && ext->format <= 0x50) {
          ext->texture->lpVtbl->Release(ext->texture);
          ext->texture = 0;
        }
        break;
      case 5:
        if (ext->texture) {
          ext->texture->lpVtbl->Release(ext->texture);
          ext->texture = 0;
        }
        break;
      case 1:
        if (ext->texture && ext->texture != DAT_007d4114) {
          ext->texture->lpVtbl->Release(ext->texture);
          ext->texture = 0;
        }
        break;
      case 2:
        if (ext->surface) {
          ext->surface->lpVtbl->Release(ext->surface);
        }
        break;
      }
    }
    cur = next;
  }
}
