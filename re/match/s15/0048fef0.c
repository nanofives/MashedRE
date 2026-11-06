// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x0048fef0 (h3): 100% (regs 100%), relocheck OK. Raw 85.7% (Ghidra prototype kept a
   needless temp pointer); written as a plain `|= 0x40` on the dword at (*(param_1+0x18))+8.
   FUN_0048fee0 is the callback passed by address (extern, not in this file). No residual. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
void *FUN_0048fee0(void *a, void *b);
int FUN_004e89a0(int p0, void *(*cb)(void *, void *), void *data);
// MATCH: 0x0048fef0 FUN_0048fef0
int FUN_0048fef0(int param_1, void *param_2)
{
    int g = *(int *)(param_1 + 0x18);
    *(unsigned int *)(g + 8) |= 0x40;
    FUN_004e89a0(g, FUN_0048fee0, param_2);
    return param_1;
}
