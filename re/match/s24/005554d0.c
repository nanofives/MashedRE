// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x005554d0: 100% (regs 100%). Font/glyph structs typed from the offsets; hi = 0 before the if (gives xor ebx,ebx and mask = bl), unsigned char mask, separate cursor p = str, and the c >= 0x80 branch first. First pass (hi set in both arms, cursor = param) 37.5%. */
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
typedef struct Glyph { float w; int pad[7]; } Glyph;      /* 0x20 bytes */
typedef struct Font {
  int pad00[3];
  float spacing;        /* +0x0c */
  unsigned char flags;  /* +0x10 */
  char pad11[0x13];
  short map[0x80];      /* +0x24 */
  int first;            /* +0x124 */
  int count;            /* +0x128 */
  short *ext;           /* +0x12c */
  int pad130;
  int raster;           /* +0x134 */
} Font;
Glyph *__cdecl FUN_005c4d30(int);
// MATCH: 0x005554d0 FUN_005554d0
float FUN_005554d0(Font *font, unsigned char *str, float scale)
{
  float sum = 0.0f;
  Glyph *glyphs;
  int step, hi, c, idx;
  unsigned char mask;
  unsigned char *p;
  glyphs = FUN_005c4d30(font->raster);
  hi = 0;
  if (font->flags & 2) {
    step = 2;
    mask = 0xff;
    hi = 1;
  } else {
    step = 1;
    mask = 0;
  }
  p = str;
  c = ((str[hi] & mask) << 8) | str[0];
  while (c > 0) {
    if (c >= 0x80) {
      c -= font->first;
      if (c < 0 || c >= font->count) idx = -1;
      else idx = font->ext[c];
    } else {
      idx = font->map[c];
    }
    p += step;
    if (idx >= 0) sum += glyphs[idx].w + font->spacing;
    c = ((p[hi] & mask) << 8) | p[0];
  }
  return sum * scale;
}
