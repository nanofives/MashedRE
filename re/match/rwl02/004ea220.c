// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 0x004ea220, rewritten by RACE-LOAD-004E99B0 (run 57, 2026-11-27) from the disassembly of
   0x004ea220..0x004ea4b6 (Ghidra pool slot Mashed_pool0, read-only). The earlier h1 body (46.5%) read the 0x18-byte chunk
   header through separate locals. Frame (S = esp after `sub esp,0x24`):
     S+0x00 struct chunk size, S+0x28 (the stream argument's slot, the stream is in EBX first) its version (0x004ea223)
     S+0x04 2-dword error record {2, FUN_004d7ff0(code, ...)} (0x004ea2a4)
     S+0x0c 0x18-byte header read by FUN_004cbd30 (zeroed first, rep stosd 6 at 0x004ea26d)
   Args: stream, int *next (memory cursor), world, format. Takes 0x18 bytes of the cursor (0x004ea296). Child 0 -> +0x08 and
   child 1 -> +0x0c: chunk 9 + 0x004e9e40 when header +0x08 / +0x0c != 0, else chunk 10 + this function. Any child failure:
   when +0x08 != 0, 0x004e4f70(+0x08) if world byte +3 bit 0 else 0x004e5020(+0x08), then +0x08 = 0 (0x004ea43e). */
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
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)

/* the 0x18-byte struct chunk body (S+0x0c); field names are offsets */
typedef struct {
    int h00;          /* -> plane +0x00 (0x004ea2cc) */
    int h04;          /* -> plane +0x04, and +0x10 / +0x14 when format bit 0x40000000 is clear (0x004ea2d6) */
    int h08;          /* != 0: child 0 is a chunk-9 sector (0x004ea302) */
    int h0c;          /* != 0: child 1 is a chunk-9 sector (0x004ea3a2) */
    int h10;          /* -> plane +0x10 when format bit 0x40000000 (0x004ea2e4) */
    int h14;          /* -> plane +0x14 (0x004ea2eb) */
} Hdr_004ea220;

typedef struct { int a; int b; } Err_004ea220;

int FUN_004cc5e0(int stream, int type, uint *size, uint *version);
uint FUN_004cbd30(int stream, void *buf, uint size);
int FUN_004d7ff0(int code, ...);
void FUN_004d8480(Err_004ea220 *err);
int FUN_004e9e40(int stream, int *next, int world, uint format);
int FUN_004ea220(int stream, int *next, int world, uint format);
void FUN_004e4f70(int sector);
void FUN_004e5020(int sector);

// MATCH: 0x004ea220 FUN_004ea220
int FUN_004ea220(int stream, int *next, int world, uint format)
{
    uint size;
    uint version;
    Err_004ea220 err;
    Hdr_004ea220 h;
    int plane;
    int child;

    if (!FUN_004cc5e0(stream, 1, &size, &version)) {
        return 0;
    }
    if (version < 0x35000 || version > 0x37002) {
        err.a = 2;
        err.b = FUN_004d7ff0(0x80000004);
        FUN_004d8480(&err);
        return 0;
    }
    memset(&h, 0, sizeof(h));
    if (FUN_004cbd30(stream, &h, size) != size) {
        return 0;
    }
    plane = *next;
    *next = plane + 0x18;
    if (plane == 0) {
        err.a = 2;
        err.b = FUN_004d7ff0(0x80000013, 0x18);
        FUN_004d8480(&err);
        return 0;
    }
    *(int *)(plane + 0x0) = h.h00;
    *(int *)(plane + 0x4) = h.h04;
    if (format & 0x40000000) {
        *(int *)(plane + 0x10) = h.h10;
        *(int *)(plane + 0x14) = h.h14;
    } else {
        *(int *)(plane + 0x10) = h.h04;
        *(int *)(plane + 0x14) = h.h04;
    }
    if (h.h08 != 0) {
        if (!FUN_004cc5e0(stream, 9, 0, &version)) {
            goto fail;
        }
        if (version < 0x35000 || version > 0x37002) {
            goto bad_version;
        }
        child = FUN_004e9e40(stream, next, world, format);
    } else {
        if (!FUN_004cc5e0(stream, 10, 0, &version)) {
            goto fail;
        }
        if (version < 0x35000 || version > 0x37002) {
            goto bad_version;
        }
        child = FUN_004ea220(stream, next, world, format);
    }
    *(int *)(plane + 0x8) = child;
    if (child == 0) {
        goto fail;
    }
    if (h.h0c != 0) {
        if (!FUN_004cc5e0(stream, 9, 0, &version)) {
            goto fail;
        }
        if (version < 0x35000 || version > 0x37002) {
            goto bad_version;
        }
        child = FUN_004e9e40(stream, next, world, format);
    } else {
        if (!FUN_004cc5e0(stream, 10, 0, &version)) {
            goto fail;
        }
        if (version < 0x35000 || version > 0x37002) {
            goto bad_version;
        }
        child = FUN_004ea220(stream, next, world, format);
    }
    *(int *)(plane + 0xc) = child;
    if (child != 0) {
        return plane;
    }
    goto fail;
bad_version:
    err.a = 2;
    err.b = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&err);
fail:
    if (*(int *)(plane + 0x8) != 0) {
        if (*(unsigned char *)(world + 3) & 1) {
            FUN_004e4f70(*(int *)(plane + 0x8));
        } else {
            FUN_004e5020(*(int *)(plane + 0x8));
        }
        *(int *)(plane + 0x8) = 0;
    }
    return 0;
}

