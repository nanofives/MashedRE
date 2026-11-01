// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042bf30 (h6, by hand): 100% (regs 100%). Raw 76%. Fixes: dropped the duplicate undefined1 DAT_0067ead0 global; store order from the permuter (eab4 and eab0=1 first, then eac0 before eabc). Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0067eab0, 0x0067eab0);
MGLOBAL(undefined4, DAT_0067eac0, 0x0067eac0);
MGLOBAL(undefined4, DAT_0067eabc, 0x0067eabc);
MGLOBAL(undefined4, DAT_0067eac8, 0x0067eac8);
MGLOBAL(undefined4, DAT_0067eacc, 0x0067eacc);
MGLOBAL(int, DAT_0067ea5c, 0x0067ea5c);
MGLOBAL(undefined4, DAT_0067eab4, 0x0067eab4);
MGLOBAL(undefined4, DAT_0067eab8, 0x0067eab8);
MGLOBAL(undefined4, DAT_0067ead4, 0x0067ead4);
MGLOBAL(int, DAT_0067ead0, 0x0067ead0);
// MATCH: 0x0042bf30 FUN_0042bf30
/* [C2 2026-05-18] Body (0x0042bf30..0x0042bfa0): */

void FUN_0042bf30(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5,undefined4 param_6)

{
  if (DAT_0067eab0 == 0) {
    DAT_0067eab4 = param_1;
    DAT_0067eab0 = 1;
    DAT_0067eac0 = param_3;
    DAT_0067eabc = param_2;
    DAT_0067eac8 = param_4;
    DAT_0067eacc = param_5;
    DAT_0067ead0 = param_6;
    DAT_0067eab8 = 0;
    DAT_0067ead4 = 0;
    if (DAT_0067ea5c != 0) {
      DAT_0067ead4 = 1;
      DAT_0067ea5c = 0;
    }
  }
  return;
}
