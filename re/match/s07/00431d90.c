// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00431d90: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs07_20261101/results.tsv. */
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
typedef unsigned int uint;
MGLOBAL(uint, DAT_0067e7d8, 0x0067e7d8);
MGLOBAL(int, DAT_0067e7b0, 0x0067e7b0);
MGLOBAL(uint, DAT_0067e7f0, 0x0067e7f0);
MGLOBAL(uint, DAT_0067e7d0, 0x0067e7d0);
MGLOBAL(uint, DAT_0067e7a8, 0x0067e7a8);
MGLOBAL(uint, DAT_0067e7c0, 0x0067e7c0);
MGLOBAL(uint, DAT_0067e7c8, 0x0067e7c8);
MGLOBAL(uint, DAT_0067e7f8, 0x0067e7f8);
MGLOBAL(uint, DAT_0067e838, 0x0067e838);
MGLOBAL(uint, DAT_0067e828, 0x0067e828);
MGLOBAL(uint, DAT_0067e830, 0x0067e830);
MGLOBAL(uint, DAT_0067e820, 0x0067e820);
MGLOBAL(uint, DAT_0067e7e0, 0x0067e7e0);
MGLOBAL(uint, DAT_0067e7e8, 0x0067e7e8);
MGLOBAL(uint, DAT_0067e808, 0x0067e808);
MGLOBAL(uint, DAT_0067e810, 0x0067e810);
MGLOBAL(uint, DAT_0067e818, 0x0067e818);
MGLOBAL(uint, DAT_0067e7b8, 0x0067e7b8);
MGLOBAL(uint, DAT_0067e800, 0x0067e800);
// MATCH: 0x00431d90 FUN_00431d90
void FUN_00431d90(void)

{
  DAT_0067e7d8 = (DAT_0067e7d8 != 1) - 1 & 2;
  if (DAT_0067e7b0 != 1) {
    DAT_0067e7b0 = 0;
  }
  DAT_0067e7f0 = (DAT_0067e7f0 != 1) - 1 & 2;
  DAT_0067e7d0 = (DAT_0067e7d0 != 1) - 1 & 2;
  DAT_0067e7a8 = (DAT_0067e7a8 != 1) - 1 & 2;
  DAT_0067e7c0 = (DAT_0067e7c0 != 1) - 1 & 2;
  DAT_0067e7c8 = (DAT_0067e7c8 != 1) - 1 & 2;
  DAT_0067e7f8 = (DAT_0067e7f8 != 1) - 1 & 2;
  DAT_0067e838 = (DAT_0067e838 != 1) - 1 & 2;
  DAT_0067e828 = (DAT_0067e828 != 1) - 1 & 2;
  DAT_0067e830 = (DAT_0067e830 != 1) - 1 & 2;
  DAT_0067e820 = (DAT_0067e820 != 1) - 1 & 2;
  DAT_0067e7e0 = (DAT_0067e7e0 != 1) - 1 & 2;
  DAT_0067e7e8 = (DAT_0067e7e8 != 1) - 1 & 2;
  DAT_0067e808 = (DAT_0067e808 != 1) - 1 & 2;
  DAT_0067e810 = (DAT_0067e810 != 1) - 1 & 2;
  DAT_0067e818 = (DAT_0067e818 != 1) - 1 & 2;
  DAT_0067e7b8 = (DAT_0067e7b8 != 1) - 1 & 2;
  DAT_0067e800 = (DAT_0067e800 != 1) - 1 & 2;
  return;
}
