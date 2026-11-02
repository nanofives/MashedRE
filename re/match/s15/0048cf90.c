// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h1 0x0048cf90 (337 B): 100.0% (regs 100%), by hand from the disassembly.
   Particle spawn: claims slot DAT_007668b0[type] of the per-type 256-slot pool. What moved it from 76%:
   reading the bitmap byte into a local before the test and writing `b | mask` back (not `*bp |= mask`),
   and taking the pool pointer after that read. Args: (vel, pos, type, scale), cdecl. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct { uchar r, g, b, a; } RGBA;
typedef struct { float x, y; } V2;
typedef struct {
    V3 pos;        /* +0x00 */
    V3 vel;        /* +0x0c */
    int pad18[2];  /* +0x18 */
    RGBA col;      /* +0x20 */
    float rot;     /* +0x24 */
    float rotv;    /* +0x28 */
    float age;     /* +0x2c */
    float t;       /* +0x30 */
    float damp;    /* +0x34 */
    int fast;      /* +0x38 */
    int pad3c;     /* +0x3c */
} Particle;        /* 0x40 B, pool at 0x007228b0 (256 per type) */
typedef struct {
    uchar alpha;   /* +0x0 */
    uchar pad1[3];
    float life;    /* +0x4 */
    float rotk;    /* +0x8 */
    uchar r, g, b, pad;  /* +0xc */
} PType;           /* 0x10 B table at 0x00722140 */
typedef struct { int d[16]; } PBatch;  /* 0x40 B, array at 0x00722470 (0x0048cf70 walks it) */
MGLOBALA(PType, DAT_00722140, 16, 0x00722140);
MGLOBALA(uchar, DAT_00722250, 512, 0x00722250);
MGLOBALA(int, DAT_007668b0, 16, 0x007668b0);
MGLOBALA(Particle, DAT_007228b0, 4096, 0x007228b0);
float FUN_00472650(float lo, float hi);

// MATCH: 0x0048cf90 FUN_0048cf90
void FUN_0048cf90(V3 *vel, V3 *pos, int type, float scale)
{
    int n = DAT_007668b0[type];
    uchar *bp = &DAT_00722250[type * 32 + (n >> 3)];
    Particle *p;
    uchar b = *bp;
    p = &DAT_007228b0[(type << 8) + n];
    if (b & (1 << (n & 7))) {
        p->fast = 1;
        return;
    }
    *bp = (uchar)(b | (1 << (n & 7)));
    p->pos = *pos;
    p->fast = 0;
    p->rotv = FUN_00472650(-1.0f, 1.0f);
    p->age = 0;
    p->vel.x = vel->x * 0.1f;
    p->vel.y = vel->y;
    p->vel.z = vel->z * 0.1f;
    if (scale > 1.0f)
        scale = 1.0f;
    else if (scale < 0.4f)
        scale = 0.4f;
    p->col.r = (uchar)(DAT_00722140[type].r * scale);
    p->col.b = (uchar)(DAT_00722140[type].b * scale);
    p->col.g = (uchar)(DAT_00722140[type].g * scale);
    DAT_007668b0[type]++;
    if (DAT_007668b0[type] >= 256)
        DAT_007668b0[type] = 0;
}
