// FLAGS: /O2 /GS /Ob1
/* 0x00477810 (94 B): 100% (regs 100%). RwGlobals (0x007d3ff8) +0x20 render-state set (8,0)/(8,1) around the two
   Decal loops; signed pointer compares. This file holds 0x00477810 and its static EAX-arg callee 0x00477500 (100%). */
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

typedef struct Decal {
    float m[16];
    unsigned char col[4];
    int *obj;
    float t;
    float tmax;
    float rate;
    float age;
    int active;
} Decal;

typedef int (*RenderStateSetFn)(int, int);
typedef void (*RenderFn)(int *);
MGLOBALA(Decal, DAT_006925b8, 16, 0x006925b8);
MGLOBALA(Decal, DAT_00692bb8, 16, 0x00692bb8);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_005493d0(int *p0);
int FUN_005492a0(int p0, int p1);

// MATCH: 0x00477500 FUN_00477500
static void FUN_00477500(Decal *d)
{
    float f;
    unsigned char *mat;
    unsigned int c;
    if (d->t != 0.0f) {
        f = d->t / d->tmax;
        mat = (unsigned char *)FUN_005492a0(FUN_005493d0(d->obj), 0);
        c = *(unsigned int *)d->col;
        ((unsigned char *)&c)[3] = (unsigned char)((1.0 - f) * d->col[3]);
        *(unsigned int *)(mat + 4) = c;
        (*(RenderFn *)((char *)d->obj + 0x48))(d->obj);
    }
}

// MATCH: 0x00477810 FUN_00477810
void FUN_00477810(void)
{
    Decal *d;
    (*(RenderStateSetFn *)(DAT_007d3ff8 + 0x20))(8, 0);
    for (d = DAT_00692bb8; (int)d < (int)(DAT_00692bb8 + 16); d++)
        if (d->active) FUN_00477500(d);
    for (d = DAT_006925b8; (int)d < (int)(DAT_006925b8 + 16); d++)
        if (d->active) FUN_00477500(d);
    (*(RenderStateSetFn *)(DAT_007d3ff8 + 0x20))(8, 1);
}
