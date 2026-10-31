// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041e080: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs04_20261031/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(undefined1, DAT_0063d798, 0x0063d798);
MGLOBAL(undefined1, DAT_0063d79c, 0x0063d79c);
MGLOBAL(undefined1, DAT_0063d7a0, 0x0063d7a0);
MGLOBAL(undefined1, DAT_0063d7c4, 0x0063d7c4);
MGLOBAL(undefined1, DAT_0063d7d0, 0x0063d7d0);
MGLOBAL(undefined1, DAT_0063d7d4, 0x0063d7d4);
MGLOBAL(undefined4, DAT_0063d7d8, 0x0063d7d8);
MGLOBAL(undefined1, DAT_0063d7dc, 0x0063d7dc);
MGLOBAL(undefined1, DAT_0063d7b0, 0x0063d7b0);
MGLOBAL(undefined1, DAT_0063d7b4, 0x0063d7b4);
MGLOBAL(undefined1, DAT_0063d7cc, 0x0063d7cc);
MGLOBAL(undefined1, DAT_0063d7b8, 0x0063d7b8);
MGLOBAL(int, _DAT_0063d798, 0x0063d798);
MGLOBAL(int, _DAT_0063d79c, 0x0063d79c);
MGLOBAL(int, _DAT_0063d7a0, 0x0063d7a0);
MGLOBAL(int, _DAT_0063d7c4, 0x0063d7c4);
MGLOBAL(int, _DAT_0063d7d0, 0x0063d7d0);
MGLOBAL(int, _DAT_0063d7d4, 0x0063d7d4);
MGLOBAL(int, _DAT_0063d7dc, 0x0063d7dc);
MGLOBAL(int, _DAT_0063d7b0, 0x0063d7b0);
MGLOBAL(int, _DAT_0063d7b4, 0x0063d7b4);
MGLOBAL(int, _DAT_0063d7cc, 0x0063d7cc);
MGLOBAL(int, _DAT_0063d7b8, 0x0063d7b8);
// MATCH: 0x0041e080 FUN_0041e080
/* [C1 2026-05-20] Pure leaf — zero-initialises 12 fields of a global struct rooted at
   DAT_0063d798..0063d7dc. */

void FUN_0041e080(void)

{
  _DAT_0063d798 = 0;
  _DAT_0063d79c = 0;
  _DAT_0063d7a0 = 0;
  _DAT_0063d7c4 = 0;
  _DAT_0063d7d0 = 0;
  _DAT_0063d7d4 = 0;
  DAT_0063d7d8 = 0;
  _DAT_0063d7dc = 0;
  _DAT_0063d7b0 = 0;
  _DAT_0063d7b4 = 0;
  _DAT_0063d7cc = 0x47;
  _DAT_0063d7b8 = 0;
  return;
}
