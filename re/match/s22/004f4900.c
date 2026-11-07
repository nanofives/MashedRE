// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f4900: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs22_20261107/results.tsv. */
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
typedef int code(int);
MGLOBAL(int, DAT_007d7384, 0x007d7384);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7388, 0x007d7388);
MGLOBAL(undefined4, DAT_007d7378, 0x007d7378);
MGLOBAL(undefined4, DAT_007d737c, 0x007d737c);
MGLOBAL(undefined4, DAT_007d7380, 0x007d7380);
MGLOBAL(int, DAT_007d7394, 0x007d7394);
MGLOBAL(undefined1, DAT_007d738c, 0x007d738c);
MGLOBAL(undefined1, DAT_007d7390, 0x007d7390);
MGLOBAL(undefined1, DAT_007d7398, 0x007d7398);
MGLOBAL(undefined1, DAT_007d739c, 0x007d739c);
MGLOBAL(int, _DAT_007d738c, 0x007d738c);
MGLOBAL(int, _DAT_007d7390, 0x007d7390);
MGLOBAL(int, _DAT_007d7398, 0x007d7398);
MGLOBAL(int, _DAT_007d739c, 0x007d739c);
// MATCH: 0x004f4900 FUN_004f4900
/* [C1 2026-05-20] Signature: `void FUN_004f4900(void)`. */

void FUN_004f4900(void)

{
  if (DAT_007d7384 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(DAT_007d7384);
    DAT_007d7384 = 0;
  }
  DAT_007d7378 = 0;
  DAT_007d737c = 0;
  DAT_007d7380 = 0;
  if (DAT_007d7388 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(DAT_007d7388);
    DAT_007d7388 = 0;
  }
  _DAT_007d738c = 0;
  _DAT_007d7390 = 0;
  if (DAT_007d7394 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(DAT_007d7394);
    DAT_007d7394 = 0;
  }
  _DAT_007d7398 = 0;
  _DAT_007d739c = 0;
  return;
}
