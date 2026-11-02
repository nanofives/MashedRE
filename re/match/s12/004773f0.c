// FLAGS: /O2 /GS /Ob1
/* 0x004773f0 (84 B): 100% (regs 100%). EAX-passed Decal* (pilot finding 1): static callee defined before its caller.
   This file holds 0x004773f0 and its real caller 0x00477730 (both 100%). The zero/one stores are RenderWare's
   RwMatrixSetIdentityMacro chained assignments (ones chain, three zero chains, flags |= 0x20003); the colour bytes
   must come before the macro. Decal = 0x5c-byte record, arrays of 16 at 0x00692bb8 and 0x006925b8. */
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

MGLOBALA(Decal, DAT_006925b8, 16, 0x006925b8);
MGLOBALA(Decal, DAT_00692bb8, 16, 0x00692bb8);

// MATCH: 0x004773f0 FUN_004773f0
static void FUN_004773f0(Decal *d)
{
    d->t = 0.0f;
    d->tmax = 0.0f;
    d->rate = 0.0f;
    d->age = 0.0f;
    d->active = 0;
    d->col[0] = 0xff;
    d->col[1] = 0xff;
    d->col[2] = 0xff;
    d->col[3] = 0xff;
    d->m[0] = d->m[5] = d->m[10] = 1.0f;
    d->m[1] = d->m[2] = d->m[4] = 0.0f;
    d->m[6] = d->m[8] = d->m[9] = 0.0f;
    d->m[12] = d->m[13] = d->m[14] = 0.0f;
    *(unsigned int *)&d->m[3] = *(unsigned int *)&d->m[3] | 0x20003;
}

// MATCH: 0x00477730 FUN_00477730
void FUN_00477730(void)
{
    Decal *d;
    for (d = DAT_00692bb8; (int)d < (int)(DAT_00692bb8 + 16); d++)
        FUN_004773f0(d);
    for (d = DAT_006925b8; (int)d < (int)(DAT_006925b8 + 16); d++)
        FUN_004773f0(d);
}
