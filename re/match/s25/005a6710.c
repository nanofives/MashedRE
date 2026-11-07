// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a6710: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
void FUN_005a6df0(int p0, int p1, int p2, undefined4 p3);
// MATCH: 0x005a6710 FUN_005a6710
void FUN_005a6710(int param_1,int param_2)

{
  if (*(int *)(param_1 + 0x50) != 0) {
    *(undefined4 *)(param_1 + 0x50) = 0;
  }
  if (param_2 != 0) {
    *(int *)(param_1 + 0x50) = param_2;
    if (*(int *)(param_1 + 0x84) == 0) {
      *(float *)(param_1 + 0x3c) = (float)*(uint *)(param_2 + 0x2c);
    }
  }
  if (*(int *)(param_1 + 0x30) != 0) {
    FUN_005a6df0(*(int *)(param_1 + 0x30),9,1,*(undefined4 *)(param_1 + 0x50));
  }
  return;
}
