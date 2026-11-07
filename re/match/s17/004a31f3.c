// FLAGS: /O1
/* MATCH-S17 0x004a31f3: CRT crt0dat.c _cinit (VS.NET 2002 form, no argument): _FPinit DAT_00616044 if set,
   _initterm_e(__xi_a 0x005ea03c, __xi_z 0x005ea058), atexit FUN_004a415e(_RTC_Terminate FUN_004a78f4),
   _initterm(__xc_a 0x005ea000, __xc_z 0x005ea038). The two initterm loops are written in the body.
   100%, whole symbol BYTE-IDENTICAL, relocheck OK. The four section-bound tables must be arrays of UNKNOWN bound
   (MGLOBAL over a typedef T[]): with a sized array (MGLOBALA, or MGLOBAL of a scalar) cl 13 drops the entry
   test pf < end and keeps end as an immediate (52%). MGLOBALA with an empty N does not work either: cl 13 shifts
   the empty argument, giving n[0x5ea03c]. */
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
typedef void (__cdecl *_PVFV)(void);
typedef int (__cdecl *_PIFV)(void);
MGLOBAL(_PVFV, DAT_00616044, 0x00616044);
typedef _PIFV _PIFV_TAB[];
typedef _PVFV _PVFV_TAB[];
MGLOBAL(_PIFV_TAB, DAT_005ea03c, 0x005ea03c);
MGLOBAL(_PIFV_TAB, DAT_005ea058, 0x005ea058);
MGLOBAL(_PVFV_TAB, DAT_005ea000, 0x005ea000);
MGLOBAL(_PVFV_TAB, DAT_005ea038, 0x005ea038);
int FUN_004a415e(_PVFV func);
void FUN_004a78f4(void);

// MATCH: 0x004a31f3 FUN_004a31f3
int FUN_004a31f3(void)
{
    int initret;
    _PIFV *pfi;
    _PVFV *pfv;

    if (DAT_00616044 != 0)
        (*DAT_00616044)();
    initret = 0;
    pfi = DAT_005ea03c;
    while (pfi < DAT_005ea058 && initret == 0) {
        if (*pfi != 0)
            initret = (**pfi)();
        ++pfi;
    }
    if (initret != 0)
        return initret;
    FUN_004a415e(FUN_004a78f4);
    pfv = DAT_005ea000;
    while (pfv < DAT_005ea038) {
        if (*pfv != 0)
            (**pfv)();
        ++pfv;
    }
    return 0;
}
