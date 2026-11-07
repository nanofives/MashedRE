// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cefd0 (h4): 100% (regs 100%), relocheck OK. RwImageGammaCorrect shape: depth 4/8 gamma-corrects
   the palette (1 << depth entries), depth 32 every row of pixels, else RWERROR 0x80000008; NULL palette/pixels ->
   RWERROR 0x80000016. Gamma table = RwGlobals module (DAT_007d4628) + 0xc. What moved it: the per-pixel loop as a
   macro (both loops are the same code, base reloaded per row), separate src/dst pointers so alpha = alpha survives,
   palette read before 1 << depth. */
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
typedef struct RwRGBA { unsigned char red; unsigned char green; unsigned char blue; unsigned char alpha; } RwRGBA;
typedef struct RwImage {
  int flags; int width; int height; int depth; int stride; unsigned char *cpPixels; RwRGBA *palette;
} RwImage;
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct ImageGlobals { int pad[3]; unsigned char gammaTable[256]; } ImageGlobals;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4628, 0x007d4628);
int FUN_004d7ff0(int code);
RwError *FUN_004d8480(RwError *err);
#define IMGGLOBAL(v) (((ImageGlobals *)(DAT_007d3ff8 + DAT_007d4628))->v)

#define GAMMACORRECTPIXELS(_p, _n, _i, _g)   _g = IMGGLOBAL(gammaTable);   for (_i = 0; _i != (_n); _i++) {     RwRGBA *_s = (_p) + _i;     RwRGBA *_d = (_p) + _i;     _d->red = _g[_s->red];     _d->green = _g[_s->green];     _d->blue = _g[_s->blue];     _d->alpha = _s->alpha;   }

// MATCH: 0x004cefd0 FUN_004cefd0
RwImage *FUN_004cefd0(RwImage *image)
{
  switch (image->depth) {
  case 4:
  case 8: {
    int i;
    unsigned char *g;
    RwRGBA *pal = image->palette;
    int n = 1 << image->depth;
    if (!pal) {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x80000016);
      FUN_004d8480(&err);
      return 0;
    }
    GAMMACORRECTPIXELS(pal, n, i, g);
    break;
  }
  case 32: {
    int y;
    int x;
    unsigned char *g;
    unsigned char *pix = image->cpPixels;
    int width = image->width;
    int height = image->height;
    if (!pix) {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x80000016);
      FUN_004d8480(&err);
      return 0;
    }
    for (y = 0; y < height; y++) {
      RwRGBA *p = (RwRGBA *)pix;
      GAMMACORRECTPIXELS(p, width, x, g);
      pix += image->stride;
    }
    break;
  }
  default: {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x80000008);
    FUN_004d8480(&err);
    return 0;
  }
  }
  image->flags |= 2;
  return image;
}
