// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a66c0: RO-AUDIOINIT-45D460-REACH-5, see below. */
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
/* RO-AUDIOINIT-45D460-REACH-5 (2026-11-27): new source from the listing 0x005a66c0..0x005a66ce (15 B, pc_funcs.csv;
   no hooks.csv row or re/match source before this row): stores 0 at *param_2 (0x005a66c4), returns param_1 (0x005a66ca).
   0x005a6340 takes its address (re/match/s25/005a6340.c). */

// MATCH: 0x005a66c0 FUN_005a66c0
int FUN_005a66c0(int param_1, undefined4 *param_2)
{
  *param_2 = 0;
  return param_1;
}
