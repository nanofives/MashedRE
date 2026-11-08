// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d1d20 (h4): 100% (regs 100%). Raster type/format setup: cType = flags & 7, cFlags = flags & 0xf8,
   format = FUN_004d1640(cType, flags) >> 8 into cFormat, depth from the 8-byte table at 0x005d8bf4. What moved it:
   pass raster->cType (just stored) to the callee instead of a byte local (56% -> 100%). */
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
  int stride; short nOffsetX; short nOffsetY; unsigned char cType; unsigned char cFlags; unsigned char privateFlags;
  unsigned char cFormat;
} RwRaster;
typedef struct FormatInfo { unsigned char depth; unsigned char pad[7]; } FormatInfo;
MGLOBALA(FormatInfo, DAT_005d8bf4, 16, 0x005d8bf4);
unsigned int FUN_004d1640(int type, int flags);

// MATCH: 0x004d1d20 FUN_004d1d20
int FUN_004d1d20(RwRaster *raster, int flags)
{
  unsigned int format;
  raster->cType = (unsigned char)(flags & 7);
  raster->cFlags = (unsigned char)(flags & 0xf8);
  format = FUN_004d1640(raster->cType, flags) >> 8;
  raster->cFormat = (unsigned char)format;
  raster->depth = DAT_005d8bf4[format & 0xf].depth;
  return 1;
}
