// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x00411580: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs02_20261031/results.tsv. */
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
typedef unsigned int undefined4;
MGLOBAL(int, DAT_0063bb10, 0x0063bb10);
void FUN_00411530(int p0, int p1, undefined4 * p2, undefined4 * p3, undefined4 p4);
// MATCH: 0x00411580 FUN_00411580
undefined4 FUN_00411580(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)



{

  if ((DAT_0063bb10 != 0) && (*(int *)(DAT_0063bb10 + 0x17c + param_1 * 4) != 0)) {

    FUN_00411530(DAT_0063bb10,param_1,param_2,param_3,param_4);

    return 1;

  }

  return 0;

}
