// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 (h1) 0x0045c510: 100%, 22/22 ins. for-loop over the 4 pointers at 0x0088f680, one && chain.
   Residual: none. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct Obj0045c510 {
    int pad00;
    int f04;
    int pad08;
    int state;
    int pad10;
    int f14;
    int pad18[4];
    int f28;
} Obj0045c510;
typedef struct Arg0045c510 {
    int pad00;
    int slot;
} Arg0045c510;
MGLOBALA(Obj0045c510 *, DAT_0088f680, 4, 0x0088f680);

// MATCH: 0x0045c510 FUN_0045c510
int FUN_0045c510(int id, Arg0045c510 *arg)
{
    int i;
    Obj0045c510 *o;
    for (i = 0; i < 4; i++) {
        o = DAT_0088f680[i];
        if (o != 0 && o->state == 1 && o->f28 == id && arg->slot == i)
            return 1;
    }
    return 0;
}
