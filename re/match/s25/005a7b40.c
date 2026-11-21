// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a7b40: 100.0% BYTE-IDENTICAL, relocheck OK (PORT-TOP-AUDIO-5BA1D0-STOPS, 2026-11-21). */
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
/* PORT-TOP-AUDIO-5BA1D0-STOPS (2026-11-21): new source from the listing 0x005a7b40..0x005a7b4f (RO-AUDIOINIT-45D460-REACH: a callee
   of 0x005a7b60, lines 109 / 122). Stores param_1 to 0x007dcabc and returns the previous value. */
MGLOBAL(undefined4, DAT_007dcabc, 0x007dcabc);
// MATCH: 0x005a7b40 FUN_005a7b40
undefined4 FUN_005a7b40(undefined4 param_1)
{
  undefined4 old;

  old = DAT_007dcabc;
  DAT_007dcabc = param_1;
  return old;
}
