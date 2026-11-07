// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004ceba0 (h3): 100% (regs 100%). Palettised -> dst conversion: switch on (unsigned)(src->depth << 8 | dst->depth): 0x420/0x820 palette lookup per pixel into 32-bit rows, 0x408 memcpy width bytes per row, 0x404/0x808/0x2020 nothing, default RWERROR(0x80000009) and 0; else 1. From 35.2%: RwImage struct, case order, unsigned switch value (ja), and the RwError local scoped inside the default block (RWERROR-macro style: lets the 0x408 row counter share its slot, sub esp 8 not 0xc). Relocheck OK. */
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
void *memcpy(void *d, const void *s, unsigned int n);
#pragma intrinsic(memcpy)
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct RwRGBA { unsigned char red; unsigned char green; unsigned char blue; unsigned char alpha; } RwRGBA;
typedef struct RwImage {
  int flags;
  int width;
  int height;
  int depth;
  int stride;
  unsigned char *cpPixels;
  RwRGBA *palette;
} RwImage;
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);

// MATCH: 0x004ceba0 FUN_004ceba0
int FUN_004ceba0(RwImage *param_1, RwImage *param_2)
{
  unsigned char *src;
  RwRGBA *dst;
  RwRGBA *pal;
  int width;
  int height;
  int x;

  src = param_2->cpPixels;
  width = param_1->width;
  pal = param_2->palette;
  dst = (RwRGBA *)param_1->cpPixels;
  height = param_1->height;
  switch ((unsigned int)((param_2->depth << 8) | param_1->depth)) {
  case 0x420:
  case 0x820:
    while (height-- > 0) {
      for (x = 0; x < width; x++) {
        dst[x] = pal[src[x]];
      }
      src += param_2->stride;
      dst = (RwRGBA *)((unsigned char *)dst + param_1->stride);
    }
    break;
  case 0x408:
    while (height-- > 0) {
      memcpy(dst, src, width);
      src += param_2->stride;
      dst = (RwRGBA *)((unsigned char *)dst + param_1->stride);
    }
    break;
  case 0x404:
  case 0x808:
  case 0x2020:
    break;
  default: {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x80000009);
    FUN_004d8480(&err);
    return 0;
  }
  }
  return 1;
}
