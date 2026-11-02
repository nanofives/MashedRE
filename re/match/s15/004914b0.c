// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x004914b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef int code();
MGLOBAL(int, DAT_00771534, 0x00771534);
MGLOBAL(int, DAT_00771540, 0x00771540);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_00771530, 0x00771530);
undefined4 FUN_00499d90(int p0, int p1);
// MATCH: 0x004914b0 FUN_004914b0

void FUN_004914b0(void)

{
  if (DAT_00771534 != 0) {
    if (DAT_00771540 == 0) {
      (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
      (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
      (**(code **)(DAT_007d3ff8 + 0x20))(0xc,1);
      (**(code **)(DAT_007d3ff8 + 0x20))(9,2);
      DAT_00771540 = 1;
    }
    (**(code **)(DAT_007d3ff8 + 0x20))(1,0);
    FUN_00499d90(DAT_00771530,0x380);
  }
  return;
}
