// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x005555b0: 100% (regs 100%). Word-wrapping text draw (font draw method at +0x138). What moved it: switch (align) for the dec/dec dispatch, idx = c - first (c stays live), c >= 0x80 arm first, separate line variable homed in str's slot, w > maxW, the 0.5 as a literal size * 0.5f (the .rdata 0x005cc32c is 0.5; the global gave the swapped fld/fmul order), the saved second byte as save[3] of an unsigned char save[4] (byte at frame +0x27; save[1] of a 2-array 96.5%, a char 77.2%), and cur = str before line = str. 0.0 stays the .rdata global DAT_005d757c (fld from memory). */
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
typedef struct Font Font;
typedef void (__cdecl *drawfn_t)(Font *, unsigned char *, float, float *, int);
struct Font {
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
  drawfn_t draw;        /* +0x138 */
};
MGLOBAL(float, DAT_005d757c, 0x005d757c);
Glyph *__cdecl FUN_005c4d30(int);
// MATCH: 0x005555b0 FUN_005555b0
Font *FUN_005555b0(Font *font, unsigned char *str, float size, float *rect, int align, int arg)
{
  Glyph *glyphs;
  float maxW, alignK, breakW, w;
  float pos[2];
  int step, hi, c, idx, canBreak;
  unsigned int mask;
  unsigned char *cur, *brk, *line;
  unsigned char save[4];

  glyphs = FUN_005c4d30(font->raster);
  maxW = rect[2] / size;
  switch (align) {
  case 1: alignK = size * 0.5f; break;
  case 2: alignK = size; break;
  default: alignK = 0.0f; break;
  }
  breakW = 0.0f;
  if (font->flags & 2) {
    step = 2;
    mask = 0xff;
    hi = 1;
  } else {
    hi = 0;
    step = 1;
    mask = 0;
  }
  w = DAT_005d757c;
  pos[1] = rect[3] + rect[1] - size;
  cur = str;
  line = str;
  brk = str;
  canBreak = 1;
  c = ((cur[hi] & mask) << 8) | cur[0];
  while (c > 0) {
    if (c >= 0x80) {
      idx = c - font->first;
      if (idx < 0 || idx >= font->count) idx = -1;
      else idx = font->ext[idx];
    } else {
      idx = font->map[c];
    }
    if (canBreak && c == ' ') {
      canBreak = 0;
      brk = cur;
      breakW = w;
    } else {
      canBreak = 1;
      if (c == '\n') {
        brk = cur + step;
        breakW = w;
      }
    }
    if (idx >= 0) w += glyphs[idx].w + font->spacing;
    if (w > maxW || c == '\n') {
      save[0] = brk[0];
      save[3] = brk[hi];
      brk[0] = 0;
      brk[hi] = 0;
      pos[0] = (maxW - breakW) * alignK + rect[0];
      font->draw(font, line, size, pos, arg);
      brk[0] = save[0];
      brk[hi] = save[3];
      c = ((save[3] & mask) << 8) | brk[0];
      while (c == ' ') {
        brk += step;
        c = ((brk[hi] & mask) << 8) | brk[0];
      }
      w = DAT_005d757c;
      pos[1] -= size;
      line = brk;
      cur = brk;
      canBreak = 1;
      if (pos[1] < rect[1]) return 0;
    } else {
      cur += step;
    }
    c = ((cur[hi] & mask) << 8) | cur[0];
  }
  pos[0] = (maxW - w) * alignK + rect[0];
  font->draw(font, line, size, pos, arg);
  return font;
}
