// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00551cf0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
typedef undefined4 (*PFN_b)(undefined4, undefined4);
MGLOBAL(undefined4, DAT_007dc770, 0x007dc770);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_00562560(int p0);
// MATCH: 0x00551cf0 FUN_00551cf0
undefined4 FUN_00551cf0(int param_1)

{
  int *p = *(int **)(param_1 + 0x14);

  if (--p[0x10] == 0) {
    (**(PFN_b *)(DAT_007d3ff8 + 0x11c))(DAT_007dc770,*(undefined4 *)(param_1 + 0x14));
    FUN_00562560(param_1);
  }
  return 1;
}
