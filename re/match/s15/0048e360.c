// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h2 0x0048e360 (619 B): 84.0% (regs 84.0%, ops 91.2%), by hand from h1's emitter template (0x0048e180: Particle / PType structs, 8-way fall-through bitmap scan). This file also holds the static callee 0x0048d0f0 (EAX = type 14, ECX = particle; 100% here, copied from h1's form). Type 14 differs from h1's template: the bit test and clear use i (not i - 1), written as __inline BitTest/BitClear (69% -> 79%); colour from rotv in two branches, then the float constants 10/77/49/15/52/60/30/255/2 as literals (the original reads 0x005cc55c etc.; literals give the original fld [p]/fmul [k] operand order, 79 -> 80%); permuter (800 compiles) moved size.x = 0.1f and size.y's scale (84.0%). Residual (b): the original keeps the size stores in both branches and reloads size / recomputes (t + 2) * 2 for y, and does not CSE rotv * 10 in the else branch; ours const-propagates and CSEs. A __forceinline find-next-bit helper reproduces those reloads exactly (same as h1 found), but the compiler then rotates the scan loop (71-78%), with i by value or by pointer, for(;;) / while / goto forms. pos += vel operand order (fld vel; fadd pos) did not move with any spelling. relocheck: 10.0f etc. are compiler constants (unchecked). */
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

MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);

static __inline int BitTest(uchar *bm, int i)
{
    return bm[i >> 3] & (1 << (i & 7));
}
static __inline void BitClear(uchar *bm, int i)
{
    bm[i >> 3] &= ~(1 << (i & 7));
}

// MATCH: 0x0048e360 FUN_0048e360
void FUN_0048e360(Particle *base)
{
    int i = 0;
    for (;;) {
        V2 size;
        RGBA col;
        Particle *p;
        uchar b;
        int k, w;
        float c;
        size.x = 0.05f;
        size.y = 0.05f;
        for (;;) {
            if (i >= 256) return;
            w = i >> 5;
            if (DAT_00722250[14 * 8 + w] == 0) { i = (w + 1) * 32; continue; }
            k = i >> 3;
            b = ((uchar *)&DAT_00722250[14 * 8])[k];
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
        if (BitTest((uchar *)&DAT_00722250[14 * 8], i) && FUN_0048d0f0(14, p)) {
            if (p->rotv > DAT_005d757c) {
                size.x = 0.13f;
                size.y = 0.1f;
                c = (p->rotv * 2.0f - DAT_005cc320) * 10.0f;
                col.r = (uchar)(77.0f + c);
                col.g = (uchar)(49.0f + c);
                col.b = (uchar)(c + 15.0f);
            } else {
                size.y = 0.1f;
                col.r = (uchar)(p->rotv * 10.0f + 52.0f);
                col.g = (uchar)(p->rotv * 10.0f + 60.0f);
                col.b = (uchar)(p->rotv * 10.0f + 30.0f);
                size.x = 0.1f;
            }
            col.a = (uchar)((DAT_005cc320 - p->t) * 255.0f);
            p->pos.x = p->vel.x + p->pos.x;
            p->pos.y = p->vel.y + p->pos.y;
            p->pos.z = p->vel.z + p->pos.z;
            size.x = (p->t + 2.0f) * 2.0f * size.x;
            p->vel.x = p->damp * p->vel.x;
            size.y = (p->t + 2.0f) * 2.0f * size.y;
            p->vel.y = p->damp * p->vel.y;
            p->vel.z = p->damp * p->vel.z;
            p->rot = DAT_00722140[14].rotk * p->rotv + p->rot;
            FUN_004769d0(&size);
            FUN_004769a0(p);
            FUN_004769f0(&col);
            FUN_00476a30(p->rot);
            FUN_00476d00(&DAT_00722470[14]);
        } else {
            BitClear((uchar *)&DAT_00722250[14 * 8], i);
        }
    }
}
