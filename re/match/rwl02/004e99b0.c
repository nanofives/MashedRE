// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 0x004e99b0, rewritten by RACE-LOAD-004E99B0 (run 57, 2026-11-27) from the disassembly of
   0x004e99b0..0x004e9e33 (Ghidra pool slot Mashed_pool0, read-only). The earlier raw g2s.py body (45.5%) read the 0x40-byte
   chunk header through 16 separate locals and stored float products into dword slots, so as C it did not compute what
   the original computes. Frame (S = esp after `sub esp,0x58` at 0x004e99b0):
     S+0x00 chunk version of the 8/9/10 finds (0x004e9b45 / 0x004e9bf5)
     S+0x04 struct chunk size, S+0x0c its version (0x004e99b3 / 0x004e99b7, FUN_004cc5e0(stream, 1, &size, &version))
     S+0x08 next-memory pointer = world + DAT_0061864c (0x004e9b0a), passed by address to 0x004e9e40 / 0x004ea220
     S+0x10 header +0x24 copy (0x004e9a51), later the 2-dword error record {2, FUN_004d7ff0(code)} (0x004e9e0c / 0x004e9e1d)
     S+0x18 0x40-byte header read by FUN_004cbd30 (zeroed first, rep stosd 0x10 at 0x004e99fd)
   Constants: 0x35000 / 0x37002 version range (0x004e99dc / 0x004e99e7), 0x3000b alloc hint (0x004e9aab), error codes
   0x80000013 (0x004e9ac1) / 0x80000004 (0x004e9e07), DAT_005cc33c = 0xbf800000 (-1.0f) multiplier (0x004e9b3f).
   Callees: 0x004cc5e0, 0x004cbd30, [DAT_007d3ff8 + 0x108], 0x004d7ff0, 0x004d8480, 0x004e5280, 0x004e5700 (on every
   failure after the alloc), 0x004f3e90, 0x004e9e40 (header +0 != 0), 0x004ea220 (header +0 == 0), 0x004e51e0,
   0x004e5820, 0x004e8090, 0x004d8000, 0x004e1b60, 0x004e1c90, 0x004e5300. */
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
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memset)
#pragma intrinsic(memcpy)
MGLOBAL(int, DAT_006189e4, 0x006189e4);
MGLOBAL(int, DAT_0061864c, 0x0061864c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, DAT_005cc33c, 0x005cc33c);
MGLOBAL(int, DAT_007d7278, 0x007d7278);
MGLOBAL(int, DAT_007d7274, 0x007d7274);

/* the 0x40-byte struct chunk body (S+0x18); field names are offsets */
typedef struct {
    int h00;          /* != 0: chunk 9 / 0x004e9e40, else chunk 10 / 0x004ea220 (0x004e9bef) */
    float h04;        /* * DAT_005cc33c -> world +0x44 (0x004e9b3b) */
    float h08;        /* -> world +0x48 (0x004e9b52) */
    float h0c;        /* -> world +0x4c (0x004e9b62) */
    int h10;          /* size term * 8 (0x004e9a9e) */
    int h14;          /* size terms * 12 / 4 / 4 / sets*8 (0x004e9a7a) */
    int h18;          /* size term * 0x18 (0x004e9a30) */
    int h1c;          /* size term * DAT_006189e4 (0x004e9a34) */
    int h20;          /* > 0: error 0x80000004 (0x004e9a1e) */
    uint h24;         /* -> world +0x08 (0x004e9a47, 0x004e9b35) */
    uint h28[6];      /* -> world +0x50..+0x67 (0x004e9cca) */
} Hdr_004e99b0;

typedef struct { int a; int b; } Err_004e99b0;

int FUN_004cc5e0(int stream, int type, uint *size, uint *version);
uint FUN_004cbd30(int stream, void *buf, uint size);
int FUN_004d7ff0(int code);
void FUN_004d8480(Err_004e99b0 *err);
void FUN_004e5280(int world, uint size);
int FUN_004e5700(int world);
int FUN_004f3e90(int stream, int dst);
int FUN_004e9e40(int stream, int *next, int world, uint fmt);
int FUN_004ea220(int stream, int *next, int world, uint fmt);
void FUN_004e51e0(int world);
void FUN_004e5820(int world, int p1);
void FUN_004e8090(int *p0, int p1);
void FUN_004d8000(int *reg, int world);
int FUN_004e1b60(int *reg, int stream, int world);
void FUN_004e1c90(int *reg, int p1, int world, int p3);
int FUN_004e5300(int world);

typedef void *(*Alloc_004e99b0)(uint size, uint hint);

// MATCH: 0x004e99b0 FUN_004e99b0
int FUN_004e99b0(int stream)
{
    uint version2;
    uint size;
    int next;
    uint version;

    Err_004e99b0 err;
    Hdr_004e99b0 h;
    uint bytes;
    uint sets;
    int world;
    int i;

    if (!FUN_004cc5e0(stream, 1, &size, &version)) {
        return 0;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        memset(&h, 0, sizeof(h));
        if (FUN_004cbd30(stream, &h, size) != size) {
            return 0;
        }
        if (h.h20 <= 0) {
            bytes = DAT_0061864c + DAT_006189e4 * h.h1c + h.h18 * 0x18;
            
            if (h.h24 & 0xff0000) {
                sets = (h.h24 >> 16) & 0xff;
            } else if (h.h24 & 0x80) {
                sets = 2;
            } else {
                sets = (h.h24 >> 2) & 1;
            }
            if (!(h.h24 & 0x1000000)) {
                bytes += h.h14 * 12;
                if (h.h24 & 0x10) {
                    bytes += h.h14 * 4;
                }
                if (h.h24 & 0x8) {
                    bytes += h.h14 * 4;
                }
                if (sets != 0) {
                    bytes += h.h14 * sets * 8;
                }
                bytes += h.h10 * 8;
            }
            world = (int)(*(Alloc_004e99b0 *)(DAT_007d3ff8 + 0x108))(bytes, 0x3000b);
            if (world == 0) {
                err.a = 2;
                err.b = FUN_004d7ff0(0x80000013);
                FUN_004d8480(&err);
                return 0;
            }
            memset((void *)world, 0, bytes);
            next = DAT_0061864c + world;
            *(char *)(world + 0) = 7;
            *(char *)(world + 1) = 0;
            *(char *)(world + 2) = 0;
            *(int *)(world + 4) = 0;
            *(char *)(world + 3) = 1;
            FUN_004e5280(world, bytes);
            *(int *)(world + 0xc) = 2;
            *(uint *)(world + 0x8) = h.h24;
            *(float *)(world + 0x44) = h.h04 * DAT_005cc33c;
            *(float *)(world + 0x48) = h.h08 * DAT_005cc33c;
            *(float *)(world + 0x4c) = h.h0c * DAT_005cc33c;
            *(uint *)(world + 0x20) = sets;
            *(int *)(world + 0x34) = world + 0x34;
            *(int *)(world + 0x38) = world + 0x34;
            *(int *)(world + 0x1c) = 0;
            *(int *)(world + 0x6c) = 0;
            *(int *)(world + 0x2c) = world + 0x2c;
            *(int *)(world + 0x30) = world + 0x2c;
            *(int *)(world + 0x3c) = world + 0x3c;
            *(int *)(world + 0x40) = world + 0x3c;
            if (!FUN_004cc5e0(stream, 8, 0, &version2)) {
                FUN_004e5700(world);
                return 0;
            }
            if (version2 < 0x35000 || version2 > 0x37002) {
                err.a = 2;
                err.b = FUN_004d7ff0(0x80000004);
                FUN_004d8480(&err);
                FUN_004e5700(world);
                return 0;
            }
            if (!FUN_004f3e90(stream, world + 0x10)) {
                FUN_004e5700(world);
                return 0;
            }
            if (h.h00 != 0) {
                if (!FUN_004cc5e0(stream, 9, 0, &version2)) {
                    FUN_004e5700(world);
                    return 0;
                }
                if (version2 < 0x35000 || version2 > 0x37002) {
                    goto bad_version;
                }
                *(int *)(world + 0x1c) = FUN_004e9e40(stream, &next, world, h.h24);
            } else {
                if (!FUN_004cc5e0(stream, 10, 0, &version2)) {
                    FUN_004e5700(world);
                    return 0;
                }
                if (version2 < 0x35000 || version2 > 0x37002) {
                    goto bad_version;
                }
                *(int *)(world + 0x1c) = FUN_004ea220(stream, &next, world, h.h24);
            }
            if (*(int *)(world + 0x1c) == 0) {
                FUN_004e5700(world);
                return 0;
            }
            *(int *)(world + 0x28) = world + 0x2c;
            *(int *)(world + 0x24) = 0;
            memcpy((void *)(world + 0x50), h.h28, sizeof(h.h28));
            FUN_004e51e0(world);
            FUN_004e5820(world, 0);
            for (i = 0; i < *(int *)(world + 0x14); i++) {
                int *p = *(int **)(*(int *)(world + 0x10) + i * 4);
                if (*p != 0) {
                    FUN_004e8090(p, *p);
                }
            }
            FUN_004d8000(&DAT_0061864c, world);
            DAT_007d7278 = 0;
            DAT_007d7274 = 0;
            if (!FUN_004e1b60(&DAT_0061864c, stream, world)) {
                FUN_004e5700(world);
                return 0;
            }
            if (DAT_007d7278 != 0) {
                FUN_004e1c90(&DAT_0061864c, DAT_007d7278, world, DAT_007d7274);
            }
            if (!FUN_004e5300(world)) {
                FUN_004e5700(world);
                return 0;
            }
            return world;
bad_version:
            err.a = 2;
            err.b = FUN_004d7ff0(0x80000004);
            FUN_004d8480(&err);
            FUN_004e5700(world);
            return 0;
        }
    }
    err.a = 2;
    err.b = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&err);
    return 0;
}
