// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 (h1) 0x0045c480: 55.0% (regs 70.0%), 19/20 ins. Raw converter 30.0%. Best = permuter order.
   Residual (b): the original zeroes 0x0088f0a0..0x0088f0ac from a second zero register (ecx) and keeps every
   scalar store before the rep stosd of 0x0088f5e0; ours uses one zero register and sinks the stores after
   the rep stosd. Tried: int/float/void * /unsigned/double/volatile splits of the two groups, file-static
   globals, memset intrinsic vs a zeroing loop (int and float), permuter 224 variants. */
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
MGLOBAL(int, DAT_0088f0a0, 0x0088f0a0);
MGLOBAL(float, DAT_0088f6b0, 0x0088f6b0);
MGLOBAL(float, DAT_0088e67c, 0x0088e67c);
MGLOBAL(float, DAT_0088f0b0, 0x0088f0b0);
MGLOBAL(float, DAT_0088eb80, 0x0088eb80);
MGLOBAL(int, DAT_0088f0a4, 0x0088f0a4);
MGLOBAL(float, DAT_008aa2e0, 0x008aa2e0);
MGLOBAL(int, DAT_0088f0a8, 0x0088f0a8);
MGLOBAL(float, DAT_008aa2e4, 0x008aa2e4);
MGLOBAL(int, DAT_0088f0ac, 0x0088f0ac);
MGLOBAL(float, DAT_008aa2e8, 0x008aa2e8);
MGLOBALA(int, DAT_0088f5e0, 0x24, 0x0088f5e0);
MGLOBAL(float, DAT_008aa2ec, 0x008aa2ec);
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)

// MATCH: 0x0045c480 FUN_0045c480
void FUN_0045c480(void)
{
    DAT_0088f6b0 = 0.0f;
    DAT_0088f0a0 = 0;
    DAT_0088e67c = 0.0f;
    DAT_0088f0b0 = 0.0f;
    DAT_0088eb80 = 0.0f;
    DAT_0088f0a4 = 0;
    DAT_008aa2e0 = 0.0f;
    DAT_0088f0a8 = 0;
    DAT_008aa2e4 = 0.0f;
    DAT_0088f0ac = 0;
    DAT_008aa2e8 = 0.0f;
    memset(DAT_0088f5e0, 0, sizeof(DAT_0088f5e0));
    DAT_008aa2ec = 0.0f;
}
