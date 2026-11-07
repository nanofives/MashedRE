// FLAGS: /O2 /Ob1 /G5
/* C++-check edits: casts/prototypes only, DDK bytes unchanged */
/* MATCH-S28 0x005bd9e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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
MGLOBAL(void *, PTR_LAB_00634274, 0x00634274);
undefined4 FUN_005b0f10(undefined4 p0, undefined4 * p1);
// MATCH: 0x005bd9e0 FUN_005bd9e0
undefined4 FUN_005bd9e0(undefined4 param_1,undefined4 *param_2)

{
  FUN_005b0f10(param_1,param_2);
  param_2[7] = 0;
  param_2[8] = 0;
  param_2[9] = 0;
  *(undefined1 *)(param_2 + 10) = 0;
  *(undefined1 *)((int)param_2 + 0x29) = 0;
  *(undefined1 *)(param_2 + 0xd) = 0;
  *(undefined1 *)((int)param_2 + 0x35) = 0;
  param_2[0xb] = 0;
  param_2[0xc] = 0;
  param_2[0xe] = 0;
  param_2[0xf] = 0;
  param_2[0x10] = 0;
  *(undefined1 *)(param_2 + 0x11) = 0;
  *(undefined1 *)((int)param_2 + 0x45) = 0;
  *(undefined1 *)(param_2 + 0x14) = 0;
  *(undefined1 *)((int)param_2 + 0x51) = 0;
  param_2[0x12] = 0;
  param_2[0x13] = 0;
  *param_2 = 1;
  param_2[1] = 1;
  param_2[0x17] = (undefined4)&PTR_LAB_00634274;
  param_2[0x15] = 0x400;
  param_2[0x16] = 4;
  return param_1;
}
