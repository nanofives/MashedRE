// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045caf0 (h2): 100% (regs 100%). Raw 62.5%: two separate early `return 0` statements (not one
   `||` or `&&` test) give the jg / jge order; int return; 16-byte records at 0x007f1a18 read as int[p*4]. */
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
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBALA(int, DAT_007f1a18, 16, 0x007f1a18);
// MATCH: 0x0045caf0 FUN_0045caf0
int FUN_0045caf0(int param_1, int param_2)
{
  if (DAT_008aa254 > 1)
    return 0;
  if (param_2 < 0)
    return 0;
  return DAT_007f1a18[param_1 * 4] == DAT_007f1a18[param_2 * 4];
}
