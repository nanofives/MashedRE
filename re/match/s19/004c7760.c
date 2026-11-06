// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c7760 (h6): 100.0%, regs 100.0%, relocheck OK, cxx ok. RwRasterSubRaster shape: typed RwRaster/RwRect, cFlags & 0x80 test gives test al,al; jns (the raw char < 0 gave jge, 97.2%), typed fn pointer RwGlobals+0x78 */
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
typedef struct RwRect { int x; int y; int w; int h; } RwRect;
typedef int (*RwStdFunc)(void *, void *, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004c7760 FUN_004c7760
RwRaster *FUN_004c7760(RwRaster *raster, RwRaster *parent, RwRect *rect)
{
  if (raster->cFlags & 0x80) {
    raster->width = rect->w;
    raster->height = rect->h;
    raster->nOffsetX = (short)(parent->nOffsetX + rect->x);
    raster->nOffsetY = (short)(parent->nOffsetY + rect->y);
    if ((*(RwStdFunc *)(DAT_007d3ff8 + 0x78))(raster, parent, 0)) {
      raster->parent = parent->parent;
      return raster;
    }
  }
  return 0;
}
