// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x00554940: no Ghidra size; nosize.py: acc 12.8, regs 30.0 on our 1417 B symbol; the original runs 0x00554940..0x005551cb (ret at 0x005551ca, nop padding to the next entry 0x005551d0), 2187 B. Structure only: string draw (font glyph quads, 4 vertices / 6 indices per glyph through DAT_00912a04 / DAT_00912700, flush at a texture change or n > 0x17a, render states 9/1, gradient colours from the style). Residual (c): 16 colour-channel float-to-int conversions in the original are inline fnstcw / or ah,0xc / fldcw / fistp / fldcw sequences (an __asm truncating macro); plain C compiles them to call __ftol under 13.00.9176 and __asm is barred by the source rules, so 770 B of the original have no C counterpart and the streams cannot align. Same macro as 0x005572c0. */
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
typedef struct Tex { int raster; int pad[19]; unsigned int flags; } Tex;  /* +0x50 */
typedef struct Glyph { float w; Tex *tex; float u0, v0, u1, v1; int pad[2]; } Glyph;  /* 0x20 */
typedef struct Vtx { float x, y, z; float pad[3]; unsigned int color; float u, v; } Vtx;  /* 0x24 */
typedef struct Font {
  int pad00[2];
  float height;         /* +0x08 */
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
typedef struct Style {
  float c0[4];          /* +0x00 */
  float uv0[2];         /* +0x10 */
  float dc0[4];         /* +0x18 */
  float duv0[2];        /* +0x28 */
  float c1[4];          /* +0x30 */
  float uv1[2];         /* +0x40 */
  float dc1[4];         /* +0x48 */
  float duv1[2];        /* +0x58 */
  unsigned char rgba[4];/* +0x60 */
  unsigned int flags;   /* +0x64 */
  Tex *tex;             /* +0x68 */
} Style;
typedef int (__cdecl *rsset_t)(int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(unsigned short, DAT_00912700, 0x180, 0x00912700);
MGLOBAL(Vtx *, DAT_00912a04, 0x00912a04);
MGLOBAL(float, DAT_00912be0, 0x00912be0);
int __cdecl FUN_004cd070(Vtx *, int, int, int);
int __cdecl FUN_004cd140(void);
int __cdecl FUN_004cd170(int, unsigned short *, int);
int __cdecl FUN_00552d10(void);
int __cdecl FUN_00552d70(void);
int __cdecl FUN_00552da0(float, float);
int __cdecl FUN_00552df0(float, float);
int __cdecl FUN_00552e40(void);
float __cdecl FUN_005554d0(Font *, unsigned char *, float);
Glyph *__cdecl FUN_005c4d30(int);
// MATCH: 0x00554940 FUN_00554940
#define COL(cc, dd, t) ((((unsigned int)(int)((cc)[3] + (dd)[3] * (t)) << 8 | ((int)((cc)[0] + (dd)[0] * (t)) & 0xff)) << 8 | ((int)((cc)[1] + (dd)[1] * (t)) & 0xff)) << 8 | ((int)((cc)[2] + (dd)[2] * (t)) & 0xff))
#define SCOL(s) ((((unsigned int)(s)->rgba[3] << 8 | (s)->rgba[0]) << 8 | (s)->rgba[1]) << 8 | (s)->rgba[2])
Font *FUN_00554940(Font *font, unsigned char *str, float size, float *pos, Style *st)
{
  float z, inv, penX, penT, t;
  int n, step, hi, c, idx, ctx;
  unsigned char mask;
  unsigned char *cur;
  Glyph *glyphs, *g;
  Tex *curTex;
  Vtx *v;

  z = DAT_00912be0;
  n = 0;
  if (font) {
    glyphs = FUN_005c4d30(font->raster);
    if (font->flags & 2) {
      step = 2;
      mask = 0xff;
      hi = 1;
    } else {
      step = 1;
      mask = 0;
      hi = 0;
    }
    cur = str;
    FUN_00552d10();
    FUN_00552df0(pos[0], pos[1]);
    FUN_00552da0(size, size);
    FUN_00552df0(0, -font->height);
    ctx = FUN_00552e40();
    inv = 1.0f / FUN_005554d0(font, str, 1.0f);
    c = ((str[hi] & mask) << 8) | str[0];
    penT = 0;
    penX = 0;
    curTex = 0;
    v = DAT_00912a04;
    while (c > 0) {
      if (c >= 0x80) {
        c -= font->first;
        if (c < 0 || c >= font->count) idx = -1;
        else idx = font->ext[c];
      } else {
        idx = font->map[c];
      }
      cur += step;
      if (idx >= 0) {
        g = &glyphs[idx];
        if (g->tex != curTex || n > 0x17a) {
          if (n && FUN_004cd070(DAT_00912a04, v - DAT_00912a04, ctx, 5)) {
            FUN_004cd170(3, DAT_00912700, n);
            FUN_004cd140();
          }
          if (g->tex != curTex) {
            (*(rsset_t *)(DAT_007d3ff8 + 0x20))(9, g->tex->flags & 0xff);
            (*(rsset_t *)(DAT_007d3ff8 + 0x20))(1, g->tex->raster);
            curTex = g->tex;
          }
          v = DAT_00912a04;
          n = 0;
          if (st->tex && (st->flags & 2)) st->flags &= ~2;
        }
        v[0].x = penX;
        v[0].y = 0;
        v[0].z = z;
        v[0].u = g->u0;
        v[0].v = g->v1;
        if (st->flags & 1) { t = inv * penT; v[0].color = COL(st->c1, st->dc1, t); }
        else v[0].color = SCOL(st);
        v[1].x = penX;
        v[1].y = 1.0f;
        v[1].z = z;
        v[1].u = g->u0;
        v[1].v = g->v0;
        if (st->flags & 1) { t = inv * penT; v[1].color = COL(st->c0, st->dc0, t); }
        else v[1].color = SCOL(st);
        v[2].x = penX + g->w;
        v[2].y = 0;
        v[2].z = z;
        v[2].u = g->u1;
        v[2].v = g->v1;
        if (st->flags & 1) { t = (penT + g->w) * inv; v[2].color = COL(st->c1, st->dc1, t); }
        else v[2].color = SCOL(st);
        v[3].x = penX + g->w;
        v[3].y = 1.0f;
        v[3].z = z;
        v[3].u = g->u1;
        v[3].v = g->v0;
        if (st->flags & 1) { t = (penT + g->w) * inv; v[3].color = COL(st->c0, st->dc0, t); }
        else v[3].color = SCOL(st);
        v += 4;
        n += 6;
        penX += font->spacing + g->w;
        penT += font->spacing + g->w;
      }
      c = ((cur[hi] & mask) << 8) | cur[0];
    }
    if (n && FUN_004cd070(DAT_00912a04, v - DAT_00912a04, ctx, 5)) {
      FUN_004cd170(3, DAT_00912700, n);
      FUN_004cd140();
    }
    pos[0] = penX * size + pos[0];
    FUN_00552d70();
  }
  return font;
}
#undef COL
#undef SCOL
