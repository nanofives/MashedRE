// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h1 0x0048d0f0 (80 B): 100.0% (regs 100%), by hand.
   Register-passed args (pilot finding 1): EAX = particle type, ECX = particle; the function is `static`
   and needs a caller in the file, so this file carries a small stand-in caller (not original code; the
   10 real callers are the emitter updates 0x0048d140..0x0048e3f0, each of which also holds this callee).
   What moved it from 76%: the age update through a `float *` (`*age = dt + *age`) and the compare read
   back through `p->age` (`>=`): the original stores, then reloads age twice instead of keeping it in st0. */
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

/* stand-in caller (not original code): makes the compiler emit the static callee with the
   EAX = type, ECX = particle convention its real callers use */
int MatchStandIn_0048d0f0(Particle *p)
{
    return FUN_0048d0f0(9, p);
}
