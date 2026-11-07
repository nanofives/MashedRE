// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x00555af0: 99.3% (regs 99.3%, ops 100%). .met font loader. What moved it (63.5 -> 99.3): the texture pointer goes back into the name parameter (name = (char *)FUN_004c5cb0(name, ...)): the original homes it in param_2's slot and a separate Tex* local cost a whole extra frame slot (every offset +4, about 30 points); memcpy/memset intrinsics for the copy and the 0/0xff fills, while the tail fill of the grown map is a plain loop (rep stosd without a byte tail); if (code > 0xffff) result = 0 first; if (tex) {...} else result = 0; fw/fh float temporaries for the glyph aspect (fild/fild/fdivp, not fidiv) with g->flag = 0 last; the 0.5f/1.0f .rdata constants as literals. Residual (b): the int temp for the final (float)(y1 - y0) lands in font's dead parameter slot (+0x50) where the original uses buf's (+0x5c), 2 instructions. Tried an int temp, a reused local, a float temp, permuter 300 s (no gain). */
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
typedef struct Raster { int pad[3]; int width; int height; } Raster;   /* +0x0c, +0x10 */
typedef struct Tex { Raster *raster; } Tex;
typedef struct Glyph {
  float w;              /* +0x00 */
  Tex *tex;             /* +0x04 */
  float u0, v0, u1, v1; /* +0x08..+0x14 */
  int pad18;
  unsigned char flag;   /* +0x1c */
  char pad1d[3];
} Glyph;
typedef struct Font {
  int f00;              /* +0x00 */
  float height;         /* +0x04 */
  float scale;          /* +0x08 */
  float f0c;            /* +0x0c */
  int flags;            /* +0x10 */
  Tex *tex;             /* +0x14 */
  int pad18[3];
  short map[0x80];      /* +0x24 */
  int pad124[3];
  int cap;              /* +0x130 */
  int raster;           /* +0x134 */
} Font;
typedef int (__cdecl *sscanf_t)(const char *, const char *, ...);
typedef int (__cdecl *sprintf_t)(char *, const char *, ...);
typedef void *(__cdecl *malloc_t)(unsigned int, unsigned int);
typedef void (__cdecl *free_t)(void *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(char *, DAT_00912a0c, 0x00912a0c);
MGLOBAL(int, DAT_00912a20, 0x00912a20);
MGLOBALA(char, DAT_0091320c, 80, 0x0091320c);
MGLOBAL(int *, DAT_0091325c, 0x0091325c);
MGLOBAL(int, DAT_00913260, 0x00913260);
MGLOBALA(char, s__i__i__i__i__i_00623ed8, 16, 0x00623ed8);
MGLOBALA(char, DAT_00623ee8, 4, 0x00623ee8);
MGLOBALA(char, PTR_DAT_00623eec, 4, 0x00623eec);
MGLOBALA(char, s__sm_s_00623ef0, 6, 0x00623ef0);
MGLOBALA(char, s__s__s_00623ef8, 7, 0x00623ef8);
#ifdef __cplusplus
extern "C" {
#endif
void *__cdecl memset(void *, int, unsigned int);
void *__cdecl memcpy(void *, const void *, unsigned int);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(memset, memcpy)
void __cdecl FUN_004c5c80(int);
int __cdecl FUN_004c5ca0(void);
Tex *__cdecl FUN_004c5cb0(char *, char *);
void __cdecl FUN_004ce2d0(char *);
int __cdecl FUN_00550a20(char *, int, int);
int __cdecl FUN_00550b00(char *);
int __cdecl FUN_00555f20(Font *);
void __cdecl FUN_005c4c60(int, int, int);
Glyph *__cdecl FUN_005c4d30(int);
// MATCH: 0x00555af0 FUN_00555af0
Font *FUN_00555af0(Font *font, char *name, int file, char *buf)
{
  int saved;
  Font *result;
  int cap, count, n, newN;
  int *map, *p;
  Glyph *glyphs, *g;
  int width, height, lineH;
  float invW, invH, fw, fh;
  int code, x0, y0, x1, y1, i;

  saved = FUN_004c5ca0();
  FUN_004c5c80(DAT_00912a20);
  result = font;
  count = 0;
  cap = font->cap;
  font->f00 = 0;
  if (!cap) cap = 0x100;
  map = DAT_0091325c;
  FUN_005c4c60(font->raster, cap, 0x30190);
  glyphs = FUN_005c4d30(font->raster);
  memset(glyphs, 0, cap * sizeof(Glyph));
  FUN_00550a20(buf, 0x100, file);
  n = (*(sscanf_t *)(DAT_007d3ff8 + 0x104))(buf, s__s__s_00623ef8, name, DAT_0091320c);
  if (n == 1 || n == 2) {
    FUN_004ce2d0(DAT_00912a0c);
    if (n == 1) {
      (*(sprintf_t *)(DAT_007d3ff8 + 0xc4))(DAT_0091320c, s__sm_s_00623ef0, DAT_00912a0c, name);
      if (FUN_00550b00(DAT_0091320c)) {
        (*(sprintf_t *)(DAT_007d3ff8 + 0xc4))(DAT_0091320c, PTR_DAT_00623eec, name);
        name = (char *)FUN_004c5cb0(name, DAT_0091320c);
      } else {
        name = (char *)FUN_004c5cb0(name, 0);
      }
    } else {
      name = (char *)FUN_004c5cb0(name, DAT_0091320c);
    }
    if (name != 0) {
      font->tex = (Tex *)name;
      width = ((Tex *)name)->raster->width;
      height = ((Tex *)name)->raster->height;
      FUN_00550a20(buf, 0x100, file);
      (*(sscanf_t *)(DAT_007d3ff8 + 0x104))(buf, DAT_00623ee8, &lineH);
      invW = 1.0f / (float)width;
      invH = 1.0f / (float)height;
      do {
        if (!FUN_00550a20(buf, 0x100, file)) {
          if (result == font) {
            font->cap = count;
            FUN_00555f20(font);
            if (DAT_0091325c) memset(DAT_0091325c, 0xff, DAT_00913260 * sizeof(int));
            font->f0c = 0;
            font->height = (float)(y1 - y0);
            font->scale = (float)lineH / font->height;
          }
          break;
        }
        if ((*(sscanf_t *)(DAT_007d3ff8 + 0x104))(buf, s__i__i__i__i__i_00623ed8, &code, &x0, &y0, &x1, &y1) == 5) {
          if (code > 0xffff) {
            result = 0;
          } else {
            if (count >= DAT_00913260) {
              newN = DAT_00913260 + 0x20;
              p = (int *)(*(malloc_t *)(DAT_007d3ff8 + 0x108))(newN * sizeof(int), 0x30190);
              memcpy(p, map, DAT_00913260 * sizeof(int));
              for (i = DAT_00913260; i < newN; i++) p[i] = -1;
              if (map) (*(free_t *)(DAT_007d3ff8 + 0x10c))(map);
              DAT_00913260 = newN;
              map = p;
              DAT_0091325c = p;
            }
            if (code >= 0x80) {
              font->flags |= 2;
              map[count] = code;
            } else {
              font->map[code] = (short)count;
            }
            g = &glyphs[count];
            g->u0 = ((float)x0 + 0.5f) * invW;
            g->v0 = ((float)y0 + 0.5f) * invH;
            g->u1 = ((float)x1 + 0.5f) * invW;
            g->v1 = ((float)y1 + 0.5f) * invH;
            g->tex = (Tex *)name;
            fw = (float)(x1 - x0);
            fh = (float)(y1 - y0);
            g->w = fw / fh;
            g->flag = 0;
            count++;
            if (count >= cap) {
              FUN_005c4c60(font->raster, 0x20, 0x30190);
              glyphs = FUN_005c4d30(font->raster);
              cap += 0x20;
              memset(&glyphs[count], 0, 0x20 * sizeof(Glyph));
            }
          }
        }
      } while (result == font);
    } else {
      result = 0;
    }
  }
  FUN_004c5c80(saved);
  return result;
}
