// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00422b50 (75 B): 100% exact (regs 100, ops 100), from pilot re/match/pilot/a5_00422b50.c.
   Raw g2s shape (in-place param_2 -= ...) 25.8%: the compiler tail-duplicates the store and keeps n on the
   stack. A separate local r initialised from n (and recomputed from n in the branch) keeps n in ESI across
   the call. 0x43960000 = 300.0f is the second arg of FUN_00472650 (cdecl, float return, _ftol 0x004a2c48).
   Stride 0x138 = Rec[] at 0x008995bc; the array count 8 is a declaration size only [UNCERTAIN real count].
   Residual: none. */
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
typedef struct { int v; int pad[0x4d]; } Rec_b50;   /* stride 0x138 at 0x008995bc */
MGLOBALA(Rec_b50, DAT_008995bc, 8, 0x008995bc);
int FUN_0040e470(int i);
float FUN_00472650(int a, float b);
// MATCH: 0x00422b50 FUN_00422b50
void FUN_00422b50(int i, int n)
{
    int r = n;
    if (FUN_0040e470(i) != 1) {
        r = n - (int)FUN_00472650(0, 300.0f);
        if (r < 0)
            r = 0;
    }
    DAT_008995bc[i].v += r;
}
