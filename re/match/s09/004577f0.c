// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004577f0 (h6): 100% (2/2, 9 B byte-identical). It loads its stack argument into EAX and
   tail-jumps to 0x00456eb0, which takes that pointer in EAX (raw converter 50%: void prototype, no EAX). The file
   holds the real callee 0x00456eb0 as a `static` function defined first (pilot finding 1), also 100% and
   byte-identical (181 B): pool-H slot pick (first free bit in DAT_0068a300, else the entry with the largest
   +0x00 float), then the slot init and the 3-float + 64-byte copies from the matrix argument.
   Struct names are mechanical (PoolH = the 0x68 B entry at 0x00688810, Mat = 64 B, +0x30 copied to entry +0x08).
   Residual: none. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef unsigned char byte;
typedef struct { float x, y, z; } V3;
typedef struct { V3 right; unsigned int flags; V3 up; unsigned int pad1; V3 at; unsigned int pad2; V3 pos;
                 unsigned int pad3; } Mat;
typedef struct {
    float f0; float f4; V3 pos; float f14; Mat mat; float f58; float f5c; float f60; float f64;
} PoolH;
MGLOBALA(PoolH, DAT_00688810, 64, 0x00688810);
MGLOBALA(byte, DAT_0068a300, 8, 0x0068a300);
// MATCH: 0x00456eb0 FUN_00456eb0
static void FUN_00456eb0(Mat *m)
{
    PoolH *best = DAT_00688810;
    PoolH *slot = 0;
    PoolH *p;
    int i;
    for (i = 0; i < 64; i++) {
        p = &DAT_00688810[i];
        if (p->f0 > best->f0)
            best = p;
        if ((DAT_0068a300[i >> 3] & (1 << (i & 7))) == 0) {
            slot = &DAT_00688810[i];
            break;
        }
    }
    if (slot == 0)
        slot = best;
    slot->f60 = 0.0f;
    slot->f0 = 0.0f;
    DAT_0068a300[i >> 3] |= 1 << (i & 7);
    slot->f5c = -1.0f;
    slot->f58 = 0.0f;
    slot->f14 = 0.25f;
    slot->pos = m->pos;
    slot->mat = *m;
}
// MATCH: 0x004577f0 FUN_004577f0
void FUN_004577f0(Mat *m)
{
    FUN_00456eb0(m);
}
