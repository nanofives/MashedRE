// FLAGS: /O2 /GS /Ob1
/* 0x00476df0 (680 B Ghidra size): matched by bytes: match.py --bytes: 687 B whole symbol BYTE-IDENTICAL (the Ghidra
   size stops before the count clear / pops / ret). Lock-and-copy of up to seven per-particle arrays through
   FUN_00535700(obj, &lock{ptr,stride}, flag, 0x40000000) and FUN_00535910(obj): written as typed struct copies
   (12/8/4/64/4/16/32-byte elements) with ptr += stride. First hand pass, no permuter needed. */
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

typedef struct V3 { float x, y, z; } V3;
typedef struct V2 { float u, v; } V2;
typedef struct V4 { float a, b, c, d; } V4;
typedef struct M16 { float m[16]; } M16;
typedef struct B32 { int w[8]; } B32;
typedef struct Lock { char *ptr; int stride; } Lock;
typedef struct PrtBatch {
    int f0;
    int obj;
    int f8;
    int count;
    V3 *pos;
    V2 *uv;
    unsigned int *col;
    M16 *mat;
    unsigned int *x20;
    V4 *x24;
    B32 *x28;
} PrtBatch;
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
int FUN_00535700(int p0, Lock *p1, unsigned int p2, unsigned int p3);
void FUN_00535910(int p0);

// MATCH: 0x00476df0 FUN_00476df0
void FUN_00476df0(PrtBatch *p)
{
    Lock lk;
    int i;
    *(unsigned int *)(*(int *)(p->obj + DAT_007dc57c) + 0x40) |= 0x800000;
    *(int *)(*(int *)(p->obj + DAT_007dc57c) + 4) = p->count;
    if (p->count == 0)
        return;
    if (p->pos) {
        FUN_00535700(p->obj, &lk, 1, 0x40000000);
        for (i = 0; i < p->count; i++) {
            *(V3 *)lk.ptr = p->pos[i];
            lk.ptr += lk.stride;
        }
    }
    if (p->uv) {
        FUN_00535700(p->obj, &lk, 4, 0x40000000);
        for (i = 0; i < p->count; i++) {
            *(V2 *)lk.ptr = p->uv[i];
            lk.ptr += lk.stride;
        }
    }
    if (p->col) {
        FUN_00535700(p->obj, &lk, 2, 0x40000000);
        for (i = 0; i < p->count; i++) {
            *(unsigned int *)lk.ptr = p->col[i];
            lk.ptr += lk.stride;
        }
    }
    if (p->mat) {
        FUN_00535700(p->obj, &lk, 8, 0x40000000);
        for (i = 0; i < p->count; i++) {
            *(M16 *)lk.ptr = p->mat[i];
            lk.ptr += lk.stride;
        }
    }
    if (p->x20) {
        FUN_00535700(p->obj, &lk, 0x20, 0x40000000);
        for (i = 0; i < p->count; i++) {
            *(unsigned int *)lk.ptr = p->x20[i];
            lk.ptr += lk.stride;
        }
    }
    if (p->x24) {
        FUN_00535700(p->obj, &lk, 0x80, 0x40000000);
        for (i = 0; i < p->count; i++) {
            *(V4 *)lk.ptr = p->x24[i];
            lk.ptr += lk.stride;
        }
    }
    if (p->x28) {
        FUN_00535700(p->obj, &lk, 0x100, 0x40000000);
        for (i = 0; i < p->count; i++) {
            *(B32 *)lk.ptr = p->x28[i];
            lk.ptr += lk.stride;
        }
    }
    FUN_00535910(p->obj);
    p->count = 0;
}
