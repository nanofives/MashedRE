// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cecb0 (h2): 79.6% (regs 79.6%). Raw 10.9%. RwImage copy/convert shape: equal depth = palette
   memcpy plus row memcpy loop; otherwise switch on (uint)(src->depth << 8 | dst->depth): 0x404/0x808/0x2020
   nothing, 0x408 row memcpy, 0x420/0x820 palette expand, default RWERROR(0x80000009) and dest = NULL (the original
   then runs the flag fix-up on NULL, kept as is). Every instruction matches. Residual (b): block layout only: the
   original places the shared flag fix-up tail after the default case; ours places it after the 0x420/0x820 loop.
   Tried all 6 case orders, default first, inverted outer if (36.3%), permuter (no gain). */
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
typedef struct { int flags; int width; int height; int depth; int stride; uchar *cpPixels; uint *palette; } RwImage;
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
int FUN_004d7ff0(int p0);
RwError *FUN_004d8480(RwError *p0);
// MATCH: 0x004cecb0 FUN_004cecb0
RwImage *FUN_004cecb0(RwImage *param_1,RwImage *param_2)
{
  RwError err;
  uchar *dst;
  uchar *src;
  int x, y;
  int rowBytes;
  int width;
  uint *pal;

  if (param_1->depth == param_2->depth) {
    if (param_1->palette && param_2->palette && param_2->depth <= 8) {
      memcpy(param_1->palette, param_2->palette, (1 << param_2->depth) * 4);
    }
    rowBytes = ((param_1->depth + 7) >> 3) * param_1->width;
    dst = param_1->cpPixels;
    src = param_2->cpPixels;
    for (y = 0; y < param_1->height; y++) {
      memcpy(dst, src, rowBytes);
      dst += param_1->stride;
      src += param_2->stride;
    }
  } else {
    pal = param_2->palette;
    width = param_1->width;
    src = param_2->cpPixels;
    dst = param_1->cpPixels;
    switch ((uint)((param_2->depth << 8) | param_1->depth)) {
    case 0x404:
    case 0x808:
    case 0x2020:
      break;
    case 0x408:
      for (y = param_1->height; y > 0; y--) {
        memcpy(dst, src, width);
        src += param_2->stride;
        dst += param_1->stride;
      }
      break;
    case 0x420:
    case 0x820:
      for (y = param_1->height; y > 0; y--) {
        for (x = 0; x < width; x++) {
          ((uint *)dst)[x] = pal[src[x]];
        }
        src += param_2->stride;
        dst += param_1->stride;
      }
      break;
    default:
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x80000009);
      FUN_004d8480(&err);
      param_1 = 0;
      break;
    }
  }
  param_1->flags &= ~2;
  param_1->flags |= param_2->flags & 2;
  return param_1;
}
