// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004ce970 (h3): 71.5% (regs 38.7%, ops 83.2%). RwImage read with mask: image = FUN_004ce390(name); no mask name -> image; mask = FUN_004ce390(maskname); FUN_004cde50(mask) (make mask) and FUN_004cdf20(image, mask) (apply) -> destroy mask, return image; otherwise destroy what was read and return 0. Image destroy is written as a macro (free pixels via RwGlobals+0x10c when flags&1, FUN_004d8060(&DAT_006182b4, img), freelist free +0x11c on module+0 with module = RwGlobals + DAT_007d4628). Residual: the original keeps image in esi / mask in edi (ours swapped), cross-jumps the mask==NULL path into the shared freelist-free tail (push esi; jmp), places the success path last, and schedules the three destroy copies differently. Tried: nested RW-style form (56.2%), early-return form (41.6%), if/else with a shared victim free (44.5%), static __inline destroy (not inlined under /Ob1, 19.7%), permuter 90 compiles (71.5%, this file: the failure path destroys mask first). Class (b). */
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
typedef struct Mod { void *freeList; } Mod;
typedef void (*fpFree)(void *mem);
typedef void (*fpFreeListFree)(void *fl, void *mem);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4628, 0x007d4628);
MGLOBAL(int, DAT_006182b4, 0x006182b4);
RwImage *FUN_004cde50(RwImage *image);
RwImage *FUN_004cdf20(RwImage *image, RwImage *mask);
RwImage *FUN_004ce390(const char *name);
int FUN_004d8060(void *reg, void *obj);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_007d4628))
#define IMAGEDESTROY(img) \
  if ((img)->flags & 1) { \
    (*(fpFree *)(DAT_007d3ff8 + 0x10c))((img)->cpPixels); \
    (img)->cpPixels = 0; \
    (img)->palette = 0; \
    (img)->flags &= ~1; \
  } \
  FUN_004d8060(&DAT_006182b4, (img)); \
  (*(fpFreeListFree *)(DAT_007d3ff8 + 0x11c))(MOD->freeList, (img))

static __forceinline int ImgDestroy(RwImage *img)
{
  IMAGEDESTROY(img);
  return 1;
}

// MATCH: 0x004ce970 FUN_004ce970
RwImage *FUN_004ce970(const char *param_1, const char *param_2)
{
  RwImage *image;
  RwImage *mask;

  image = FUN_004ce390(param_1);
  if (!image) goto ret0;
  if (param_2 && *param_2) {
    mask = FUN_004ce390(param_2);
    if (!mask) { ImgDestroy(image); goto ret0; }
    if (!FUN_004cde50(mask) || !FUN_004cdf20(image, mask)) { ImgDestroy(image); ImgDestroy(mask); goto ret0; }
    ImgDestroy(mask);
  }
  return image;
ret0:
  return 0;
}
