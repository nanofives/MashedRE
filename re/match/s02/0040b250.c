// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 lead 2026-10-31: store order fixed after re/tools/match/relocheck.py showed the stores went to swapped arrays at 100% masked score; now 100% and relocheck OK. */
/* 0x0040b250 FUN_0040b250 (52 B): DAT_008a9500[4] and DAT_008a9520[4] all set to -1000. Best: 100% exact
   (regs 100, ops 100). Raw Ghidra 54.5% (eight scalar stores); written as one i<4 loop storing both arrays,
   which the compiler unrolls into the original store order a0..a2, b0..b2, a3, b3. Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBALA(int, DAT_008a9500, 4, 0x008a9500);
MGLOBALA(int, DAT_008a9520, 4, 0x008a9520);

// MATCH: 0x0040b250 FUN_0040b250
void FUN_0040b250(void)
{
  int i;
  for (i = 0; i < 4; i++) {
    DAT_008a9520[i] = -1000;
    DAT_008a9500[i] = -1000;
  }
}
