// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00490fb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs15_20261102/results.tsv. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_006146c0, 0x006146c0);
MGLOBAL(undefined4, DAT_006146c4, 0x006146c4);
MGLOBAL(undefined4, DAT_00771538, 0x00771538);
MGLOBAL(undefined4, DAT_0077153c, 0x0077153c);
MGLOBAL(undefined4, DAT_006146c8, 0x006146c8);
MGLOBAL(undefined4, DAT_006146cc, 0x006146cc);
// MATCH: 0x00490fb0 FUN_00490fb0
void FUN_00490fb0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5,undefined4 param_6)

{
  DAT_006146c0 = param_1;
  DAT_006146c4 = param_2;
  DAT_00771538 = param_3;
  DAT_0077153c = param_4;
  DAT_006146c8 = param_5;
  DAT_006146cc = param_6;
  return;
}
