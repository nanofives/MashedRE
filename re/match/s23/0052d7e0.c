// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052d7e0: 100% (regs 100%). BMP/image writer row emitter (no library source): packs a 4-bit row,
   copies an 8-bit row (memcpy intrinsic = rep movsd/movsb) or swaps a 32-bit row to 24-bit BGR, writes it with
   FUN_004cbe80 and pads to a multiple of 4 with single zero bytes. raster: +0x04 width, +0x10 stride,
   +0x14 pixels. What moved it: the zero pad byte lives in the dead `ras` argument slot ([esp+0x18]), so the
   source stores the byte through &ras (a separate local gave 98.0%). Residual: none. */
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
void *FUN_004cbe80(void *stream, void *buf, unsigned size);
void *memcpy(void *dst, const void *src, unsigned int n);
#pragma intrinsic(memcpy)

typedef struct {
    int pad0;
    int width;                  /* 0x04 */
    int pad8[2];
    int stride;                 /* 0x10 */
    unsigned char *pixels;      /* 0x14 */
} raster_t;

// MATCH: 0x0052d7e0 FUN_0052d7e0
unsigned FUN_0052d7e0(void *stream, raster_t *ras, int row, int depth, unsigned char *out)
{
    unsigned char *src;
    unsigned n;
    int i;

    if (depth == 4) {
        src = ras->pixels + ras->stride * row;
        for (i = 0; i < ras->width; i += 2)
            out[i >> 1] = (unsigned char)((src[i] << 4) | src[i + 1]);
        n = ras->width >> 1;
    } else if (depth == 8) {
        src = ras->pixels + ras->stride * row;
        memcpy(out, src, ras->width);
        n = ras->width;
    } else {
        src = ras->pixels + ras->stride * row;
        for (i = 0; i < ras->width; i++) {
            out[i * 3 + 2] = src[i * 4 + 0];
            out[i * 3 + 1] = src[i * 4 + 1];
            out[i * 3 + 0] = src[i * 4 + 2];
        }
        n = ras->width * 3;
    }
    FUN_004cbe80(stream, out, n);
    for (; (n & 3) != 0; n++) {
        *(unsigned char *)&ras = 0;
        FUN_004cbe80(stream, &ras, 1);
    }
    return n;
}
