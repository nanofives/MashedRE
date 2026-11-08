// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005ac980: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs25_20261107/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned char byte;
// MATCH: 0x005ac980 FUN_005ac980

void FUN_005ac980(undefined4 *param_1,undefined4 *param_2,int param_3)

{
  if (param_3 != 0) {
    *param_2 = 0;
    param_2[1] = 0;
    param_2[2] = 0;
    *(undefined1 *)(param_2 + 3) = 0;
    *(undefined1 *)((int)param_2 + 0xd) = 0;
    *(undefined1 *)(param_2 + 6) = 0;
    *(undefined1 *)((int)param_2 + 0x19) = 0;
    param_2[4] = 0;
    param_2[5] = 0;
  }
  *(undefined1 *)(param_2 + 3) = *(undefined1 *)(param_1 + 1);
  param_2[1] = *param_1;
  *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 5);
  *(undefined1 *)(param_2 + 6) = 0;
  if ((*(byte *)(param_1 + 4) & 1) != 0) {
    *(undefined1 *)(param_2 + 6) = 1;
  }
  if ((*(byte *)(param_1 + 4) & 4) != 0) {
    *(byte *)(param_2 + 6) = *(byte *)(param_2 + 6) | 4;
  }
  return;
}
