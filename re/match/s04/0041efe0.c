// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041efe0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char byte;
MGLOBAL(undefined1, DAT_0063dc74, 0x0063dc74);
/* C++ port: return the full EAX value the original sets (0x0041efe0 widens before RET); the DDK build keeps byte. */
#ifdef __cplusplus
#define MATCH_RET_0041efe0 unsigned int
#else
#define MATCH_RET_0041efe0 byte
#endif
// MATCH: 0x0041efe0 FUN_0041efe0
MATCH_RET_0041efe0 FUN_0041efe0(int param_1)

{
  return (byte)(&DAT_0063dc74)[param_1 * 0x2ac] >> 3 & 1;
}
