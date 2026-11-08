// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00556d20: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl05_20261107/results.tsv. */
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
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_00623f64, 0x00623f64);
MGLOBAL(undefined4, DAT_00623f60, 0x00623f60);
MGLOBAL(undefined4, DAT_00912a04, 0x00912a04);
MGLOBAL(undefined4, DAT_00912a00, 0x00912a00);
MGLOBAL(undefined1, DAT_007dc7e8, 0x007dc7e8);
uint * thunk_FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
// MATCH: 0x00556d20 FUN_00556d20
undefined4 FUN_00556d20(void)

{
  DAT_00912a04 = (**(code **)(DAT_007d3ff8 + 0x108))(0x2400,0x40190);
  DAT_00912a00 = (undefined4)thunk_FUN_004cc820(0x78,DAT_00623f60,0x10,DAT_00623f64,(uint *)&DAT_007dc7e8,0x401a2);
  return 1;
}
