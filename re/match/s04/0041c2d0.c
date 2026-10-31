// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041c2d0 (35 B), same file text as 0041c300.c: 0x0041c2d0 (35 B) + its caller 0x0041c300 (20 B): both 100.0% (regs 100, ops 100), by hand. This file holds 0x0041c2d0.
   Raw: 66.7% / 84.2%. 0x0041c300 passes &DAT_0063cda4 in EAX and tail-jumps into 0x0041c2d0, which reads the
   pointer from EAX (Ghidra in_EAX). Pilot finding 1: 0x0041c2d0 is `static`, defined before its caller, taking a
   pointer to 4 object pointers; each object has a function pointer at +0x48 called with the object (order 3,1,0,2).
   Residual: none. Relocheck OK. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct MatchObj0041c2d0 MatchObj0041c2d0;
typedef void (*MatchFn0041c2d0)(MatchObj0041c2d0 *);
struct MatchObj0041c2d0 { char pad[0x48]; MatchFn0041c2d0 fn; };
MGLOBAL(int, DAT_0063cdbc, 0x0063cdbc);
MGLOBALA(MatchObj0041c2d0 *, DAT_0063cda4, 4, 0x0063cda4);
// MATCH: 0x0041c2d0 FUN_0041c2d0
static void FUN_0041c2d0(MatchObj0041c2d0 **o)
{
  o[3]->fn(o[3]);
  o[1]->fn(o[1]);
  o[0]->fn(o[0]);
  o[2]->fn(o[2]);
}
// MATCH: 0x0041c300 FUN_0041c300
void FUN_0041c300(void)
{
  if (DAT_0063cdbc != 0) {
    FUN_0041c2d0(DAT_0063cda4);
  }
}
