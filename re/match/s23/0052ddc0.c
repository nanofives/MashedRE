// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052ddc0: 100% (regs 100%). Counts entries of a ring list (no library source): link offset from
   (*obj)+8, head obj[4]; p advances by the offset until *p == head. What moved it: comparing against obj[4] in
   the loop condition (a `head` local gave 52.6%). Residual: none. */
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
// MATCH: 0x0052ddc0 FUN_0052ddc0
int FUN_0052ddc0(int *obj)
{
    int n = 0;
    int off = *(int *)(obj[0] + 8);
    char *p = (char *)obj[4];

    while (*(char **)p != (char *)obj[4]) {
        p += off;
        n++;
    }
    return n;
}
