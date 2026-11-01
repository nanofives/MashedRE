// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h11 0x0042aad0 (15 B): 100.0% exact (regs 100, ops 100), byte-identical.
   Raw converter 50.0% (in_EAX). The original takes its pointer in EAX: that is the register convention plain
   cl 13.00.9176 gives a static function defined before its caller (pilot finding 1). caller_0042aad0 is a
   stand-in for the real callers at 0x0043b0a8.. (not matched). Residual: none. */
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
MGLOBAL(int, DAT_008990e4, 0x008990e4);
// MATCH: 0x0042aad0 FUN_0042aad0
MATCH_STATIC void FUN_0042aad0(unsigned char *col)
{
  col[3] = 0x30;
  DAT_008990e4 = 1;
}

#ifndef __cplusplus
/* stand-in caller (not matched): the real callers sit at 0x0043b0a8 and later */
void caller_0042aad0(unsigned char *a, unsigned char *b)
{
  FUN_0042aad0(a);
  FUN_0042aad0(b);
}
#endif
