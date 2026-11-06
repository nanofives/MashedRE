// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047acd0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef long double float10;
MGLOBAL(int, DAT_006bf1cc, 0x006bf1cc);
int FUN_004b6fc0(int * p0);
float10 FUN_004b7090(undefined4 p0, undefined4 p1);
// MATCH: 0x0047acd0 FUN_0047acd0
undefined4 FUN_0047acd0(undefined4 param_1)

{
  undefined4 uVar1;
  float10 fVar2;
  
  uVar1 = FUN_004b6fc0(param_1);
  fVar2 = (float10)FUN_004b7090(param_1,uVar1);
  *(float *)(DAT_006bf1cc + 0x20c) = (float)fVar2;
  return 0;
}
