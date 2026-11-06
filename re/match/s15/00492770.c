// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00492770: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(undefined4, DAT_00828300, 0x00828300);
MGLOBAL(undefined4, DAT_007f1000, 0x007f1000);
MGLOBAL(undefined4, DAT_007f101c, 0x007f101c);
MGLOBAL(undefined1, DAT_007f1020, 0x007f1020);
MGLOBAL(undefined4, DAT_00771968, 0x00771968);
MGLOBAL(undefined1, DAT_007e9dc0, 0x007e9dc0);
undefined4 FUN_0042b920(void);
void FUN_0042b950(void);
undefined4 FUN_00433240(undefined4 p0);
void FUN_00495110(void);
undefined4 FUN_004c57a0(void);
MGLOBAL(int, _DAT_007f1020, 0x007f1020);
MGLOBAL(int, _DAT_007e9dc0, 0x007e9dc0);
// MATCH: 0x00492770 FUN_00492770
/* [C2 2026-05-16] Main game-loop initializer: clears exit flag (DAT_00828300=0), resets frame-time
   globals, sets game state to 1 (DAT_00771968), returns 1. */

undefined4 FUN_00492770(void)

{
  undefined4 uVar1;
  
  DAT_00828300 = 0;
  FUN_00495110();
  DAT_007f1000 = 0;
  DAT_007f101c = 0;
  _DAT_007f1020 = 0;
  DAT_00771968 = 1;
  uVar1 = FUN_0042b920();
  FUN_00433240(uVar1);
  FUN_0042b950();
  _DAT_007e9dc0 = FUN_004c57a0();
  return 1;
}
