// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004dfbc0 (h3): best 56.6% (regs 73.7%, ops 71.1%, 649/657 ins). Raw 8.9%. Raster-from-image conversion: when
   the raster's format nibble at +0x23 is not 4, an 8/4-bit image is copied (one memcpy when strides match,
   else per row, plus the palette unless ext+0xb is set), any other image is quantized (FUN_004cdca0 image,
   FUN_004daa40 state, FUN_004d9050, FUN_004d9530 to 1 << depth colours, FUN_004da4f0, FUN_004daaf0, the
   0x600 alpha check through FUN_004dee40) and the function recurses; format nibble 4 converts by D3D format
   at ext+0x18 (0x1b, 0x34, 0x1c, default) through a 256-entry LUT for palettised images or per 32-bit
   pixel. What moved it: '!= 4' and 'quantize' branches first (20 -> 55%), row pointer order (permuter).
   Residual: frame 0x410c vs 0x4108 (one more row counter on the stack in the 0x34/default 32-bit loops),
   register choice for the row counter and width loads in most loops. Out of budget. Class (b). */
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
typedef struct Ras_fbc0 { int f0; unsigned char *pixels; unsigned char *palette; int width; int height; int depth; int stride; char pad[7]; unsigned char fmt; } Ras_fbc0;
typedef struct Img_fbc0 { int f0; int width; int height; int depth; int stride; unsigned char *pixels; unsigned char *palette; } Img_fbc0;
typedef struct Ext_fbc0 { int f0; int f4; unsigned char alpha; char p9[2]; unsigned char palShared; int fc; int f10; int f14; int d3dfmt; } Ext_fbc0;
typedef struct Q_fbc0 { int v[0x1001]; } Q_fbc0;
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
Img_fbc0 *FUN_004cdca0(int width, int height, int depth);
Img_fbc0 *FUN_004cdd60(Img_fbc0 *img);
int FUN_004cdd00(Img_fbc0 *img);
int FUN_004daa40(Q_fbc0 *q);
void FUN_004d9050(Q_fbc0 *q, Img_fbc0 *img, float weight);
int FUN_004d9530(unsigned char *pal, int n, Q_fbc0 *q);
void FUN_004da4f0(unsigned char *pixels, int stride, int depth, int zero, Q_fbc0 *q, Img_fbc0 *src);
void FUN_004daaf0(Q_fbc0 *q);
unsigned int FUN_004dee40(Img_fbc0 *img);
void FUN_004dfbc0(Ras_fbc0 *param_1, Img_fbc0 *param_2);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
// MATCH: 0x004dfbc0 FUN_004dfbc0
void FUN_004dfbc0(Ras_fbc0 *param_1, Img_fbc0 *param_2)
{
  Ext_fbc0 *ext = (Ext_fbc0 *)((char *)param_1 + DAT_00911ae4);
  unsigned char lut[256];
  Q_fbc0 q;
  int x, y, i, n;
  unsigned char *s;
  unsigned char *d;
  if ((param_1->fmt & 0xf) != 4) {
    if (param_2->depth != 4 && param_2->depth != 8) {
    int depth = param_1->depth;
    Img_fbc0 *img = FUN_004cdca0(param_2->width, param_2->height, depth);
    if (img != 0) {
      FUN_004cdd60(img);
      if (FUN_004daa40(&q)) {
        FUN_004d9050(&q, param_2, 1.0f);
        FUN_004d9530(img->palette, 1 << depth, &q);
        FUN_004da4f0(img->pixels, img->stride, img->depth, 0, &q, param_2);
        FUN_004daaf0(&q);
        if (ext->alpha && (FUN_004dee40(img) & 0xf00) == 0x600) {
          param_1->fmt = (param_1->fmt & 0xfe) | 6;
          ext->alpha = 0;
        }
        FUN_004dfbc0(param_1, img);
        FUN_004cdd00(img);
      }
    }
      } else {
    if (param_2->stride == param_1->stride && param_1->width == param_1->stride) {
      memcpy(param_1->pixels, param_2->pixels, param_1->height * param_1->width);
    } else {
      for (y = 0; y < param_1->height; y++) {
        memcpy(param_1->pixels + param_1->stride * y, param_2->pixels + param_2->stride * y, param_1->width);
      }
    }
    if (ext->palShared == 0 && param_1->palette != 0) {
      memcpy(param_1->palette, param_2->palette, (1 << param_2->depth) * 4);
    }
    }
  } else {
    if (ext->d3dfmt == 0x1b) {
      if (param_2->depth == 4 || param_2->depth == 8) {
        n = 1 << param_2->depth;
        s = param_2->palette;
        for (i = 0; i < n; i++) {
          lut[i] = (s[0] & 0xe0) | ((s[1] & 0xe0) >> 3) | (s[2] >> 6);
          s += 4;
        }
        for (y = 0; y < param_1->height; y++) {
          d = param_1->pixels + param_1->stride * y;
          s = param_2->pixels + param_2->stride * y;
          for (x = 0; x < param_1->width; x++) {
            *d++ = lut[*s++];
          }
        }
      } else {
        for (y = 0; y < param_1->height; y++) {
          s = param_2->pixels + param_2->stride * y;
          d = param_1->pixels + param_1->stride * y;
          for (x = 0; x < param_1->width; x++) {
            *d++ = (s[0] & 0xe0) | ((s[1] & 0xe0) >> 3) | (s[2] >> 6);
            s += 4;
          }
        }
      }
    } else if (ext->d3dfmt == 0x34) {
      if (param_2->depth == 4 || param_2->depth == 8) {
        n = 1 << param_2->depth;
        s = param_2->palette;
        for (i = 0; i < n; i++) {
          lut[i] = (unsigned char)((((s[1] * 59 + s[2] * 11 + s[0] * 30) / 100) >> 4) | (s[3] & 0xf0));
          s += 4;
        }
        for (y = 0; y < param_1->height; y++) {
          d = param_1->pixels + param_1->stride * y;
          s = param_2->pixels + param_2->stride * y;
          for (x = 0; x < param_1->width; x++) {
            *d++ = lut[*s++];
          }
        }
      } else {
        for (y = 0; y < param_1->height; y++) {
          d = param_1->pixels + param_1->stride * y;
          s = param_2->pixels + param_2->stride * y;
          for (x = 0; x < param_1->width; x++) {
            *d++ = (unsigned char)((((s[1] * 59 + s[2] * 11 + s[0] * 30) / 100) >> 4) | (s[3] & 0xf0));
            s += 4;
          }
        }
      }
    } else if (ext->d3dfmt == 0x1c) {
      if (param_2->depth == 4 || param_2->depth == 8) {
        n = 1 << param_2->depth;
        s = param_2->palette;
        for (i = 0; i < n; i++) {
          lut[i] = s[3];
          s += 4;
        }
        for (y = 0; y < param_1->height; y++) {
          d = param_1->pixels + param_1->stride * y;
          s = param_2->pixels + param_2->stride * y;
          for (x = 0; x < param_1->width; x++) {
            *d++ = lut[*s++];
          }
        }
      } else {
        for (y = 0; y < param_1->height; y++) {
          s = param_2->pixels + param_2->stride * y;
          d = param_1->pixels + param_1->stride * y;
          for (x = 0; x < param_1->width; x++) {
            *d++ = s[3];
            s += 4;
          }
        }
      }
    } else {
      if (param_2->depth == 4 || param_2->depth == 8) {
        n = 1 << param_2->depth;
        s = param_2->palette;
        for (i = 0; i < n; i++) {
          lut[i] = (unsigned char)((((unsigned int)(s[1] * 59 + s[2] * 11 + s[0] * 30) / 100) * s[3]) / 255);
          s += 4;
        }
        for (y = 0; y < param_1->height; y++) {
          d = param_1->pixels + param_1->stride * y;
          s = param_2->pixels + param_2->stride * y;
          for (x = 0; x < param_1->width; x++) {
            *d++ = lut[*s++];
          }
        }
      } else {
        for (y = 0; y < param_1->height; y++) {
          d = param_1->pixels + param_1->stride * y;
          s = param_2->pixels + param_2->stride * y;
          for (x = 0; x < param_1->width; x++) {
            *d++ = (unsigned char)((((unsigned int)(s[1] * 59 + s[2] * 11 + s[0] * 30) / 100) * s[3]) / 255);
            s += 4;
          }
        }
      }
    }
  }
}
