// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d0660 (h4): 64.9% (regs 64.9%). Raster render-combination dispatch: NULL raster -> 0; switch on
   the current raster's (0x007d46c8) cType and the source raster's cType & 7: FUN_004d0d00(raster, x, y) or
   FUN_004d07b0(raster, x, y, flags), else RWERROR(2, "SRC, DST Raster render combination..." 0x006182dc) -> 0.
   Same instruction count (74/74) and the same three jump tables; residual is block order: the original places the
   inner case 2 call and the shared final error before the case 0/4/5 FUN_004d07b0 call, ours keeps source order.
   Tried: errors inline in every case (55.8), final error after the switch with breaks (64.9), reordered inner cases
   (59.5), permuter 137 compiles no gain. Class (b). */
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
typedef struct RwError { int pluginID; int errorCode; } RwError;
MGLOBAL(RwRaster *, DAT_007d46c8, 0x007d46c8);
MGLOBALA(char, s_SRC__DST_Raster_render_combinati_006182dc, 4, 0x006182dc);
void FUN_004d07b0(RwRaster *raster, int x, int y, int flags);
int FUN_004d0d00(RwRaster *raster, int x, int y);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);

// MATCH: 0x004d0660 FUN_004d0660
int FUN_004d0660(RwRaster *raster, int x, int y, int flags)
{
  RwError err;
  if (!raster) {
    return 0;
  }
  switch (DAT_007d46c8->cType) {
  case 0:
  case 4:
    switch (raster->cType & 7) {
    case 0:
    case 2:
    case 4:
    case 5:
      if (!FUN_004d0d00(raster, x, y)) {
        err.pluginID = 1;
        err.errorCode = FUN_004d7ff0(2, s_SRC__DST_Raster_render_combinati_006182dc);
        FUN_004d8480(&err);
        return 0;
      }
      return 1;
    case 1:
      break;
    default:
      return 1;
    }
    break;
  case 2:
  case 5:
    switch (raster->cType & 7) {
    case 0:
    case 4:
    case 5:
      FUN_004d07b0(raster, x, y, flags);
      return 1;
    case 1:
      {
        err.pluginID = 1;
        err.errorCode = FUN_004d7ff0(2, s_SRC__DST_Raster_render_combinati_006182dc);
        FUN_004d8480(&err);
        return 0;
      }
    case 2:
      if (FUN_004d0d00(raster, x, y)) {
        return 1;
      }
      break;
    default:
      return 1;
    }
    break;
  }
  err.pluginID = 1;
  err.errorCode = FUN_004d7ff0(2, s_SRC__DST_Raster_render_combinati_006182dc);
  FUN_004d8480(&err);
  return 0;
}
