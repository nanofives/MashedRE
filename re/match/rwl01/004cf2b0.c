// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004cf2b0, rewritten by RACE-LOAD-004E99B0 (run 57, 2026-11-27) from the disassembly of
   0x004cf2b0..0x004cf429 (Ghidra pool slot Mashed_pool0, read-only). The earlier h3 body (47.8%) read the 13-dword chunk
   header through separate locals. Frame (S = esp after `sub esp,0x40`):
     S+0x00 chunk version, S+0x44 (the stream argument slot, the stream is in ESI first) chunk size (0x004cf2b3 / 0x004cf2b7)
     S+0x04 2-dword error record {1, FUN_004d7ff0(code)} (0x004cf3c0 / 0x004cf3cd)
     S+0x0c 0x34-byte header read by FUN_004cbd30 (zeroed first, rep stosd 0xd at 0x004cf2fb)
   Args: stream, dst (0: FUN_004c57a0() makes one, 0x004cf325). Header dwords 0..2 -> dst +0x00..+0x08, 3..5 -> +0x10..+0x18,
   6..8 -> +0x20..+0x28, 9..11 -> +0x30..+0x38, dword 12 & 3 -> +0x0c; when that is 3, FUN_004c4670(dst, dst) (0x004cf3ac).
   Errors: chunk not found or short read 0x8000001a, version outside 0x35000..0x37002 0x80000004. */
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

typedef struct { int a; int b; } Err_004cf2b0;

int FUN_004cc5e0(int stream, int type, uint *size, uint *version);
uint FUN_004cbd30(int stream, void *buf, uint size);
int FUN_004d7ff0(int code, ...);
void FUN_004d8480(Err_004cf2b0 *err);
int *FUN_004c57a0(void);
void FUN_004c4670(int *a, int *b);

// MATCH: 0x004cf2b0 FUN_004cf2b0
int *FUN_004cf2b0(int stream, int *dst)
{
    uint version;
    uint size;
    Err_004cf2b0 err;
    int h[13];
    int *r;
    int i;

    if (!FUN_004cc5e0(stream, 1, &size, &version)) {
        err.a = 1;
        err.b = FUN_004d7ff0(0x8000001a);
        FUN_004d8480(&err);
        return 0;
    }
    if (version < 0x35000 || version > 0x37002) {
        err.a = 1;
        err.b = FUN_004d7ff0(0x80000004);
        FUN_004d8480(&err);
        return 0;
    }
    for (i = 0; i < 13; i++) {
        h[i] = 0;
    }
    if (size != FUN_004cbd30(stream, h, size)) {
        err.a = 1;
        err.b = FUN_004d7ff0(0x8000001a);
        FUN_004d8480(&err);
        return 0;
    }
    r = dst;
    if (r == 0) {
        r = FUN_004c57a0();
        if (r == 0) {
            return 0;
        }
    }
    r[3] = 0;
    r[0] = h[0];
    r[1] = h[1];
    r[2] = h[2];
    r[4] = h[3];
    r[5] = h[4];
    r[6] = h[5];
    r[8] = h[6];
    r[9] = h[7];
    r[10] = h[8];
    r[12] = h[9];
    r[13] = h[10];
    r[14] = h[11];
    r[3] = h[12] & 3;
    if ((char)(h[12] & 3) == 3) {
        FUN_004c4670(r, r);
    }
    return r;
}
