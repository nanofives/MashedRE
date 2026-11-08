// FLAGS: /O2 /Ob1 /G5
// C++ check edits (casts, code(...) typedef under __cplusplus): DDK bytes unchanged, relocheck re-run OK.
/* MATCH-RWL04 0x005540d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(undefined4, DAT_00623e90, 0x00623e90);
MGLOBAL(undefined4, DAT_00623e8c, 0x00623e8c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_00912a24, 0x00912a24);
MGLOBAL(undefined4 *, DAT_00912a28, 0x00912a28);
MGLOBAL(undefined1, DAT_007dc77c, 0x007dc77c);
int * FUN_00553f40(int * p0);
uint * thunk_FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
// MATCH: 0x005540d0 FUN_005540d0
undefined4 FUN_005540d0(void)

{
  undefined4 *puVar1;
  
  DAT_00912a24 = (undefined4)thunk_FUN_004cc820(0x1c,DAT_00623e8c,4,DAT_00623e90,(uint *)&DAT_007dc77c,0x401a1);
  puVar1 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_00912a24,0x301a1);
  if (puVar1 != (undefined4 *)0x0) {
    *puVar1 = 0;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 1;
    puVar1[5] = 0;
    puVar1[6] = (undefined4)puVar1;
    puVar1[4] = 0;
    FUN_00553f40((int *)puVar1);
  }
  DAT_00912a28 = puVar1;
  FUN_00553f40((int *)puVar1);
  return 1;
}
