// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x0048ad50: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
typedef int code();
MGLOBAL(int, DAT_0071fa30, 0x0071fa30);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_0071ba30, 0x0071ba30);
undefined4 FUN_00499d90(int p0, int p1);
// MATCH: 0x0048ad50 FUN_0048ad50
void FUN_0048ad50(void)

{
  if (DAT_0071fa30 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
    (**(code **)(DAT_007d3ff8 + 0x20))(1,0);
    (**(code **)(DAT_007d3ff8 + 0x20))(10,5);
    (**(code **)(DAT_007d3ff8 + 0x20))(0xb,2);
    FUN_00499d90(&DAT_0071ba30,DAT_0071fa30);
    (**(code **)(DAT_007d3ff8 + 0x20))(10,5);
    (**(code **)(DAT_007d3ff8 + 0x20))(0xb,6);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(0x14,2);
  }
  return;
}
