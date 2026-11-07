// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004e02d0 (h1, by hand): best 67.8% exact, regs 74.9% (raw 44.0%). Image -> 32-bit raster pixel
   conversion. Depth 4/8: build a 256-entry palette (888: 0xff000000|rgb, 8888: argb, else the per-format converter
   picked by the switch on the native format at raster + DAT_00911ae4 + 0x18, alpha byte from 0x00911b00[format])
   and map the indices; depth 32: convert pixels directly. Native-ext +8 = alpha flag, also set on the parent when
   it differs. Converter switch is a local macro (#undef after); structure, switch tables, loop shapes and frame
   (0x410) match. Residual: register allocation in the pixel loops. The original evicts param_2 (image) from ebx
   to its stack home inside the loops and keeps y in ebp; 9178 keeps the image in ebx and spills y, which costs
   one extra temp in every packing expression. Tried: sequenced c = (c<<8)|x packing, the 0xffffff00 spelling,
   loop-var order, src/dst statement order, separate dst++. Permuter (895 compiles) reached 71.2% only by moving
   the pixel store after src += 4, which changes the bytes read, so it is not kept. Class (b). C++ ok. */
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
typedef struct FUN_004e02d0_raster FUN_004e02d0_raster;
struct FUN_004e02d0_raster {
  FUN_004e02d0_raster *parent;
  uchar *cpPixels;
  uchar *palette;
  int width;
  int height;
  int depth;
  int stride;
  short nOffsetX;
  short nOffsetY;
  uchar cType;
  uchar cFlags;
  uchar privateFlags;
  uchar cFormat;
};
typedef struct { int flags; int width; int height; int depth; int stride; uchar *cpPixels; uchar *palette; } FUN_004e02d0_image;
typedef struct { int pad00[2]; uchar alpha; uchar pad09[3]; int pad0c[3]; int format; } FUN_004e02d0_ext;
typedef uint (*FUN_004e02d0_conv)(uchar *rgba);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBALA(uint, DAT_00911b00, 128, 0x00911b00);
uint FUN_004df870(uchar *rgba);
uint FUN_004df8a0(uchar *rgba);
uint FUN_004df8d0(uchar *rgba);
uint FUN_004df910(uchar *rgba);
uint FUN_004df950(uchar *rgba);
uint FUN_004df980(uchar *rgba);
uint FUN_004df9b0(uchar *rgba);
uint FUN_004df9e0(uchar *rgba);
uint FUN_004dfa10(uchar *rgba);
uint FUN_004dfa40(uchar *rgba);
uint FUN_004dfa70(uchar *rgba);
uint FUN_004dfaa0(uchar *rgba);
uint FUN_004dfab0(uchar *rgba);
uint FUN_004dfb10(uchar *rgba);
uint FUN_004dfb60(uchar *rgba);
#define FUN_004e02d0_FIND(fn, format)            \
  fn = 0;                                        \
  switch (format) {                              \
  case 0x15: fn = FUN_004dfa40; break;           \
  case 0x16: fn = FUN_004df9e0; break;           \
  case 0x17: fn = FUN_004df8a0; break;           \
  case 0x18: fn = FUN_004df870; break;           \
  case 0x19: fn = FUN_004df8d0; break;           \
  case 0x1a: fn = FUN_004df910; break;           \
  case 0x1d: fn = FUN_004df950; break;           \
  case 0x1e: fn = FUN_004df980; break;           \
  case 0x20: case 0x3f: fn = FUN_004dfa70; break;\
  case 0x21: case 0x3e: fn = FUN_004dfa10; break;\
  case 0x32: fn = FUN_004dfab0; break;           \
  case 0x33: fn = FUN_004dfb10; break;           \
  case 0x3c: case 0x75: fn = FUN_004dfaa0; break;\
  case 0x3d: fn = FUN_004df9b0; break;           \
  case 0x51: fn = FUN_004dfb60; break;           \
  }
// MATCH: 0x004e02d0 FUN_004e02d0
void FUN_004e02d0(FUN_004e02d0_raster *param_1,FUN_004e02d0_image *param_2)

{
  FUN_004e02d0_ext *ext;
  FUN_004e02d0_conv fn;
  uchar alpha;
  uint pal[256];
  int x, y, i, n;
  uchar *src;
  uint *dst;

  ext = (FUN_004e02d0_ext *)((char *)param_1 + DAT_00911ae4);
  switch (param_2->depth) {
  case 4:
  case 8:
    if ((param_1->cFormat & 0xf) << 8 == 0x600) {
      n = 1 << param_2->depth;
      alpha = 0xff;
      for (i = 0; i < n; i++) {
        src = param_2->palette + i * 4;
        pal[i] = 0xff000000 | (src[0] << 16) | (src[1] << 8) | src[2];
      }
    }
    else if ((param_1->cFormat & 0xf) << 8 == 0x500) {
      n = 1 << param_2->depth;
      alpha = 0;
      for (i = 0; i < n; i++) {
        src = param_2->palette + i * 4;
        pal[i] = (src[3] << 24) | (src[0] << 16) | (src[1] << 8) | src[2];
      }
    }
    else {
      FUN_004e02d0_FIND(fn, ext->format);
      alpha = (uchar)DAT_00911b00[ext->format];
      for (i = 0; i < 1 << param_2->depth; i++) {
        pal[i] = fn(param_2->palette + i * 4);
      }
    }
    for (y = 0; y < param_1->height; y++) {
      dst = (uint *)(param_1->cpPixels + param_1->stride * y);
      src = param_2->cpPixels + param_2->stride * y;
      for (x = 0; x < param_1->width; x++) {
        *dst++ = pal[*src++];
      }
    }
    break;
  case 32:
    if ((param_1->cFormat & 0xf) << 8 == 0x600) {
      for (y = 0; y < param_1->height; y++) {
        src = param_2->cpPixels + param_2->stride * y;
        dst = (uint *)(param_1->cpPixels + param_1->stride * y);
        for (x = 0; x < param_1->width; x++) {
          *dst = 0xff000000 | (src[0] << 16) | (src[1] << 8) | src[2];
          src += 4;
          dst++;
        }
      }
      ext->alpha = 0;
      return;
    }
    if ((param_1->cFormat & 0xf) << 8 == 0x500) {
      for (y = 0; y < param_1->height; y++) {
        src = param_2->cpPixels + param_2->stride * y;
        dst = (uint *)(param_1->cpPixels + param_1->stride * y);
        for (x = 0; x < param_1->width; x++) {
          *dst = (src[3] << 24) | (src[0] << 16) | (src[1] << 8) | src[2];
          src += 4;
          dst++;
        }
      }
      goto has_alpha;
    }
    FUN_004e02d0_FIND(fn, ext->format);
    alpha = (uchar)DAT_00911b00[ext->format];
    for (y = 0; y < param_1->height; y++) {
      src = param_2->cpPixels + param_2->stride * y;
      dst = (uint *)(param_1->cpPixels + param_1->stride * y);
      for (x = 0; x < param_1->width; x++) {
        *dst++ = fn(src);
        src += 4;
      }
    }
    break;
  default:
    goto has_alpha;
  }
  if (alpha == 0xff) {
    ext->alpha = 0;
    return;
  }
has_alpha:
  ext->alpha = 1;
  if (param_1 != param_1->parent) {
    ((FUN_004e02d0_ext *)((char *)param_1->parent + DAT_00911ae4))->alpha = 1;
  }
}
#undef FUN_004e02d0_FIND
