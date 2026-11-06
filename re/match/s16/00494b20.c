// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00494b20: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs16_20261102/results.tsv. */
/* ---- MATCH-S16 preamble (same text in every re/match/s16 source; not part of any function body) ----
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
typedef unsigned char undefined;
typedef unsigned int undefined4;
MGLOBAL(undefined *, PTR_LAB_005cf9d0, 0x005cf9d0);
MGLOBAL(undefined *, PTR_FUN_005cfd30, 0x005cfd30);
MGLOBAL(undefined *, PTR_LAB_005cfd70, 0x005cfd70);
MGLOBAL(undefined *, PTR_LAB_005d0e74, 0x005d0e74);
MGLOBAL(undefined *, PTR_LAB_005d0e88, 0x005d0e88);
void __fastcall FUN_0049ebc0();
// MATCH: 0x00494b20 FUN_00494b20
void __fastcall __fastcall FUN_00494b20(undefined4 *param_1)

{
  *param_1 = &PTR_LAB_005cfd70;
  param_1[3] = &PTR_FUN_005cfd30;
  param_1[4] = &PTR_LAB_005cf9d0;
  param_1[0x38] = &PTR_LAB_005d0e88;
  param_1[0x39] = &PTR_LAB_005d0e74;
  FUN_0049ebc0();
  return;
}
