// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S19 0x004c6750 (h5): 83.3% (regs 85.6%). Texture image read + resample: name / mask name copied through the
   RwGlobals string functions (+0xd0 strncpy, +0xf4 strlen, +0xd4 strcat) with the 0x8000001e truncation RWERROR,
   extension from FUN_004ce6c0, image read FUN_004ce970, raster-size lookup FUN_004d5340 when *width or *height is 0
   (0x80000009 RWERROR on failure), early return when the size already matches, else convert to 32 bpp, resample
   (FUN_004dabd0) and for 4 / 8 bpp sources re-palettise through a local palette quantiser (FUN_004daa40 init,
   FUN_004d9050 add, FUN_004d9530 resolve 16 / 256, FUN_004da4f0 match, FUN_004daaf0 term).
   FLAGS: /O2 /Ob1 /G5 WITHOUT /GS: the original has no security cookie although it holds two char[256] buffers
   (with /GS: 69.6%), so the RW core is a /G5, no-/GS build (h5 finding). The quantiser struct is 0x4008 B (frame
   0x4610 matches; 0x4004 gives 0x460c).
   Residual: 9176 shrink-wraps the ebp/edi pushes past the first early return (original pushes all four callee-saved
   registers up front), which also changes the pop order of every early exit; the size-match early return gets its
   own epilogue (original joins the common exit). Tried: single-exit tail with `image` reassigned (70.3 -> 83.3%),
   old-image temp for the 32 bpp conversion (+6.7), flag variants (/Ox, /Ob2, /G4, /Gs, /Oy- worse), permuter
   561 + 240 compiles (its only gains were semantically wrong destroy orders, rejected). Class (b). */
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
typedef struct RwImage { int flags; int width; int height; int depth; int stride; unsigned char *cpPixels; RwRGBA *palette; } RwImage;
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct RwPalQuant { unsigned char data[0x4008]; } RwPalQuant;
typedef struct Pal16 { RwRGBA c[16]; } Pal16;
typedef struct Pal256 { RwRGBA c[256]; } Pal256;
typedef char *(*fp_strncpy)(char *, const char *, unsigned int);
typedef char *(*fp_strcat)(char *, const char *);
typedef unsigned int (*fp_strlen)(const char *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
const char *FUN_004ce6c0(const char *name);
RwImage *FUN_004ce970(const char *imageName, const char *maskName);
int FUN_004d5340(RwImage *image, int format, int *width, int *height, int *depth, int *flags);
int FUN_004cdd00(RwImage *image);
RwImage *FUN_004cdca0(int width, int height, int depth);
RwImage *FUN_004cdd60(RwImage *image);
RwImage *FUN_004cecb0(RwImage *dst, const RwImage *src);
RwImage *FUN_004dabd0(RwImage *dst, const RwImage *src);
int FUN_004daa40(RwPalQuant *pq);
void FUN_004d9050(RwPalQuant *pq, RwImage *img, float weight);
void FUN_004d9530(RwRGBA *palette, int depth, RwPalQuant *pq);
void FUN_004da4f0(unsigned char *pixels, int stride, int depth, int unk, RwPalQuant *pq, RwImage *img);
void FUN_004daaf0(RwPalQuant *pq);

// MATCH: 0x004c6750 FUN_004c6750
RwImage *FUN_004c6750(const char *name, const char *maskName, int format, int *width, int *height, int *depth, int *flags)
{
  char imageName[256];
  char maskImageName[256];
  RwRGBA palette[256];
  RwPalQuant palq;
  RwError err;
  const char *ext;
  RwImage *image;
  RwImage *newImage;
  RwImage *palImage;
  int origDepth;

  (*(fp_strncpy *)(DAT_007d3ff8 + 0xd0))(imageName, name, 256);
  if ((*(fp_strlen *)(DAT_007d3ff8 + 0xf4))(name) >= 256) {
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x8000001e, name, 256, 255, (int)name[255]);
    FUN_004d8480(&err);
    imageName[255] = 0;
  }
  ext = FUN_004ce6c0(name);
  if (ext) {
    (*(fp_strcat *)(DAT_007d3ff8 + 0xd4))(imageName, ext);
  }
  maskImageName[0] = 0;
  if (maskName && *maskName) {
    (*(fp_strncpy *)(DAT_007d3ff8 + 0xd0))(maskImageName, maskName, 256);
    if ((*(fp_strlen *)(DAT_007d3ff8 + 0xf4))(maskName) >= 256) {
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x8000001e, maskName, 256, 255, (int)maskName[255]);
      FUN_004d8480(&err);
      maskImageName[255] = 0;
    }
    ext = FUN_004ce6c0(maskName);
    if (ext) {
      (*(fp_strcat *)(DAT_007d3ff8 + 0xd4))(maskImageName, ext);
    }
  }
  image = FUN_004ce970(imageName, maskImageName);
  if (!image) {
    return 0;
  }
  if ((!*width || !*height) && !FUN_004d5340(image, format, width, height, depth, flags)) {
    FUN_004cdd00(image);
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x80000009);
    FUN_004d8480(&err);
    return 0;
  }
  if (image->width == *width && image->height == *height) {
    return image;
  }
  origDepth = image->depth;
  if (origDepth != 32) {
    RwImage *oldImage = image;
    image = FUN_004cdca0(oldImage->width, oldImage->height, 32);
    if (!image) {
      FUN_004cdd00(oldImage);
      return 0;
    }
    if (!FUN_004cdd60(image)) {
      FUN_004cdd00(image);
      FUN_004cdd00(oldImage);
      return 0;
    }
    FUN_004cecb0(image, oldImage);
    FUN_004cdd00(oldImage);
  }
  newImage = FUN_004cdca0(*width, *height, 32);
  if (!newImage) {
    FUN_004cdd00(image);
    return 0;
  }
  if (!FUN_004cdd60(newImage)) {
    FUN_004cdd00(newImage);
    FUN_004cdd00(image);
    return 0;
  }
  FUN_004dabd0(newImage, image);
  FUN_004cdd00(image);
  image = newImage;
  if (origDepth == 4) {
    if (FUN_004daa40(&palq)) {
      FUN_004d9050(&palq, image, 1.0f);
      FUN_004d9530(palette, 16, &palq);
      palImage = FUN_004cdca0(image->width, image->height, origDepth);
      if (palImage) {
        FUN_004cdd60(palImage);
        FUN_004da4f0(palImage->cpPixels, palImage->stride, palImage->depth, 0, &palq, image);
        *(Pal16 *)palImage->palette = *(Pal16 *)palette;
        FUN_004cdd00(image);
        image = palImage;
        FUN_004daaf0(&palq);
      }
    }
  } else if (origDepth == 8) {
    if (FUN_004daa40(&palq)) {
      FUN_004d9050(&palq, image, 1.0f);
      FUN_004d9530(palette, 256, &palq);
      palImage = FUN_004cdca0(image->width, image->height, origDepth);
      if (palImage) {
        FUN_004cdd60(palImage);
        FUN_004da4f0(palImage->cpPixels, palImage->stride, palImage->depth, 0, &palq, image);
        *(Pal256 *)palImage->palette = *(Pal256 *)palette;
        FUN_004cdd00(image);
        image = palImage;
        FUN_004daaf0(&palq);
      }
    }
  }
  return image;
}
