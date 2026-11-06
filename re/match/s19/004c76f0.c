// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c76f0 (h6): 100.0%, regs 100.0%, relocheck OK, cxx ok. RwRasterGetNumLevels shape: typed RwRaster, cFormat & 0x80 gives test al,al; js, the RwGlobals+0xb8 slot read into a function-pointer local first (gives mov eax,[eax+0xb8]; call eax; the direct call through the slot was 50.0), the parameter slot is the out value (h4's 0x004c76c0 form). Raw g2s was 45.0. */
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
typedef int (*fp3)(void *, void *, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004c76f0 FUN_004c76f0
int FUN_004c76f0(RwRaster *raster)
{
  fp3 fn;
  if (!(raster->cFormat & 0x80)) {
    return 1;
  }
  fn = *(fp3 *)(DAT_007d3ff8 + 0xb8);
  if (fn(&raster, raster, 0)) {
    return (int)raster;
  }
  return -1;
}
