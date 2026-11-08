// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005b1030: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs26_20261107/results.tsv. */
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
void FUN_005b10e0(undefined4 p0, int p1, int p2, undefined4 *p3);
void thunk_FUN_005b10a0(undefined4 p0, int p1, int p2, undefined4 *p3);
// MATCH: 0x005b1030 FUN_005b1030
void FUN_005b1030(int param_1,undefined4 param_2,int param_3,undefined4 param_4)

{
  undefined4 local_8 [2];
  
  local_8[0] = param_4;
  FUN_005b10e0(param_3,*(undefined4 *)(param_3 + 0x28),1,local_8);
  local_8[0] = param_2;
  thunk_FUN_005b10a0(param_1,*(undefined4 *)(param_1 + 0x28),1,local_8);
  return;
}
