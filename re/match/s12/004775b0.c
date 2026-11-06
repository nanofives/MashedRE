// FLAGS: /O2 /GS /Ob1
/* 0x004775b0 (365 B Ghidra size): matched by bytes: match.py --bytes: 372 B whole symbol BYTE-IDENTICAL (the Ghidra size
   stops before the last cmp/jl/pop/ret). Loads shockwave.dff / hemisphere.dff (FUN_0042a5d0 3 args, FUN_004b3f90 1 arg),
   RwMatrixSetIdentityMacro on the matrix at 0x00692b78, then fills both Decal arrays. What moved it from 86.9%:
   indexed loops (arr[i].obj) instead of pointer loops, which puts the induction pointer on +0x44 like the original.
   This file holds 0x004775b0 and its static EAX-arg callee 0x004773f0 (100%). */
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
MGLOBALA(float, DAT_00692b78, 16, 0x00692b78);
MGLOBALA(Decal, DAT_00692bb8, 16, 0x00692bb8);
MGLOBALA(char, s_shockwave_dff_005ceb00, 14, 0x005ceb00);
MGLOBALA(char, s_hemisphere_dff_005ceaf0, 15, 0x005ceaf0);
MGLOBAL(int, DAT_00693178, 0x00693178);
MGLOBAL(int, DAT_0069317c, 0x0069317c);
MGLOBAL(int, DAT_00693180, 0x00693180);
MGLOBAL(int, DAT_00693184, 0x00693184);
MGLOBAL(int, DAT_00693188, 0x00693188);
MGLOBAL(int, DAT_0069318c, 0x0069318c);
int FUN_0042a5d0(char *p0, int p1, int p2);
int FUN_004b3f90(int p0);
int FUN_005493d0(int p0);
void FUN_004b4150(int p0, int p1);
int *FUN_004e69a0(int p0);
int FUN_004c0b30(void);
int FUN_004e7e30(int *p0, int p1);

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

// MATCH: 0x004775b0 FUN_004775b0
void FUN_004775b0(void)
{
    int i;
    DAT_00693178 = FUN_0042a5d0(s_shockwave_dff_005ceb00, 0, 0);
    DAT_00693180 = FUN_004b3f90(DAT_00693178);
    DAT_00693188 = FUN_005493d0(DAT_00693180);
    FUN_004b4150(DAT_00693180, 0);
    DAT_0069317c = FUN_0042a5d0(s_hemisphere_dff_005ceaf0, 0, 0);
    DAT_00693184 = FUN_004b3f90(DAT_0069317c);
    DAT_0069318c = FUN_005493d0(DAT_00693184);
    FUN_004b4150(DAT_00693184, 0);
    DAT_00692b78[0] = DAT_00692b78[5] = DAT_00692b78[10] = 1.0f;
    DAT_00692b78[1] = DAT_00692b78[2] = DAT_00692b78[4] = 0.0f;
    DAT_00692b78[6] = DAT_00692b78[8] = DAT_00692b78[9] = 0.0f;
    DAT_00692b78[12] = DAT_00692b78[13] = DAT_00692b78[14] = 0.0f;
    *(unsigned int *)&DAT_00692b78[3] = *(unsigned int *)&DAT_00692b78[3] | 0x20003;
    for (i = 0; i < 16; i++) {
        DAT_00692bb8[i].obj = FUN_004e69a0(DAT_00693180);
        FUN_004e7e30(DAT_00692bb8[i].obj, FUN_004c0b30());
        FUN_004773f0(&DAT_00692bb8[i]);
    }
    for (i = 0; i < 16; i++) {
        DAT_006925b8[i].obj = FUN_004e69a0(DAT_00693184);
        FUN_004e7e30(DAT_006925b8[i].obj, FUN_004c0b30());
        FUN_004773f0(&DAT_006925b8[i]);
    }
}
