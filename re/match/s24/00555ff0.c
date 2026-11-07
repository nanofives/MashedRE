// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00555ff0 (h1): 81.9% (regs 84.8%, ops 98.2%), 558/558 instructions. Font METRICS2 loader written
   from odis.py (the raw converter did not compile: `%` in a global name). Shape that moved the score:
   do { } while (++page < 4) page loop (a for(;;) rotated the fgets head, 69.2%); memcmp / memset / memcpy
   intrinsics (repe cmpsb, rep stosd, 0x81-byte rep movsd + movsb); the invalid UTF-8 branch sets code = 0x10000 so
   the compiler threads it to the `page = 4; result = 0` block; `code <= 0xffff` and `code >= 0x80` branch order;
   `(c & 0x80) == 0` for the `js` test; aspect = float temp / float temp (fild + fdivp, not fidiv); cursor = 0
   before the loop and at the end of each pass (ebp reused for font->fc = 0).
   Residual (b): stack-slot assignment and register choice (ours keeps the sscanf count in esi, the original keeps
   the font in esi and the count in a stack slot), and the page byte store scheduled before the char-offset
   increment. Permuter (2584 compiles, 300 s) reached 87.8% only by calling FUN_004cdd60 before FUN_004cdca0
   (changes behaviour, rejected); its prevH/prevW order is kept. Budget spent (40 min). */
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
#pragma intrinsic(memcmp, memset, memcpy)
int memcmp(const void *a, const void *b, unsigned int n);
void *memset(void *d, int c, unsigned int n);
void *memcpy(void *d, const void *s, unsigned int n);

typedef struct RwRaster { void *parent; unsigned char *cpPixels; unsigned char *palette; int width; int height; } RwRaster;
typedef struct RwTexture { RwRaster *raster; } RwTexture;
typedef struct RwImage { int flags; int width; int height; int depth; int stride; unsigned char *cpPixels; } RwImage;
typedef struct FontChar {
  float aspect;
  RwTexture *tex;
  float u0;
  float v0;
  float u1;
  float v1;
  int f18;
  unsigned char page;
} FontChar;
typedef struct Font {
  int f0;
  float cellH;
  float baseline;
  int fc;
  unsigned int flags;
  RwTexture *pages[4];
  short ascii[128];
  int pad124[3];
  int count;
  void *chars;
} Font;
typedef int (*SScanfFn)(const char *s, const char *fmt, ...);
typedef unsigned int (*StrLenFn)(const char *s);
typedef void *(*MallocFn)(unsigned int size, unsigned int hint);
typedef void (*FreeFn)(void *p);
void FUN_004c5c80(void *dict);
void *FUN_004c5ca0(void);
RwTexture *FUN_004c5cb0(const char *name, const char *mask);
RwImage *FUN_004cdca0(int w, int h, int depth);
int FUN_004cdd00(RwImage *image);
RwImage *FUN_004cdd60(RwImage *image);
void *FUN_004ce2d0(const char *path);
RwImage *FUN_004d52d0(RwImage *image, RwRaster *raster);
char *FUN_00550a20(char *buf, int size, void *stream);
int FUN_00555f20(Font *font);
void *FUN_005c4c60(void *list, int n, unsigned int hint);
FontChar *FUN_005c4d30(void *list);
MGLOBAL(void *, DAT_00912a20, 0x00912a20);
MGLOBAL(int *, DAT_0091325c, 0x0091325c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(char *, DAT_00912a0c, 0x00912a0c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(int, DAT_00913260, 0x00913260);
MGLOBAL(float, DAT_005cd088, 0x005cd088);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005cc31c, 0x005cc31c);
MGLOBALA(char, DAT_00623ee8, 3, 0x00623ee8);
MGLOBALA(char, s__s__s_00623ef8, 7, 0x00623ef8);
MGLOBALA(char, DAT_0091320c, 1, 0x0091320c);

// MATCH: 0x00555ff0 FUN_00555ff0
Font *FUN_00555ff0(Font *param_1, char *param_2, void *param_3, char *param_4)
{
  Font *result;
  void *savedDict;
  unsigned int page;
  int count;
  int cap;
  int *map;
  int *newMap;
  int newSize;
  int i;
  FontChar *chars;
  RwImage *image;
  RwTexture *tex;
  RwRaster *raster;
  int w;
  int h;
  int prevW;
  int prevH;
  float invW;
  float invH;
  int baseline;
  int n;
  unsigned char *pixels;
  int bpp;
  int cellH;
  int x;
  int y;
  int y2;
  int lastX;
  int cw;
  char *cursor;
  int code;
  int len;
  char c;
  FontChar *ch;
  float fw;
  float fh;

  page = 0;
  savedDict = FUN_004c5ca0();
  FUN_004c5c80(DAT_00912a20);
  result = param_1;
  count = 0;
  cap = param_1->count;
  param_1->f0 = 0;
  if (cap == 0) {
    cap = 0x100;
  }
  map = DAT_0091325c;
  FUN_005c4c60(param_1->chars, cap, 0x30190);
  chars = FUN_005c4d30(param_1->chars);
  memset(chars, 0, cap * sizeof(FontChar));
  prevH = 0;
  prevW = 0;
  image = 0;
  cursor = 0;
  do {
    if (FUN_00550a20(param_4, 0x100, param_3) == 0) {
      break;
    }
    n = (*(SScanfFn *)(DAT_007d3ff8 + 0x104))(param_4, s__s__s_00623ef8, param_2, DAT_0091320c);
    if (n == 0 || (n != 1 && n != 2)) {
      break;
    }
    FUN_004ce2d0(DAT_00912a0c);
    if (n == 1) {
      tex = FUN_004c5cb0(param_2, 0);
    }
    else {
      tex = FUN_004c5cb0(param_2, DAT_0091320c);
    }
    if (tex == 0) {
      result = 0;
      break;
    }
    raster = tex->raster;
    param_1->pages[page] = tex;
    w = raster->width;
    h = raster->height;
    if (image == 0 || prevW != w || prevH != h) {
      if (image != 0) {
        FUN_004cdd00(image);
      }
      image = FUN_004cdca0(w, h, 0x20);
      FUN_004cdd60(image);
    }
    FUN_004d52d0(image, raster);
    prevW = w;
    prevH = h;
    invW = DAT_005cc320 / (float)w;
    invH = DAT_005cc320 / (float)h;
    FUN_00550a20(param_4, 0x100, param_3);
    (*(SScanfFn *)(DAT_007d3ff8 + 0x104))(param_4, DAT_00623ee8, &baseline);
    pixels = image->cpPixels;
    bpp = image->depth >> 3;
    cellH = 0;
    for (y = 1; y < h; y++) {
      if (memcmp(pixels, pixels + y * image->stride, bpp) == 0) {
        cellH = y;
        break;
      }
    }
    if (cellH <= 0) {
      result = 0;
      break;
    }
    for (y = 0; y < h && result == param_1; y += cellH) {
      lastX = 0;
      for (x = 1; x < w && result == param_1; x++) {
        if (memcmp(pixels, pixels + image->stride * y + x * bpp, bpp) == 0) {
          cw = x - lastX;
          if (cw == 1) {
            break;
          }
          y2 = y + cellH;
          if (cursor == 0 || *cursor == '\n' || *cursor == '\0') {
            FUN_00550a20(param_4, 0x100, param_3);
            cursor = param_4;
          }
          else if (cursor - param_4 >= 0x80 && (*(StrLenFn *)(DAT_007d3ff8 + 0xf4))(cursor) == 0x80) {
            memcpy(param_4, cursor, 0x81);
            cursor = param_4;
            len = 0x80;
            if (param_4[0x7f] == '\n') {
              len = 0x7f;
            }
            FUN_00550a20(param_4 + len, 0x100 - len, param_3);
          }
          c = *cursor;
          if ((c & 0x80) == 0) {
            code = c;
            len = 1;
          }
          else if ((c & 0xe0) == 0xc0) {
            code = ((c & 0x1f) << 6) | (cursor[1] & 0x3f);
            len = 2;
          }
          else if ((c & 0xf0) == 0xe0) {
            code = ((((c & 0xf) << 6) | (cursor[1] & 0x3f)) << 6) | (cursor[2] & 0x3f);
            len = 3;
          }
          else if ((c & 0xf8) == 0xf0) {
            code = ((((((c & 7) << 6) | (cursor[1] & 0x3f)) << 6) | (cursor[2] & 0x3f)) << 6) | (cursor[3] & 0x3f);
            len = 4;
          }
          else if ((c & 0xfc) == 0xf8) {
            code = ((((((((c & 3) << 6) | (cursor[1] & 0x3f)) << 6) | (cursor[2] & 0x3f)) << 6) |
                    (cursor[3] & 0x3f)) << 6) | (cursor[4] & 0x3f);
            len = 5;
          }
          else if ((c & 0xfe) == 0xfc) {
            code = ((((((((((c & 1) << 6) | (cursor[1] & 0x3f)) << 6) | (cursor[2] & 0x3f)) << 6) |
                      (cursor[3] & 0x3f)) << 6) | (cursor[4] & 0x3f)) << 6) | (cursor[5] & 0x3f);
            len = 6;
          }
          else {
            code = 0x10000;
            len = 0;
          }
          if (code <= 0xffff) {
            cursor += len;
            if (count >= DAT_00913260) {
              newSize = DAT_00913260 + 0x20;
              newMap = (int *)(*(MallocFn *)(DAT_007d3ff8 + 0x108))(newSize * sizeof(int), 0x30190);
              memcpy(newMap, map, DAT_00913260 * sizeof(int));
              for (i = DAT_00913260; i < newSize; i++) {
                newMap[i] = -1;
              }
              if (map != 0) {
                (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(map);
              }
              DAT_00913260 = newSize;
              map = newMap;
              DAT_0091325c = newMap;
            }
            if (code >= 0x80) {
              param_1->flags |= 2;
              map[count] = code;
            }
            else {
              param_1->ascii[code] = (short)count;
            }
            ch = &chars[count];
            ch->tex = tex;
            ch->u0 = ((float)lastX + DAT_005cd088) * invW;
            ch->page = (unsigned char)page;
            ch->v0 = ((float)y + DAT_005cd088) * invH;
            ch->u1 = ((float)x - DAT_005cc32c) * invW;
            ch->v1 = ((float)y2 - DAT_005cc32c) * invH;
            fw = (float)(cw - 3);
            fh = (float)(y2 - y - 3);
            ch->aspect = fw / fh;
            count++;
            if (count >= cap) {
              FUN_005c4c60(param_1->chars, 0x20, 0x30190);
              chars = FUN_005c4d30(param_1->chars);
              cap += 0x20;
              memset(&chars[count], 0, 0x20 * sizeof(FontChar));
            }
          }
          else {
            page = 4;
            result = 0;
          }
          lastX = x;
        }
      }
    }
    cursor = 0;
    param_1->cellH = (float)cellH - DAT_005cc31c;
    param_1->baseline = (float)baseline / param_1->cellH;
    param_1->fc = 0;
    page++;
  } while (page < 4);
  if (image != 0) {
    FUN_004cdd00(image);
  }
  if (result == param_1) {
    param_1->count = count;
    FUN_00555f20(param_1);
    if (DAT_0091325c != 0) {
      memset(DAT_0091325c, 0xff, DAT_00913260 * sizeof(int));
    }
  }
  FUN_004c5c80(savedDict);
  return result;
}
