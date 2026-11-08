// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c1980: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
MGLOBAL(undefined4, DAT_00617fb4, 0x00617fb4);
MGLOBAL(undefined4, DAT_00617fb0, 0x00617fb0);
MGLOBAL(int, DAT_007d3ec0, 0x007d3ec0);
MGLOBAL(undefined4, DAT_00617f98, 0x00617f98);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3ec4, 0x007d3ec4);
MGLOBAL(undefined1, DAT_007d3e9c, 0x007d3e9c);
uint * thunk_FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
// MATCH: 0x004c1980 FUN_004c1980

undefined4 FUN_004c1980(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  
  DAT_007d3ec0 = param_2;
  uVar1 = (undefined4)thunk_FUN_004cc820(DAT_00617f98,DAT_00617fb0,4,DAT_00617fb4,(uint *)&DAT_007d3e9c,0x40005);
  *(undefined4 *)(DAT_007d3ff8 + DAT_007d3ec0) = uVar1;
  if (*(int *)(DAT_007d3ff8 + DAT_007d3ec0) == 0) {
    return 0;
  }
  DAT_007d3ec4 = DAT_007d3ec4 + 1;
  return param_1;
}
