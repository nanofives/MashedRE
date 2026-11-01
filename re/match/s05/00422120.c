// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00422120: by hand, 100% (regs 100%) for both functions, no residual.
   This file holds the static callee 0x00421c50 (record in EAX), defined before its
   caller. 0x00421c10 is the separate raw source re/match/s05/00421c10.c (stack argument). */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct Grp00422120 { char b[0x100]; } Grp00422120;
typedef struct Rec00422120 { Grp00422120 g[2]; int model; int idx; } Rec00422120;
MGLOBALA(Rec00422120, DAT_0063fb90, 4, 0x0063fb90);
void FUN_00421c10(Grp00422120 *p0);

// MATCH: 0x00421c50 _FUN_00421c50
static void FUN_00421c50(Rec00422120 *rec)
{
    int j;
    for (j = 0; j < 2; j++)
        FUN_00421c10(&rec->g[j]);
}

// MATCH: 0x00422120 _FUN_00422120
void FUN_00422120(void)
{
    int i;
    for (i = 0; i < 4; i++)
        FUN_00421c50(&DAT_0063fb90[i]);
}
