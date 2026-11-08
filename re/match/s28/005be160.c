// FLAGS: /O2 /Ob1 /G5
/* C++-check edits: casts/prototypes only, DDK bytes unchanged */
/* MATCH-S28 0x005be160: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef long LONG;
void FUN_005be140(int p0, undefined4 p1, undefined4 p2);
void FUN_005be190(undefined4 * p0);
LONG __stdcall FUN_005bf690(int p0);
// MATCH: 0x005be160 FUN_005be160
void __stdcall FUN_005be160(int param_1)

{
  if (*(int *)(param_1 + 4) == 1) {
    FUN_005be140(param_1,0,0);
    FUN_005be190((undefined4 *)param_1);
  }
  FUN_005bf690(param_1);
  return;
}
