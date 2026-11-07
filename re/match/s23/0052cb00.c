// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052cb00: 22.9% (regs 43.7%, ops 58.2%). BMP file reader (RtBMPImageRead shape, no source): RwStream
   open/read/skip/close (FUN_004cc230 / 004cbd30 / 004cc050 / 004cc160), 'BM' magic, core (12) and info (0x28)
   headers, palette to RGBA, RwImageCreate/AllocatePixels/Destroy (004cdca0 / 004cdd60 / 004cdd00), RwMalloc /
   RwFree via RwEngineInstance 0x007d3ff8 +0x108/+0x10c, rows through FUN_0052d170, BI_RGB and BI_RLE8 (the RLE
   code bytes reuse the RwError slot), RW error 0x19d with 0x80000009 / 0x80000013. Control flow written from the
   decomp; one pass only (function > 1000 B). Residual: the original keeps every `close(stream); return 0` exit
   as its own block with its own epilogue (545 vs our 473 instructions: ours tail-merges the identical exits),
   a larger frame (0x454 vs 0x440: width/height/bpp/compression kept in frame slots, bpp stored as a word), and
   the register assignment that follows. Class (b) (budget; the tail-merge part may be (a)). */
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
typedef struct RwStream RwStream;
typedef struct {
    int pad0[6];
    unsigned char *palette;     /* 0x18 */
} RwImage;
typedef void *(*rwmalloc_t)(unsigned size, unsigned hint);
typedef void (*rwfree_t)(void *p);

MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);

unsigned FUN_004cbd30(RwStream *stream, void *buffer, unsigned length);
RwStream *FUN_004cc050(RwStream *stream, unsigned offset);
int FUN_004cc160(RwStream *stream, void *data);
RwStream *FUN_004cc230(int type, int accessType, const void *data);
RwImage *FUN_004cdca0(int width, int height, int depth);
int FUN_004cdd00(RwImage *image);
RwImage *FUN_004cdd60(RwImage *image);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
int FUN_0052d170(RwImage *img, int row, unsigned char *src, int srcDepth, int swap);
void *memset(void *dst, int c, unsigned int n);
#pragma intrinsic(memset)

// MATCH: 0x0052cb00 FUN_0052cb00
RwImage *FUN_0052cb00(const char *name)
{
    RwStream *stream;
    unsigned magic[4];
    unsigned char buf[0x400];
    int offBits;
    int hsize;
    int pos;
    int width;
    int height;
    short bpp;
    int compression;
    int ncolors;
    int bppi;
    int depth;
    int n;
    int i;
    int y;
    int x;
    int rowBytes;
    unsigned size;
    RwImage *image;
    unsigned char *pal;
    unsigned char *p;
    unsigned char *buffer;
    RwError err;
    unsigned char *code;

    stream = FUN_004cc230(2, 1, name);
    if (stream == 0)
        return 0;
    if (FUN_004cbd30(stream, magic, 4) != 4) {
        FUN_004cc160(stream, 0);
        return 0;
    }
    if ((magic[0] & 0xffff) != 0x4d42) {
        FUN_004cc160(stream, 0);
        return 0;
    }
    if (FUN_004cbd30(stream, buf + 2, 14) != 14) {
        FUN_004cc160(stream, 0);
        return 0;
    }
    offBits = *(int *)(buf + 8);
    hsize = *(int *)(buf + 0xc);
    if (FUN_004cbd30(stream, buf, hsize - 4) != (unsigned)(hsize - 4)) {
        FUN_004cc160(stream, 0);
        return 0;
    }
    pos = hsize + 14;
    if (hsize == 12) {
        width = *(short *)(buf + 0);
        height = *(short *)(buf + 2);
        bpp = *(short *)(buf + 6);
        compression = 0;
        ncolors = 1 << bpp;
    } else {
        width = *(int *)(buf + 0);
        height = *(int *)(buf + 4);
        bpp = *(short *)(buf + 0xa);
        compression = *(int *)(buf + 0xc);
        if (compression == 2) {
            err.pluginID = 0x19d;
            err.errorCode = FUN_004d7ff0(0x80000009);
            FUN_004d8480(&err);
            FUN_004cc160(stream, 0);
            return 0;
        }
        ncolors = *(int *)(buf + 0x1c);
        if (ncolors < 1 || (1 << bpp) < ncolors)
            ncolors = 1 << bpp;
    }
    bppi = bpp;
    if (bppi == 1 || bppi == 4)
        depth = 4;
    else if (bppi == 8)
        depth = 8;
    else
        depth = 32;
    image = FUN_004cdca0(width, height, depth);
    if (image == 0) {
        FUN_004cc160(stream, 0);
        return 0;
    }
    if (FUN_004cdd60(image)) {
        if (bpp < 9) {
            pal = image->palette;
            if (hsize == 12) {
                n = ncolors * 3;
                if (FUN_004cbd30(stream, buf, n) != (unsigned)n) {
                    FUN_004cc160(stream, 0);
                    return 0;
                }
                pos = n + 0x1a;
                for (i = ncolors, p = buf; i > 0; i--, p += 3, pal += 4) {
                    pal[0] = p[2];
                    pal[1] = p[1];
                    pal[2] = p[0];
                    pal[3] = 0xff;
                }
            } else {
                if (hsize != 0x28) {
                    err.pluginID = 0x19d;
                    err.errorCode = FUN_004d7ff0(0x80000009);
                    FUN_004d8480(&err);
                    FUN_004cc160(stream, 0);
                    return 0;
                }
                n = ncolors * 4;
                if (FUN_004cbd30(stream, buf, n) != (unsigned)n) {
                    FUN_004cc160(stream, 0);
                    return 0;
                }
                pos = n + 0x36;
                for (i = 0; i < ncolors; i++, pal += 4) {
                    pal[0] = buf[i * 4 + 2];
                    pal[1] = buf[i * 4 + 1];
                    pal[2] = buf[i * 4 + 0];
                    pal[3] = 0xff;
                }
            }
        }
        if (FUN_004cc050(stream, offBits - pos)) {
            rowBytes = ((bppi * width + 7) / 8 + 3) & ~3;
            size = ((width + 7) & ~7) * ((bppi + 7) / 8);
            buffer = (unsigned char *)(*(rwmalloc_t *)(DAT_007d3ff8 + 0x108))(size, 0x10000);
            if (buffer == 0) {
                err.pluginID = 0x19d;
                err.errorCode = FUN_004d7ff0(0x80000013, size);
                FUN_004d8480(&err);
                FUN_004cdd00(image);
                FUN_004cc160(stream, 0);
                return 0;
            }
            if (compression == 0 || bpp == 24 || bpp == 32) {
                for (y = 0; y < height; y++) {
                    if (FUN_004cbd30(stream, buffer, rowBytes) != (unsigned)rowBytes) {
                        (*(rwfree_t *)(DAT_007d3ff8 + 0x10c))(buffer);
                        FUN_004cdd00(image);
                        FUN_004cc160(stream, 0);
                        return 0;
                    }
                    FUN_0052d170(image, height - y - 1, buffer, bppi, 0);
                }
            } else {
                if (compression != 1) {
                    err.pluginID = 0x19d;
                    err.errorCode = FUN_004d7ff0(0x80000009);
                    FUN_004d8480(&err);
                    (*(rwfree_t *)(DAT_007d3ff8 + 0x10c))(buffer);
                    FUN_004cdd00(image);
                    FUN_004cc160(stream, 0);
                    return 0;
                }
                code = (unsigned char *)&err;
                for (y = 0; y < height; y++) {
                    x = 0;
                    for (;;) {
                        if (FUN_004cbd30(stream, code, 2) != 2) {
                            (*(rwfree_t *)(DAT_007d3ff8 + 0x10c))(buffer);
                            FUN_004cdd00(image);
                            FUN_004cc160(stream, 0);
                            return 0;
                        }
                        if (code[0] != 0) {
                            memset(buffer + x, code[1], code[0]);
                            x += code[0];
                        } else if (code[1] >= 3) {
                            if (FUN_004cbd30(stream, buffer + x, code[1]) != code[1]) {
                                (*(rwfree_t *)(DAT_007d3ff8 + 0x10c))(buffer);
                                FUN_004cdd00(image);
                                FUN_004cc160(stream, 0);
                                return 0;
                            }
                            x += code[1];
                            if ((code[1] & 1) && FUN_004cbd30(stream, code, 1) != 1) {
                                (*(rwfree_t *)(DAT_007d3ff8 + 0x10c))(buffer);
                                FUN_004cdd00(image);
                                FUN_004cc160(stream, 0);
                                return 0;
                            }
                        } else
                            break;
                    }
                    if (code[1] != 0) {
                        if (code[1] != 1) {
                            (*(rwfree_t *)(DAT_007d3ff8 + 0x10c))(buffer);
                            FUN_004cdd00(image);
                            FUN_004cc160(stream, 0);
                            return 0;
                        }
                        y = height - 1;
                    }
                    FUN_0052d170(image, height - y - 1, buffer, bppi, 1);
                }
            }
            (*(rwfree_t *)(DAT_007d3ff8 + 0x10c))(buffer);
            FUN_004cc160(stream, 0);
            return image;
        }
    }
    FUN_004cdd00(image);
    FUN_004cc160(stream, 0);
    return 0;
}
