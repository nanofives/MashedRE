// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d7ac0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
MGLOBAL(int, DAT_007d6aec, 0x007d6aec);
MGLOBAL(int, DAT_007d6ae8, 0x007d6ae8);
MGLOBAL(int, DAT_007d5834, 0x007d5834);
MGLOBAL(undefined4, DAT_007d5830, 0x007d5830);
MGLOBAL(int, DAT_007d6c14, 0x007d6c14);
MGLOBAL(int, DAT_007d58b4, 0x007d58b4);
MGLOBAL(undefined4, DAT_007d58b0, 0x007d58b0);
MGLOBAL(undefined4, DAT_007d5168, 0x007d5168);
// MATCH: 0x004d7ac0 FUN_004d7ac0
void FUN_004d7ac0(int param_1)

{
  if (param_1 == 0) {
    if (DAT_007d6aec != 0) {
      if ((DAT_007d6ae8 == 0) && (DAT_007d5830 = 0, DAT_007d5834 == 0)) {
        DAT_007d5834 = 1;
        (&DAT_007d5168)[DAT_007d6c14] = 7;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
      DAT_007d58b0 = 8;
      if (DAT_007d58b4 == 0) {
        DAT_007d58b4 = 1;
        (&DAT_007d5168)[DAT_007d6c14] = 0x17;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
      DAT_007d6aec = 0;
    }
  }
  else if (DAT_007d6aec == 0) {
    if ((DAT_007d6ae8 == 0) && (DAT_007d5830 = 1, DAT_007d5834 == 0)) {
      DAT_007d5834 = 1;
      (&DAT_007d5168)[DAT_007d6c14] = 7;
      DAT_007d6c14 = DAT_007d6c14 + 1;
    }
    DAT_007d58b0 = 4;
    if (DAT_007d58b4 == 0) {
      DAT_007d58b4 = 1;
      (&DAT_007d5168)[DAT_007d6c14] = 0x17;
      DAT_007d6c14 = DAT_007d6c14 + 1;
    }
    DAT_007d6aec = 1;
    return;
  }
  return;
}
