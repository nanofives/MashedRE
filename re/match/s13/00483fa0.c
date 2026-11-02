// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00483fa0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
undefined4 FUN_004c0c20(int p0);
undefined4 FUN_004c1cf0(undefined4 p0);
undefined4 * FUN_004c5860(undefined4 * p0, int p1);
undefined4 FUN_004c5a60(int * p0);
undefined4 FUN_004c7650(undefined4 p0);
// MATCH: 0x00483fa0 FUN_00483fa0
undefined4 FUN_00483fa0(int param_1,int param_2)

{
  if (param_1 != 0) {
    if (*(int *)(param_1 + 4) != 0) {
      FUN_004c0c20(*(int *)(param_1 + 4));
    }
    if (*(int *)(param_1 + 0x60) != 0) {
      FUN_004c7650(*(int *)(param_1 + 0x60));
    }
    if (*(int *)(param_1 + 100) != 0) {
      FUN_004c7650(*(int *)(param_1 + 100));
    }
    FUN_004c1cf0(param_1);
  }
  if (param_2 != 0) {
    FUN_004c5860(param_2,0);
    FUN_004c5a60(param_2);
  }
  return 0;
}
