// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00556780 (h2): 75.5% (regs 80.1%), 390/392 instructions. Font .met loader: reads a header
   line, sizes the glyph free list (+0x134) to numGlyphs (+0x130, default 0x100) and zeroes it, then per line:
   a UTF-8 code point (1..6 byte lead forms) when no glyph is open, "begin\n" opens a path (FUN_00553ef0/
   FUN_00553f40), "end\n" closes it (FUN_00553e00/FUN_00554010), maps the code (short map at +0x24 below 0x80,
   else the global table 0x0091325c grown by 0x20 entries), stores path and x into the 0x20-byte glyph entry
   and grows the list by 0x20 when full; moveto/lineto/curveto/closepath feed FUN_00553030/005530f0/005531f0/
   005533d0. Then numGlyphs, FUN_00555f20, the global table filled with 0xff, +0x138 = FUN_005547f0.
   RwGlobals +0x104 = sscanf, +0xe8 = strcmp, +0x108/+0x10c = malloc/free. Raw converter did not compile
   (`%` in a string name). memcpy/memset intrinsics give the original's rep movsd/stosd; `code >= 0x80` branch
   first; statement order from the permuter (1724 compiles: count = 0 before the first read, code = 0 before
   count++, cap += 0x20 before the list grow). The permuter's 80.1% variant moved `result = 0` after a goto
   (a dead store, changes behaviour) and was not kept.
   Residual: stack-slot and register assignment (the original keeps line in ebx, font in ebp, a zero in esi
   and code/cap/count in different frame slots). Class (b): local layout / register allocation not found. */
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
typedef int (*rwsscanf_t)(const char *s, const char *fmt, ...);
typedef int (*rwstrcmp_t)(const char *a, const char *b);
typedef void *(*rwmalloc_t)(unsigned int size, unsigned int hint);
typedef void (*rwfree_t)(void *p);
typedef struct {
  float x;            /* +0x00 */
  int pad[5];
  int path;           /* +0x18 */
  int pad2;
} FontGlyph;
typedef struct {
  int a;              /* +0x000 */
  float scale;        /* +0x004 */
  int c;              /* +0x008 */
  int d;              /* +0x00c */
  unsigned int flags; /* +0x010 */
  int pad[4];
  short map[0x80];    /* +0x024 */
  int pad3[3];
  int numGlyphs;      /* +0x130 */
  int glyphs;         /* +0x134 */
  void *fn;           /* +0x138 */
} FontDef;
#ifdef __cplusplus
extern "C" {
#endif
void *memset(void *d, int c, unsigned int n);
void *memcpy(void *d, const void *s, unsigned int n);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(memset, memcpy)
MGLOBALA(char, s_closepath_00623f00, 11, 0x00623f00);
MGLOBALA(char, s_curveto_00623f0c, 27, 0x00623f0c);
MGLOBALA(char, s_lineto_00623f28, 14, 0x00623f28);
MGLOBALA(char, s_moveto_00623f38, 14, 0x00623f38);
MGLOBALA(char, s_end_00623f48, 5, 0x00623f48);
MGLOBALA(char, s_begin_00623f50, 7, 0x00623f50);
MGLOBALA(char, s_line_00623f58, 6, 0x00623f58);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int *, DAT_0091325c, 0x0091325c);
MGLOBAL(int, DAT_00913260, 0x00913260);
int FUN_00550a20(char *buf, int size, int stream);
int FUN_00553030(int path, float x, float y);
int FUN_005530f0(int path, float x, float y);
int FUN_005531f0(int path, float x0, float y0, float x1, float y1, float x2, float y2);
int FUN_005533d0(int path);
int FUN_00553e00(int path);
int FUN_00553ef0(void);
int FUN_00553f40(int path);
int FUN_00554010(int p);
int FUN_00555f20(FontDef *font);
int FUN_005547f0();
int FUN_005c4c60(int list, int n, int hint);
FontGlyph *FUN_005c4d30(int list);
// MATCH: 0x00556780 FUN_00556780
FontDef *FUN_00556780(FontDef *font, int unused, int stream, char *line)
{
  int path;
  int code;
  FontDef *result;
  int cap;
  int count;
  int *table;
  float x[3];
  float y[3];
  FontGlyph *glyphs;
  int offset;
  int *newTable;
  char name[128];
  int i;

  result = font;
  font->a = 1;
  font->scale = 1.0f;
  font->c = 0;
  font->d = 0;
  count = 0;
  FUN_00550a20(line, 0x100, stream);
  path = 0;
  cap = font->numGlyphs;
  if (cap == 0) {
    cap = 0x100;
  }
  table = DAT_0091325c;
  FUN_005c4c60(font->glyphs, cap, 0x30190);
  glyphs = FUN_005c4d30(font->glyphs);
  memset(glyphs, 0, cap << 5);
  if ((*(rwsscanf_t *)(DAT_007d3ff8 + 0x104))(line, s_line_00623f58, name) == 1) {
    code = 0;
    offset = 0;
    while (FUN_00550a20(line, 0x100, stream) != 0) {
      if (code == 0) {
        unsigned char c = line[1];
        int v;
        if ((c & 0x80) == 0) {
          v = (signed char)c;
        }
        else if ((c & 0xe0) == 0xc0) {
          v = ((c & 0x1f) << 6) | (line[2] & 0x3f);
        }
        else if ((c & 0xf0) == 0xe0) {
          v = ((((c & 0xf) << 6) | (line[2] & 0x3f)) << 6) | (line[3] & 0x3f);
        }
        else if ((c & 0xf8) == 0xf0) {
          v = ((((((c & 7) << 6) | (line[2] & 0x3f)) << 6) | (line[3] & 0x3f)) << 6) | (line[4] & 0x3f);
        }
        else if ((c & 0xfc) == 0xf8) {
          v = ((((((((c & 3) << 6) | (line[2] & 0x3f)) << 6) | (line[3] & 0x3f)) << 6) | (line[4] & 0x3f)) << 6) |
              (line[5] & 0x3f);
        }
        else if ((c & 0xfe) == 0xfc) {
          v = ((((((((((c & 1) << 6) | (line[2] & 0x3f)) << 6) | (line[3] & 0x3f)) << 6) | (line[4] & 0x3f)) << 6) |
                (line[5] & 0x3f)) << 6) | (line[6] & 0x3f);
        }
        else {
          result = 0;
          goto next;
        }
        code = v;
        if (code > 0xffff) {
          result = 0;
        }
      }
      else if ((*(rwstrcmp_t *)(DAT_007d3ff8 + 0xe8))(line, s_begin_00623f50) == 0) {
        path = FUN_00553ef0();
        FUN_00553f40(path);
      }
      else if ((*(rwstrcmp_t *)(DAT_007d3ff8 + 0xe8))(line, s_end_00623f48) == 0) {
        int p = FUN_00553e00(path);
        FUN_00554010(p);
        if (count >= DAT_00913260) {
          int n = DAT_00913260 + 0x20;
          newTable = (int *)(*(rwmalloc_t *)(DAT_007d3ff8 + 0x108))(n * 4, 0x30190);
          memcpy(newTable, table, DAT_00913260 * 4);
          for (i = DAT_00913260; i < n; i++) {
            newTable[i] = -1;
          }
          if (table != 0) {
            (*(rwfree_t *)(DAT_007d3ff8 + 0x10c))(table);
          }
          DAT_00913260 = n;
          table = newTable;
          DAT_0091325c = newTable;
        }
        if (code >= 0x80) {
          font->flags |= 2;
          table[count] = code;
        }
        else {
          font->map[code] = (short)count;
        }
        offset += 0x20;
        ((FontGlyph *)((char *)glyphs + offset))[-1].path = p;
        ((FontGlyph *)((char *)glyphs + offset))[-1].x = x[0];
        code = 0;
        count++;
        path = 0;
        if (count >= cap) {
          cap += 0x20;
          FUN_005c4c60(font->glyphs, 0x20, 0x30190);
          glyphs = FUN_005c4d30(font->glyphs);
          memset((char *)glyphs + offset, 0, 0x400);
        }
      }
      else if (path != 0) {
        if ((*(rwsscanf_t *)(DAT_007d3ff8 + 0x104))(line, s_moveto_00623f38, &x[0], &y[0]) == 2) {
          FUN_00553030(path, x[0], y[0]);
        }
        else if ((*(rwsscanf_t *)(DAT_007d3ff8 + 0x104))(line, s_lineto_00623f28, &x[0], &y[0]) == 2) {
          FUN_005530f0(path, x[0], y[0]);
        }
        else if ((*(rwsscanf_t *)(DAT_007d3ff8 + 0x104))(line, s_curveto_00623f0c, &x[0], &y[0], &x[1], &y[1],
                                                         &x[2], &y[2]) == 6) {
          FUN_005531f0(path, x[0], y[0], x[1], y[1], x[2], y[2]);
        }
        else if ((*(rwstrcmp_t *)(DAT_007d3ff8 + 0xe8))(line, s_closepath_00623f00) == 0) {
          FUN_005533d0(path);
        }
      }
next:
      if (result != font) {
        return result;
      }
    }
    if (result != font) {
      return result;
    }
  }
  font->numGlyphs = count;
  FUN_00555f20(font);
  if (DAT_0091325c != 0) {
    memset(DAT_0091325c, 0xff, DAT_00913260 * 4);
  }
  font->fn = (void *)FUN_005547f0;
  return result;
}
