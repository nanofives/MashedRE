// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d2c80 (h2): 100% (regs 100%). Raw converter did not compile (Ghidra stack garbage). Hand-written
   from odis: RwRaster / raster-extension structs, D3D9 EvictManagedResources (+0x14), CreateCubeTexture (+0x64) /
   CreateTexture (+0x5c) through __stdcall pointer typedefs, RWERROR with 2 args (0x004d7ff0 is variadic: code 2 +
   message). Moves: whole body under if ((fmt & 0x6000) == 0) with the palettized error after it (tail-merged with
   the out-of-video-memory error), levels = (~fmt >> 15) & 1 (the ?: form gave 98.7%). Residual: none. relocheck OK. */
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
typedef struct { int pluginID; int errorCode; } RwError;
typedef struct { int d3dFormat; uchar b4; uchar alpha; uchar pad[2]; } FmtEnt;
typedef struct { uchar b[4]; } FmtInfo;
typedef struct {
  void *parent; uchar *cpPixels; uchar *palette; int width; int height; int depth; int stride;
  short nOffsetX; short nOffsetY; uchar cType; uchar cFlags; uchar privateFlags; uchar cFormat;
} RwRaster;
typedef struct {
  void *texture; void *palette; uchar alpha; uchar cube; uchar automipmap; uchar compressed;
  int pad0c; int pad10; int pad14; int d3dFormat;
} RasExt;
typedef long (__stdcall *evict_fn)(int *);
typedef long (__stdcall *ctex_fn)(int *, uint, uint, uint, uint, int, int, void *, void *);
typedef long (__stdcall *ccube_fn)(int *, uint, uint, uint, int, int, void *, void *);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_009120e8, 0x009120e8);
MGLOBAL(int, DAT_00911f94, 0x00911f94);
MGLOBALA(FmtEnt, DAT_005d8bf0, 16, 0x005d8bf0);
MGLOBALA(FmtInfo, DAT_00911b00, 0x125, 0x00911b00);
MGLOBALA(char, s_rwRASTERTYPECAMERATEXTURE_can_no_00618390, 49, 0x00618390);
MGLOBALA(char, s_Failed_to_create_raster__006183c4, 25, 0x006183c4);
MGLOBALA(char, s_Out_of_video_memory__Failed_to_c_006183e0, 46, 0x006183e0);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *p0);
void FUN_004ddcb0(int *p0, int *p1, uint p2);
// MATCH: 0x004d2c80 FUN_004d2c80
int FUN_004d2c80(RwRaster *param_1,RasExt *param_2)
{
  uint fmt;
  uint levels;
  long hr;
  RwError err;
  fmt = (uint)param_1->cFormat << 8;
  if ((fmt & 0x6000) == 0) {
    param_2->d3dFormat = DAT_005d8bf0[(fmt & 0xf00) >> 8].d3dFormat;
    param_2->alpha = DAT_005d8bf0[(fmt & 0xf00) >> 8].alpha;
    FUN_004ddcb0(&param_1->width, &param_1->height, param_1->cType | fmt);
    (*(evict_fn *)(*DAT_007d4110 + 0x14))(DAT_007d4110);
    levels = (~fmt >> 15) & 1;
    if (param_2->cube & 0xf) {
      hr = (*(ccube_fn *)(*DAT_007d4110 + 0x64))(DAT_007d4110, param_1->width, levels,
            ((param_2->automipmap & 0xf) ? 0x400 : 0) | 1, param_2->d3dFormat, 0, param_2, 0);
    } else {
      hr = (*(ctex_fn *)(*DAT_007d4110 + 0x5c))(DAT_007d4110, param_1->width, param_1->height, levels,
            ((param_2->automipmap & 0xf) ? 0x400 : 0) | 1, param_2->d3dFormat, 0, param_2, 0);
    }
    if (hr == (long)0x8876017c) {
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(2, s_Out_of_video_memory__Failed_to_c_006183e0);
      FUN_004d8480(&err);
      return 0;
    }
    if (hr < 0) {
      if (param_2->cube & 0xf) {
        hr = (*(ccube_fn *)(*DAT_007d4110 + 0x64))(DAT_007d4110, param_1->width, levels,
              (param_2->automipmap & 0xf) ? 0x400 : 0, DAT_009120e8, 0, param_2, 0);
      } else {
        hr = (*(ctex_fn *)(*DAT_007d4110 + 0x5c))(DAT_007d4110, param_1->width, param_1->height, levels,
              (param_2->automipmap & 0xf) ? 0x400 : 0, DAT_009120e8, 0, param_2, 0);
      }
      if (hr < 0) {
        err.pluginID = 1;
        err.errorCode = FUN_004d7ff0(2, s_Failed_to_create_raster__006183c4);
        FUN_004d8480(&err);
        return 0;
      }
      param_1->cFormat = DAT_00911b00[DAT_009120e8].b[3];
      param_1->depth = DAT_00911f94;
    }
    return 1;
  }
  err.pluginID = 1;
  err.errorCode = FUN_004d7ff0(2, s_rwRASTERTYPECAMERATEXTURE_can_no_00618390);
  FUN_004d8480(&err);
  return 0;
}
