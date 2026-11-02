// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h11 0x0042a9c0 (41 B): 100.0% exact (regs 100, ops 100), byte-identical.
   Raw converter 21.4%. Natural form: one || chain over DAT_007f0fd0 (10, 9, 8, 7, 5) -> 0x2d, else 0x16.
   Residual: none. */
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
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
// MATCH: 0x0042a9c0 FUN_0042a9c0
int FUN_0042a9c0(void)
{
  if (DAT_007f0fd0 == 10 || DAT_007f0fd0 == 9 || DAT_007f0fd0 == 8 || DAT_007f0fd0 == 7 || DAT_007f0fd0 == 5)
    return 0x2d;
  return 0x16;
}
