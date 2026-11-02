// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h1 0x0048dd40 (505 B): 89.9% (regs 89.9%), by hand; one template for the four 505 B emitter updates
   0x0048d900 / 0x0048db20 / 0x0048dd40 / 0x0048df60 (type 13 / 15 / 16 / 1; function-scope colour
   {c8,c8,ff} / {dc,dc,dc} / {b4,b4,b4} / {96,96,96} alpha 255; size 0.04,0.04 except 0.05,0.03 for
   0x0048df60; size scale (t < 0.1 ? k1 : k2) 6/12, 6/12, 6/9, 8/13; vel.y gravity 0.0004 / 0.0003 for
   0x0048dd40; alpha clamped to >= 5). This file also holds the static callee 0x0048d0f0 (100% here).
   Position update is a __forceinline V3Add(o, a, b); size.y before size.x in each branch (permuter, +1.9).
   Residual (b): ours hoists the (t + 1) * k product for the second size component (the original reloads
   t after the first store), and the first pos add operand order. Tried: plain/compound assignments,
   macro V3Add, __forceinline size helper through pointers, permuter 488 compiles. */
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
MGLOBAL(int, DAT_007668f4, 0x007668f4);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(PType, DAT_00722140, 16, 0x00722140);
MGLOBALA(uint, DAT_00722250, 128, 0x00722250);
MGLOBALA(PBatch, DAT_00722470, 16, 0x00722470);
void FUN_004769a0(void *p);
void FUN_004769d0(void *p);
void FUN_004769f0(void *p);
void FUN_00476a30(float f);
void FUN_00476d00(void *p);

// MATCH: 0x0048d0f0 FUN_0048d0f0
static int FUN_0048d0f0(int type, Particle *p)
{
    float dt;
    float *age = &p->age;
    if (DAT_007668f4 != 0)
        p->fast = 1;
    dt = DAT_007f100c;
    if (p->fast != 0)
        dt = dt * 2.5f;
    *age = dt + *age;
    if (p->age >= DAT_00722140[type].life)
        return 0;
    p->t = p->age / DAT_00722140[type].life;
    return 1;
}

static __forceinline void V3Add(V3 *o, V3 *a, V3 *b)
{
    o->x = a->x + b->x;
    o->y = a->y + b->y;
    o->z = a->z + b->z;
}

// MATCH: 0x0048dd40 FUN_0048dd40
void FUN_0048dd40(Particle *base)
{
    int i = 0;
    RGBA col = { 0xb4, 0xb4, 0xb4, 255 };
    for (;;) {
        V2 size;
        Particle *p;
        uchar b;
        int k, w;
        size.x = 0.04f;
        size.y = 0.04f;
        for (;;) {
            if (i >= 256) return;
            w = i >> 5;
            if (DAT_00722250[16 * 8 + w] == 0) { i = (w + 1) * 32; continue; }
            k = i >> 3;
            b = ((uchar *)&DAT_00722250[16 * 8])[k];
            if (b != 0) {
                switch (i & 7) {
                case 0: i++; if (b & 1) goto found;
                case 1: i++; if (b & 2) goto found;
                case 2: i++; if (b & 4) goto found;
                case 3: i++; if (b & 8) goto found;
                case 4: i++; if (b & 0x10) goto found;
                case 5: i++; if (b & 0x20) goto found;
                case 6: i++; if (b & 0x40) goto found;
                case 7: i++; if (b & 0x80) goto found;
                }
            }
            i = k * 8 + 8;
        }
found:
        if (i < 0) return;
        p = &base[i - 1];
        if (!FUN_0048d0f0(16, p)) {
            ((uchar *)&DAT_00722250[16 * 8])[(i - 1) >> 3] &= ~(1 << ((i - 1) & 7));
            continue;
        }
        col.a = (uchar)((1.0f - p->t) * (int)DAT_00722140[16].alpha);
        if (col.a < 5)
            col.a = 5;
        V3Add(&p->pos, &p->vel, &p->pos);
        if (p->t < 0.1f) {
            size.y = (p->t + 1.0f) * 6.0f * size.y;
            size.x = (p->t + 1.0f) * 6.0f * size.x;
        } else {
            size.y = (p->t + 1.0f) * 9.0f * size.y;
            size.x = (p->t + 1.0f) * 9.0f * size.x;
        }
        p->vel.x = p->vel.x * p->damp;
        p->vel.y = p->vel.y * p->damp - 0.0003f;
        p->vel.z = p->vel.z * p->damp;
        FUN_004769d0(&size);
        FUN_004769a0(p);
        FUN_004769f0(&col);
        p->rot = DAT_00722140[16].rotk * p->rotv + p->rot;
        FUN_00476a30(p->rot);
        FUN_00476d00(&DAT_00722470[16]);
    }
}
