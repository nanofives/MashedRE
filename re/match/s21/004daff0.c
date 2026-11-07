// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004daff0 (h2): 77.8% (regs 77.8%, ops 87.8%). RwImage 16.16 box-filter span: averages the RGBA
   pixels of row y>>16 between x0 and x1 (16.16) into an RwRGBAReal, partial first/last pixels weighted by their
   coverage (* 0x005d8d88 = 1/65536), colours / 255 (0x005ceb90), result * (0x005cc320 / width). Image +0x10
   stride, +0x14 pixels. Written with RW's RwRGBARealFromRwRGBA / Scale / Add macro shapes (raw form 30.4%).
   Residual: x87 operand order in every scale and add (`fld m; fmul st(1)` / `fld m; fadd st(i)` against our
   `fld st(0); fmul m` / `fadd m`), and the width kept in a stack slot (`fst [esp+0x24]`, reloaded for the final
   divide) against our register copy. Swapping the macro operand order (s * a, b + a) changes nothing.
   Class (a): the S19 x87 operand-order class; no permuter time spent per the brief. */
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
typedef struct RwRGBA { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct RwRGBAReal { float red, green, blue, alpha; } RwRGBAReal;
typedef struct RwImage { int flags; int width; int height; int depth; int stride; unsigned char *cpPixels;
  RwRGBA *palette; } RwImage;
MGLOBAL(float, DAT_005d8d88, 0x005d8d88);
MGLOBAL(float, DAT_005ceb90, 0x005ceb90);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
#define RwRGBARealFromRwRGBA(o, i) \
  { (o)->red = (float)(i)->red * DAT_005ceb90; (o)->green = (float)(i)->green * DAT_005ceb90; \
    (o)->blue = (float)(i)->blue * DAT_005ceb90; (o)->alpha = (float)(i)->alpha * DAT_005ceb90; }
#define RwRGBARealScale(o, a, s) \
  { (o)->red = (a)->red * (s); (o)->green = (a)->green * (s); (o)->blue = (a)->blue * (s); \
    (o)->alpha = (a)->alpha * (s); }
#define RwRGBARealAdd(o, a, b) \
  { (o)->red = (a)->red + (b)->red; (o)->green = (a)->green + (b)->green; \
    (o)->blue = (a)->blue + (b)->blue; (o)->alpha = (a)->alpha + (b)->alpha; }
// MATCH: 0x004daff0 FUN_004daff0
void FUN_004daff0(RwImage *img, int x0, int x1, int y, RwRGBAReal *out)
{
  float width = (float)(x1 - x0) * DAT_005d8d88;
  RwRGBA *pix = (RwRGBA *)(img->cpPixels + img->stride * (y >> 16) + (x0 >> 16) * 4);
  float s;
  if ((x0 >> 16) == (x1 >> 16)) {
    RwRGBARealFromRwRGBA(out, pix);
    RwRGBARealScale(out, out, width);
  } else {
    RwRGBAReal tmp;
    int x = (x0 & 0xffff0000) + 0x10000;
    RwRGBARealFromRwRGBA(out, pix);
    s = (float)(x - x0) * DAT_005d8d88;
    RwRGBARealScale(out, out, s);
    pix++;
    while ((x >> 16) != (x1 >> 16)) {
      RwRGBARealFromRwRGBA(&tmp, pix);
      RwRGBARealAdd(out, out, &tmp);
      pix++;
      x += 0x10000;
    }
    RwRGBARealFromRwRGBA(&tmp, pix);
    s = (float)(x1 - x) * DAT_005d8d88;
    RwRGBARealScale(&tmp, &tmp, s);
    RwRGBARealAdd(out, out, &tmp);
  }
  s = DAT_005cc320 / width;
  RwRGBARealScale(out, out, s);
}
