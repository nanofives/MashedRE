// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cde50 (h3): 100% (regs 100%). FLAGS /G5. RwImage: for depth 4/8 sets every palette entry's alpha to max(r,g,b) over 1<<depth entries; for depth 32 does the same per pixel row by row (stride +0x10). From 22.4%: RwImage/RwRGBA structs, int max through a temp, and indexed loops (pal[i], pix[x]: the compiler's +2-biased induction pointer only comes from the index form; pointer-increment loops stuck at 71.8%, permuter 284 compiles no gain), n = 1<<depth before the palette load. Relocheck OK (no globals). */
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

// MATCH: 0x004cde50 FUN_004cde50
RwImage *FUN_004cde50(RwImage *image)
{
  int i;
  int n;
  int x;
  int y;
  int m;
  int t;
  RwRGBA *pal;
  RwRGBA *pix;
  unsigned char *row;

  if (image->depth == 4 || image->depth == 8) {
    n = 1 << image->depth;
    pal = image->palette;
    for (i = 0; i < n; i++) {
      m = pal[i].red;
      t = pal[i].green;
      if (t > m) m = t;
      t = pal[i].blue;
      if (t > m) m = t;
      pal[i].alpha = (unsigned char)m;
    }
  } else if (image->depth == 32) {
    row = image->cpPixels;
    for (y = 0; y < image->height; y++) {
      pix = (RwRGBA *)row;
      for (x = 0; x < image->width; x++) {
        m = pix[x].red;
        t = pix[x].green;
        if (t > m) m = t;
        t = pix[x].blue;
        if (t > m) m = t;
        pix[x].alpha = (unsigned char)m;
      }
      row += image->stride;
    }
  }
  return image;
}
