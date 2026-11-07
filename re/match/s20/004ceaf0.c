// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004ceaf0 (h3): 100% (regs 100%). RwImage copy: palette memcpy ((1 << src->depth) * sizeof(RwRGBA)) when both palettes exist and src->depth <= 8, then memcpy each row of ((dst->depth + 7) >> 3) * dst->width bytes for dst->height rows, stepping each image's stride; returns 1. From 15.1%: RwImage struct, memcpy intrinsic, the shift-then-multiply palette size. Relocheck OK (no globals). */
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

// MATCH: 0x004ceaf0 FUN_004ceaf0
int FUN_004ceaf0(RwImage *param_1, RwImage *param_2)
{
  unsigned char *dst;
  unsigned char *src;
  int bpr;
  int y;

  if (param_1->palette && param_2->palette && param_2->depth <= 8) {
    memcpy(param_1->palette, param_2->palette, (1 << param_2->depth) * sizeof(RwRGBA));
  }
  bpr = ((param_1->depth + 7) >> 3) * param_1->width;
  src = param_2->cpPixels;
  dst = param_1->cpPixels;
  for (y = 0; y < param_1->height; y++) {
    memcpy(dst, src, bpr);
    dst += param_1->stride;
    src += param_2->stride;
  }
  return 1;
}
