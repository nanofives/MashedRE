// FLAGS: /O1
/* MATCH-S28 h1 0x005c419e : Not byte-matched (score is whatever match.py reports for a call-through). The original is the Intel libm SSE2 exp in assembly (unpcklpd/movapd on xmm0, argument arrives in xmm0, rounding-constant polynomial over the 0x005e72f0..0x005e77c8 tables, error path through 0x004a73e8). It is not expressible in C and its argument is in a register, so the file only gives the port an exp() call-through. Residual class (c), hand-written SSE2 assembly. */

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

double exp(double x);
// MATCH: 0x005c419e FUN_005c419e
double FUN_005c419e(double x)
{
  return exp(x);
}
