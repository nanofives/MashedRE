// FLAGS: /O2 /GS /Ob1
/* 0x00477450 (56 B): 100% (regs 100%). EAX-passed Decal* (pilot finding 1): static callee defined before its caller,
   64-byte matrix copy via the memcpy intrinsic (rep movsd). This file holds 0x00477450 and its real caller
   0x00477760 (both 100%). */
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

void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)

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

MGLOBALA(Decal, DAT_006925b8, 16, 0x006925b8);
MGLOBALA(float, DAT_00692b78, 16, 0x00692b78);
MGLOBALA(Decal, DAT_00692bb8, 16, 0x00692bb8);
MGLOBALA(unsigned char, DAT_006132cc, 4, 0x006132cc);

// MATCH: 0x00477450 FUN_00477450
static void FUN_00477450(Decal *d, float *mat, unsigned char *col, float tmax, float rate)
{
    d->age = 0.0f;
    d->t = 0.0f;
    d->tmax = tmax;
    d->active = 1;
    d->rate = rate;
    memcpy(d->m, mat, 64);
    *(int *)d->col = *(int *)col;
}

// MATCH: 0x00477760 FUN_00477760
void FUN_00477760(int which, float *mat, unsigned char *col, float tmax, float rate)
{
    Decal *d;
    if (mat == 0) mat = DAT_00692b78;
    if (col == 0) col = DAT_006132cc;
    if (which == 0) {
        for (d = DAT_00692bb8; (int)d < (int)(DAT_00692bb8 + 16); d++)
            if (d->active == 0) goto found;
        return;
    } else if (which == 1) {
        for (d = DAT_006925b8; (int)d < (int)(DAT_006925b8 + 16); d++)
            if (d->active == 0) goto found;
        return;
    }
    return;
found:
    FUN_00477450(d, mat, col, tmax, rate);
}
