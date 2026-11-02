// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042bfb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
MGLOBAL(int, DAT_0067eab0, 0x0067eab0);
MGLOBAL(undefined4, DAT_0067e91c, 0x0067e91c);
MGLOBAL(undefined4, DAT_0067e924, 0x0067e924);
MGLOBAL(undefined4, DAT_0067e928, 0x0067e928);
MGLOBAL(undefined4, DAT_0067e92c, 0x0067e92c);
MGLOBAL(undefined4, DAT_0067e920, 0x0067e920);
MGLOBAL(undefined4, DAT_0067e918, 0x0067e918);
MGLOBAL(undefined4, DAT_0067e930, 0x0067e930);
void FUN_0042bf30();
// MATCH: 0x0042bfb0 FUN_0042bfb0
void FUN_0042bfb0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5,undefined4 param_6)

{
  if (DAT_0067eab0 == 0) {
    FUN_0042bf30();
    return;
  }
  DAT_0067e91c = param_2;
  DAT_0067e924 = param_4;
  DAT_0067e928 = param_5;
  DAT_0067e92c = param_6;
  DAT_0067e920 = param_3;
  DAT_0067e918 = param_1;
  DAT_0067e930 = 1;
  return;
}
