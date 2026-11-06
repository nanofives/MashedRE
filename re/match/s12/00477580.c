// FLAGS: /O2 /GS /Ob1
/* 0x00477580 (43 B): 100% (regs 100%). ESI-passed Decal* (pilot finding 1): static callee, real caller 0x00477870 in
   this file. This file holds 0x00477580 and 0x00477870 (100% by bytes: 99 B whole symbol BYTE-IDENTICAL). */
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
MGLOBAL(int, DAT_00693178, 0x00693178);
MGLOBAL(int, DAT_0069317c, 0x0069317c);
MGLOBAL(int, DAT_00693188, 0x00693188);
MGLOBAL(int, DAT_0069318c, 0x0069318c);
int FUN_004c0c20(int p0);
int FUN_004e7e30(int *p0, int p1);
int FUN_004e6920(int *p0);
int FUN_005495b0(int p0);
int FUN_004e6e00(int p0);

// MATCH: 0x00477580 FUN_00477580
static void FUN_00477580(Decal *d)
{
    FUN_004c0c20(d->obj[1]);
    FUN_004e7e30(d->obj, 0);
    FUN_004e6920(d->obj);
    d->obj = 0;
}

// MATCH: 0x00477870 FUN_00477870
void FUN_00477870(void)
{
    Decal *d;
    for (d = DAT_00692bb8; (int)d < (int)(DAT_00692bb8 + 16); d++)
        FUN_00477580(d);
    for (d = DAT_006925b8; (int)d < (int)(DAT_006925b8 + 16); d++)
        FUN_00477580(d);
    FUN_005495b0(DAT_00693188);
    FUN_004e6e00(DAT_00693178);
    FUN_005495b0(DAT_0069318c);
    FUN_004e6e00(DAT_0069317c);
}
