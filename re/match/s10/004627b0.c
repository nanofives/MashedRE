// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004627b0 (h6): 100.0% (regs 100%), whole symbol byte-identical (51 B incl. both tables).
   Raw 45.5% (if-chain). The original's 1..7 byte-indexed table with 2 targets needs cases 1,5,6 as their own
   `return 0` group plus a separate default `return 0` (merged after the tables are built); a shared
   `case 1: default:` label turns the switch into compares. Residual: none. */
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
MGLOBAL(int, DAT_00773920, 0x00773920);
// MATCH: 0x004627b0 FUN_004627b0
int FUN_004627b0(void)
{
  switch (DAT_00773920) {
  case 1:
  case 5:
  case 6:
    return 0;
  case 2:
  case 3:
  case 4:
  case 7:
    return 1;
  default:
    return 0;
  }
}
