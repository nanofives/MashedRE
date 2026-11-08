// FLAGS: /O1 /Oy-
/* MATCH-S17 0x004a7800: CRT mlock.c _mtinitlocknum (VS.NET 2002 libcmt: no __try in this version, the
   unlock is inline on both paths). /O1 /Oy-. _malloc_crt = 0x004a45fb, _errno = 0x004aa23d,
   __crtInitCritSecAndSpinCount = 0x004ae29f, _lock/_unlock = 0x004a787f/0x004a77eb, 10 = _LOCKTAB_LOCK. 100%. */
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
typedef struct { void *lock; int kind; } locktab_t;
MGLOBALA(locktab_t, DAT_00616408, 36, 0x00616408);
void * __cdecl FUN_004a45fb(unsigned int size);
void __cdecl FUN_004a460d(void *p);
int * __cdecl FUN_004aa23d(void);
int __cdecl FUN_004ae29f(void *pcs, unsigned long spin);
void __cdecl FUN_004a787f(int locknum);
void __cdecl FUN_004a77eb(int locknum);
// MATCH: 0x004a7800 FUN_004a7800
int __cdecl FUN_004a7800(int locknum)
{
    void *pcs;

    if (DAT_00616408[locknum].lock != 0)
        return 1;

    if ((pcs = FUN_004a45fb(0x18)) == 0) {
        *FUN_004aa23d() = 12;
        return 0;
    }

    FUN_004a787f(10);
    if (DAT_00616408[locknum].lock == 0) {
        if (!FUN_004ae29f(pcs, 4000)) {
            FUN_004a460d(pcs);
            FUN_004a77eb(10);
            *FUN_004aa23d() = 12;
            return 0;
        }
        DAT_00616408[locknum].lock = pcs;
    }
    else
        FUN_004a460d(pcs);
    FUN_004a77eb(10);
    return 1;
}
