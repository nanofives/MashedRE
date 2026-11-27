// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 0x004e9e40, rewritten by RACE-LOAD-004E99B0 (run 57, 2026-11-27) from the disassembly of
   0x004e9e40..0x004ea213 (Ghidra pool slot Mashed_pool0, read-only). The earlier h1 body (34.4%) read the 0x2c-byte chunk
   header through separate locals (FUN_004cbd30 into a 2-short array). Frame (S = esp after `sub esp,0x38`):
     S+0x00 struct chunk version (0x004e9e43, FUN_004cc5e0(stream, 1, 0, &version))
     S+0x04 2-dword error record {2, FUN_004d7ff0(code, ...)} (0x004e9eb6 / 0x004e9ecd)
     S+0x0c 0x2c-byte header read by FUN_004cbd30 (0x004e9e80)
   Args: stream, int *next (memory cursor), world, format. Every cursor take of n bytes is p = *next; *next = p + n; on
   p == 0 the error is FUN_004d7ff0(0x80000013, n) (the n push is shared with the read call, 0x004e9f95 / 0x004e9fce /
   0x004ea08a / 0x004ea0e2). Failure after the take: 0x004e4f70(sector) when world byte +3 bit 0, else 0x004e5020(sector)
   (0x004e9ffd). Version 0x36002: triangle words rotated (w1, w2, w3, w0) when the chunk version is below it (0x004ea124). */
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
MGLOBAL(int, DAT_006189e4, 0x006189e4);
MGLOBAL(int, DAT_007d727c, 0x007d727c);
MGLOBAL(int, DAT_007d7270, 0x007d7270);

/* the 0x2c-byte struct chunk body (S+0x0c); field names are offsets */
typedef struct {
    int h00;          /* low word -> sector +0x80 (0x004e9f1b) */
    int h04;          /* low word -> sector +0x84 (0x004e9f27); count * 8 bytes (0x004ea0ca) */
    int h08;          /* low word -> sector +0x82 (0x004e9f33); count * 12 / 4 / 4 bytes (0x004e9f7e) */
    int h0c[3];       /* -> sector +0x6c..+0x77 (0x004e9eea) */
    int h18[3];       /* -> sector +0x60..+0x6b (0x004e9f07) */
    int h24[2];       /* not read */
} Hdr_004e9e40;

typedef struct { int a; int b; } Err_004e9e40;

int FUN_004cc5e0(int stream, int type, uint *size, uint *version);
uint FUN_004cbd30(int stream, void *buf, uint size);
int FUN_004cc790(int stream, int buf, uint size);
int FUN_004d7ff0(int code, ...);
void FUN_004d8480(Err_004e9e40 *err);
void FUN_004e4f70(int sector);
void FUN_004e5020(int sector);
void FUN_004d8000(int *reg, int obj);
int FUN_004e1b60(int *reg, int stream, int obj);
void FUN_004e1c90(int *reg, int p1, int obj, int p3);

// MATCH: 0x004e9e40 FUN_004e9e40
int FUN_004e9e40(int stream, int *next, int world, uint format)
{
    uint version;
    Err_004e9e40 err;
    Hdr_004e9e40 h;
    int sector;
    int p;
    uint n;
    int i;

    if (!FUN_004cc5e0(stream, 1, 0, &version)) {
        return 0;
    }
    if (version < 0x35000 || version > 0x37002) {
        err.a = 2;
        err.b = FUN_004d7ff0(0x80000004);
        FUN_004d8480(&err);
        return 0;
    }
    if (FUN_004cbd30(stream, &h, 0x2c) != 0x2c) {
        return 0;
    }
    sector = *next;
    *next = sector + DAT_006189e4;
    if (sector == 0) {
        err.a = 2;
        err.b = FUN_004d7ff0(0x80000013, DAT_006189e4);
        FUN_004d8480(&err);
        return 0;
    }
    *(int *)sector = -1;
    *(int *)(sector + 0x6c) = h.h0c[0];
    *(int *)(sector + 0x70) = h.h0c[1];
    *(int *)(sector + 0x74) = h.h0c[2];
    *(int *)(sector + 0x60) = h.h18[0];
    *(int *)(sector + 0x64) = h.h18[1];
    *(int *)(sector + 0x68) = h.h18[2];
    *(short *)(sector + 0x80) = (short)h.h00;
    *(short *)(sector + 0x84) = (short)h.h04;
    *(short *)(sector + 0x82) = (short)h.h08;
    *(int *)(sector + 0x4) = 0;
    *(int *)(sector + 0x8) = 0;
    *(int *)(sector + 0x30) = 0;
    *(int *)(sector + 0xc) = 0;
    for (i = 0; i < 8; i++) {
        *(int *)(sector + 0x10 + i * 4) = 0;
    }
    *(int *)(sector + 0x3c) = sector + 0x38;
    *(int *)(sector + 0x7c) = 0;
    *(int *)(sector + 0x38) = sector + 0x38;
    *(int *)(sector + 0x34) = 0;
    *(int *)(sector + 0x78) = 0;
    *(int *)(sector + 0x40) = sector + 0x40;
    *(int *)(sector + 0x44) = sector + 0x40;
    if (!(*(uint *)(world + 8) & 0x1000000)) {
        if (h.h08 != 0) {
            n = h.h08 * 12;
            p = *next;
            *next = p + n;
            *(int *)(sector + 0x8) = p;
            if (p == 0) {
                goto nomem;
            }
            if (!FUN_004cc790(stream, p, n)) {
                goto fail;
            }
            if (format & 0x10) {
                n = h.h08 * 4;
                p = *next;
                *next = p + n;
                *(int *)(sector + 0xc) = p;
                if (p == 0) {
                    goto nomem;
                }
                if (FUN_004cbd30(stream, (void *)p, n) != n) {
                    goto fail;
                }
            }
            if (format & 0x8) {
                n = h.h08 * 4;
                p = *next;
                *next = p + n;
                *(int *)(sector + 0x30) = p;
                if (p == 0) {
                    goto nomem;
                }
                if (FUN_004cbd30(stream, (void *)p, n) == 0) {
                    goto fail;
                }
            }
            if (*(int *)(world + 0x20) > 0) {
                n = (uint)*(unsigned short *)(sector + 0x82) * 8;
                for (i = 0; i < *(int *)(world + 0x20); i++) {
                    p = *next;
                    *next = p + n;
                    *(int *)(sector + 0x10 + i * 4) = p;
                    if (p == 0) {
                        goto nomem;
                    }
                    if (!FUN_004cc790(stream, p, n)) {
                        goto fail;
                    }
                }
            }
        }
        if (h.h04 != 0) {
            n = h.h04 * 8;
            p = *next;
            *next = p + n;
            *(int *)(sector + 0x4) = p;
            if (p == 0) {
                goto nomem;
            }
            if (!FUN_004cc790(stream, p, n)) {
                goto fail;
            }
            if (version < 0x36002) {
                for (i = 0; i < h.h04; i++) {
                    unsigned short *t = (unsigned short *)(*(int *)(sector + 0x4) + i * 8);
                    unsigned short w0 = t[0], w1 = t[1], w2 = t[2], w3 = t[3];
                    t[0] = w1;
                    t[1] = w2;
                    t[2] = w3;
                    t[3] = w0;
                }
            }
        }
    }
    FUN_004d8000(&DAT_006189e4, sector);
    DAT_007d727c = 0;
    DAT_007d7270 = 0;
    if (!FUN_004e1b60(&DAT_006189e4, stream, sector)) {
        goto fail;
    }
    if (DAT_007d727c != 0) {
        FUN_004e1c90(&DAT_006189e4, DAT_007d727c, sector, DAT_007d7270);
    }
    return sector;
nomem:
    err.a = 2;
    err.b = FUN_004d7ff0(0x80000013, n);
    FUN_004d8480(&err);
fail:
    if (*(unsigned char *)(world + 3) & 1) {
        FUN_004e4f70(sector);
        return 0;
    }
    FUN_004e5020(sector);
    return 0;
}
