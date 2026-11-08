// FLAGS: /O2 /Ob1 /G5
// C++ check edits (casts, code(...) typedef under __cplusplus): DDK bytes unchanged, relocheck re-run OK.
/* MATCH-RWL04 0x00543a40: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(int, DAT_007dc5f4, 0x007dc5f4);
MGLOBAL(int, DAT_007dc5f8, 0x007dc5f8);
MGLOBAL(int, DAT_007dc5fc, 0x007dc5fc);
MGLOBAL(int, DAT_007dc600, 0x007dc600);
MGLOBAL(int, DAT_007dc604, 0x007dc604);
MGLOBAL(int, DAT_007dc608, 0x007dc608);
MGLOBAL(int, DAT_007dc60c, 0x007dc60c);
MGLOBAL(int, DAT_007dc610, 0x007dc610);
void FUN_004cbb50(int * p0);
void FUN_004d41e0(int p0);
// MATCH: 0x00543a40 FUN_00543a40
undefined4 FUN_00543a40(void)

{
  if (DAT_007dc5f4 != 0) {
    FUN_004d41e0(DAT_007dc5f4);
    DAT_007dc5f4 = 0;
  }
  if (DAT_007dc5f8 != 0) {
    FUN_004d41e0(DAT_007dc5f8);
    DAT_007dc5f8 = 0;
  }
  if (DAT_007dc5fc != 0) {
    FUN_004cbb50((int *)DAT_007dc5fc);
    DAT_007dc5fc = 0;
  }
  if (DAT_007dc600 != 0) {
    FUN_004cbb50((int *)DAT_007dc600);
    DAT_007dc600 = 0;
  }
  if (DAT_007dc604 != 0) {
    FUN_004cbb50((int *)DAT_007dc604);
    DAT_007dc604 = 0;
  }
  if (DAT_007dc608 != 0) {
    FUN_004cbb50((int *)DAT_007dc608);
    DAT_007dc608 = 0;
  }
  if (DAT_007dc60c != 0) {
    FUN_004cbb50((int *)DAT_007dc60c);
    DAT_007dc60c = 0;
  }
  if (DAT_007dc610 != 0) {
    FUN_004cbb50((int *)DAT_007dc610);
    DAT_007dc610 = 0;
  }
  return 1;
}
