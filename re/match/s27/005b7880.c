// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b7880: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs27_20261107/results.tsv. */
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
typedef unsigned int undefined4;
undefined4 FUN_005ad2e0(int p0);
#ifdef __cplusplus
void FUN_005b0f40(...);
#else
void FUN_005b0f40(int p0);
#endif
void FUN_005b7e20(int p0, int p1);
void thunk_FUN_005b1500(int p0);
// MATCH: 0x005b7880 FUN_005b7880
void FUN_005b7880(int param_1,undefined4 param_2,undefined4 param_3)

{
  FUN_005b7e20(param_1,1);
  FUN_005ad2e0(*(undefined4 *)(param_1 + 0x58));
  thunk_FUN_005b1500(param_1 + 0x5c);
  FUN_005b0f40(param_1,param_2,param_3);
  return;
}
