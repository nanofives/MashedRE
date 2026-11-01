// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h11 0x0042ac50 (49 B): 100.0% exact (regs 100, ops 100), byte-identical.
   Raw converter 25.0% (in_EAX). Static function (n in EAX, w in ECX, pilot finding 1); caller_0042ac50 is a
   stand-in for the real callers at 0x0042fbb6.. (not matched). Tried: two returns (12-21%), r = 0xf0 with a
   conditional subtract (66.7%, tail merged). What matched: if (!(n & 1)) r = 0xf0 - w / 2; else r = 0xf0;
   then one shared return r - (n - 1) / 2 * w (the compiler duplicates the tail). Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
// MATCH: 0x0042ac50 FUN_0042ac50
static int FUN_0042ac50(unsigned int n, int w)
{
  int r; if (!(n & 1)) r = 0xf0 - w / 2; else r = 0xf0; return r - (n - 1) / 2 * w;
}

/* stand-in caller (not matched): the real callers sit at 0x0042fbb6 and later */
int caller_0042ac50(unsigned int n, int w)
{
  return FUN_0042ac50(n, w) + FUN_0042ac50(n + 1, w);
}
