// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cf4a0 (h4): 74.3% (regs 74.3%). RwTextureStreamWrite shape: texture chunk header (id 6, size from
   the 0x004cf460 sum, version 0x37002, build 10), struct header (1, 4), one dword = (raster && !(cFormat & 0x10) ? 1
   : 0) << 16 | filterAddressing & 0xffff, name and mask strings, plugin data (0x00618138); NULL on any failure.
   Residual: the original gives every failed check its own pop/ret epilogue returning eax (already 0); 9176 here
   tail-merges them into one xor eax,eax block (same as S19's 0x004c18c0). Also the mip flag branch layout.
   Tried: return of the call result, nested byte flag / inline ?: (67.9 -> 74.3), flags /GS, /Oy-, /O1, permuter
   124 compiles no gain. Class (b). */
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
typedef struct RwTexture {
  RwRaster *raster; void *dict; void *lInDictionary[2]; char name[32]; char mask[32]; unsigned int filterAddressing;
} RwTexture;
MGLOBALA(char, DAT_00618138, 4, 0x00618138);
int FUN_004cc580(void *stream, int type, int size, int version, int buildNum);
void *FUN_004cbe80(void *stream, const void *buffer, int length);
int FUN_004d8770(const char *string);
void *FUN_004d87a0(const char *string, void *stream);
int FUN_004e1ce0(void *reg, const void *object);
void *FUN_004e1d20(void *reg, void *stream, const void *object);

// MATCH: 0x004cf4a0 FUN_004cf4a0
RwTexture *FUN_004cf4a0(RwTexture *texture, void *stream)
{
  unsigned int data;
  int size;
  size = 4 + 0xc;
  size += FUN_004d8770(texture->name) + 0xc;
  size += FUN_004d8770(texture->mask) + 0xc;
  size += FUN_004e1ce0(DAT_00618138, texture) + 0xc;
  if (!FUN_004cc580(stream, 6, size, 0x37002, 10)) {
    return 0;
  }
  if (!FUN_004cc580(stream, 1, 4, 0x37002, 10)) {
    return 0;
  }
  data = ((unsigned int)(unsigned char)((texture->raster && !(texture->raster->cFormat & 0x10)) ? 1 : 0) << 16) |
         (texture->filterAddressing & 0xffff);
  if (!FUN_004cbe80(stream, &data, 4)) {
    return 0;
  }
  if (!FUN_004d87a0(texture->name, stream)) {
    return 0;
  }
  if (!FUN_004d87a0(texture->mask, stream)) {
    return 0;
  }
  if (!FUN_004e1d20(DAT_00618138, stream, texture)) {
    return 0;
  }
  return texture;
}
