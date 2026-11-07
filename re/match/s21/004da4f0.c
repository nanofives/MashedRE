// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004da4f0 (h2): 84.5% (regs 86.2%, ops 99.8%). Quantises an RwImage (+4 width, +8 height, +0xc depth,
   +0x10 stride, +0x14 pixels, +0x18 palette) through the 16-way colour tree into an index buffer: depth 4/8
   sources go through the palette, depth 32 reads RGBA directly; a (4, 1) destination format packs two 4-bit
   indices per byte (odd x -> high nibble, then advance), anything else writes one byte per pixel. The key is
   ((t[r>>s]*2 | t[g>>s])*2 | t[b>>s])*2 | t[a>>s] with t = 0x007d6c98, s = (8 - DAT_00618430) & 0xff; depth 0
   takes root+0x18, else FUN_004daa00(root->child[key & 0xf], key >> 4, DAT_00618430 - 1) (FUN_004daa00's first
   level written out at the call site, here as the PalLookup macro).
   What moved it: the lookup as a macro (a static __inline helper was not inlined: 7.9%), `(8 - d) & 0xff`, a
   cached width in the 4-bit loops, `c = s++`, then the permuter (974 compiles, 80.9 -> 84.5: else/if order and
   row-pointer update order). Raw form 16.2%.
   Residual: the original's frame is 0x14 against our 0x18: it keeps the row write pointer and x in the dead
   dstDepth / p4 argument slots, ours puts x in the dstDepth slot and the row pointer in a new local, which
   shifts every esp offset; plus red/green load order inside the palette-path key. Class (b). */
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
typedef struct RwImage { int flags; unsigned int width; unsigned int height; int depth; int stride;
  unsigned char *cpPixels; RwRGBA *palette; } RwImage;
typedef struct Node { char pad[0x18]; unsigned char val; char pad2[3]; struct Node *child[16]; } Node;
typedef struct Pal { char pad[0x4000]; Node *root; void *freelist; } Pal;
MGLOBAL(int, DAT_00618430, 0x00618430);
MGLOBALA(unsigned int, DAT_007d6c98, 256, 0x007d6c98);
unsigned char FUN_004daa00(Node *n, unsigned int key, int depth);
#define PalLookup(out, pal, c)   {     int depth_ = DAT_00618430;     unsigned int sh_ = (8 - depth_) & 0xff;     unsigned int key_ = ((((DAT_007d6c98[(c)->red >> sh_] << 1) | DAT_007d6c98[(c)->green >> sh_]) << 1 |                           DAT_007d6c98[(c)->blue >> sh_]) << 1) | DAT_007d6c98[(c)->alpha >> sh_];     Node *root_ = (pal)->root;     if (depth_ == 0) (out) = root_->val;     else (out) = FUN_004daa00(root_->child[key_ & 0xf], key_ >> 4, depth_ - 1);   }
// MATCH: 0x004da4f0 FUN_004da4f0
void FUN_004da4f0(unsigned char *dst, int dstStride, int dstDepth, int p4, Pal *pal, RwImage *img)
{
  int srcStride = img->stride;
  unsigned char *srcRow = img->cpPixels;
  unsigned int x, y;
  if (!(dstDepth == 4 && p4 == 1)) {
    switch (img->depth) {
    case 4:
    case 8: {
      RwRGBA *palette = img->palette;
      for (y = img->height; y != 0; y--) {
        unsigned char *d = dst;
        unsigned char *s = srcRow;
        for (x = img->width; x != 0; x--) {
          unsigned char idx;
          RwRGBA *c = &palette[*s++];
          PalLookup(idx, pal, c);
          *d++ = idx;
        }
        srcRow += srcStride;
        dst += dstStride;
      }
      break;
    }
    case 32:
      for (y = img->height; y != 0; y--) {
        unsigned char *d = dst;
        RwRGBA *s = (RwRGBA *)srcRow;
        for (x = img->width; x != 0; x--) {
          unsigned char idx;
          RwRGBA *c = s++;
          PalLookup(idx, pal, c);
          *d++ = idx;
        }
        srcRow += srcStride;
        dst += dstStride;
      }
      break;
    }
  } else {
    switch (img->depth) {
    case 4:
    case 8: {
      RwRGBA *palette = img->palette;
      for (y = img->height; y != 0; y--) {
        unsigned char *d = dst;
        unsigned char *s = srcRow;
        unsigned int w = img->width;
        for (x = 0; x < w; x++) {
          unsigned char idx;
          RwRGBA *c = &palette[*s++];
          PalLookup(idx, pal, c);
          if (x & 1) {
            *d = (*d & 0xf) | (idx << 4);
            d++;
          } else {
            *d = (*d & 0xf0) | (idx & 0xf);
          }
        }
        srcRow += srcStride;
        dst += dstStride;
      }
      break;
    }
    case 32:
      for (y = img->height; y != 0; y--) {
        unsigned char *d = dst;
        RwRGBA *s = (RwRGBA *)srcRow;
        unsigned int w = img->width;
        for (x = 0; x < w; x++) {
          unsigned char idx;
          RwRGBA *c = s++;
          PalLookup(idx, pal, c);
          if (x & 1) {
            *d = (*d & 0xf) | (idx << 4);
            d++;
          } else {
            *d = (*d & 0xf0) | (idx & 0xf);
          }
        }
        srcRow += srcStride;
        dst += dstStride;
      }
      break;
    }
  }
}
