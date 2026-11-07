// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052d170: 57.7% (regs 73.0%). BMP/image row converter (no library source): switch on
   depth | srcDepth << 8 (0x104/0x108 1-bit, 0x120 1-bit paletted, 0x404/0x408 4-bit, 0x420 4-bit paletted, 0x808
   memcpy, 0x820 8-bit paletted, 0xf20 555, 0x1020 565, 0x1820 24-bit with a swap flag, 0x2020 32-bit), RW error
   0x19d / FUN_004d7ff0(0x80000008) otherwise. image: +4 width, +0xc depth, +0x10 stride, +0x14 pixels,
   +0x18 palette. The case tree, loop shapes and stores match (ops 90%); what moved it: unsigned char b, a d32
   pointer and a separate counter in the paletted cases (49.0 -> 55.2), permuter 1664 compiles (57.7).
   Residual: register and dead-arg-slot assignment (the original keeps b in src's slot and the counter in img's,
   ours the other way; dst in eax, ours in edi/edx); argument copies (all 32 subsets) and statement orders of the
   prologue did not move it. Class (b). */
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
typedef struct { int pluginID; int errorCode; } RwError;
typedef struct {
    int pad0;
    unsigned width;             /* 0x04 */
    int pad8;
    unsigned depth;             /* 0x0c */
    int stride;                 /* 0x10 */
    unsigned char *pixels;      /* 0x14 */
    unsigned *palette;          /* 0x18 */
} image_t;

int FUN_004d7ff0(int code);
RwError *FUN_004d8480(RwError *err);
void *memcpy(void *dst, const void *src, unsigned int n);
#pragma intrinsic(memcpy)

// MATCH: 0x0052d170 FUN_0052d170
int FUN_0052d170(image_t *img, int row, unsigned char *src, int srcDepth, int swap)
{
    unsigned *pal = img->palette;
    unsigned n = img->width;
    unsigned char *dst = img->pixels + img->stride * row;
    unsigned *d32 = (unsigned *)dst;
    unsigned i;
    int bit;
    unsigned char b;
    short v;
    RwError err;

    switch (img->depth | (srcDepth << 8)) {
    case 0x104:
    case 0x108:
        bit = 7;
        b = *src++;
        for (; n != 0; n--) {
            *dst++ = (unsigned char)((b >> bit) & 1);
            if (--bit <= -1) {
                bit = 7;
                b = *src++;
            }
        }
        break;
    case 0x120:
        bit = 7;
        b = *src++;
        for (i = n; i != 0; i--) {
            *d32++ = pal[(b >> bit--) & 1];
            if (bit < 0) {
                bit = 7;
                b = *src++;
            }
        }
        break;
    case 0x404:
    case 0x408:
        bit = 4;
        b = *src++;
        for (; n != 0; n--) {
            *dst++ = (unsigned char)((b >> bit) & 0xf);
            bit -= 4;
            if (bit < 0) {
                bit = 4;
                b = *src++;
            }
        }
        break;
    case 0x420:
        bit = 4;
        b = *src++;
        for (i = n; i != 0; i--) {
            *d32++ = pal[(b >> bit) & 0xf];
            bit -= 4;
            if (bit < 0) {
                bit = 4;
                b = *src++;
            }
        }
        break;
    case 0x808:
        memcpy(dst, src, n);
        break;
    case 0x820:
        for (; n != 0; n--) {
            *(unsigned *)dst = pal[*src++];
            dst += 4;
        }
        break;
    case 0xf20:
        for (; n != 0; n--) {
            v = *(unsigned short *)src;
            src += 2;
            dst[3] = 0xff;
            dst[0] = (unsigned char)((v >> 7) & 0xf8);
            dst[1] = (unsigned char)((v >> 2) & 0xf8);
            dst += 4;
            dst[2] = (unsigned char)(v << 3);
        }
        break;
    case 0x1020:
        for (; n != 0; n--) {
            v = *(unsigned short *)src;
            src += 2;
            dst[3] = 0xff;
            dst[0] = (unsigned char)((v >> 8) & 0xf8);
            dst[1] = (unsigned char)((v >> 3) & 0xfc);
            dst[2] = (unsigned char)(v << 3);
            dst += 4;
        }
        break;
    case 0x1820:
        if (swap) {
            for (; n != 0; n--) {
                dst[1] = src[1];
                dst[3] = 0xff;
                dst[0] = src[0];
                dst[2] = src[2];
                src += 3;
                dst += 4;
            }
        } else {
            for (; n != 0; n--) {
                dst[2] = src[0];
                dst[1] = src[1];
                dst[3] = 0xff;
                dst[0] = src[2];
                src += 3;
                dst += 4;
            }
        }
        break;
    case 0x2020:
        for (; n != 0; n--) {
            dst[0] = src[0];
            dst[1] = src[1];
            dst[3] = 0xff;
            dst[2] = src[2];
            src += 4;
            dst += 4;
        }
        break;
    default:
        err.pluginID = 0x19d;
        err.errorCode = FUN_004d7ff0(0x80000008);
        FUN_004d8480(&err);
        return 0;
    }
    return 1;
}
