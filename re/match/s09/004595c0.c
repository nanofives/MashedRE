// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004595c0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs09_20261102/results.tsv. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_0068b9fc, 0x0068b9fc);
MGLOBAL(undefined4, DAT_0068ba08, 0x0068ba08);
MGLOBAL(undefined1, DAT_0068ba2c, 0x0068ba2c);
MGLOBAL(undefined1, DAT_0068ba2d, 0x0068ba2d);
MGLOBAL(undefined1, DAT_0068ba2e, 0x0068ba2e);
MGLOBAL(undefined1, DAT_0068ba2f, 0x0068ba2f);
MGLOBAL(undefined4, DAT_0068ba4c, 0x0068ba4c);
void FUN_00474d60(int p0);
void FUN_004b5320(undefined4 p0, undefined4 p1, undefined4 p2);
// MATCH: 0x004595c0 FUN_004595c0
void FUN_004595c0(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = param_1 * 0x58;
  (&DAT_0068ba08)[param_1 * 0x16] = 1;
  (&DAT_0068b9fc)[param_1 * 0x16] = 0x43340000;
  FUN_00474d60((&DAT_0068ba4c)[param_1 * 0x16],1);
  uVar1 = (&DAT_0068ba4c)[param_1 * 0x16];
  (&DAT_0068ba2c)[iVar2] = 0xff;
  (&DAT_0068ba2d)[iVar2] = 0;
  (&DAT_0068ba2e)[iVar2] = 0;
  (&DAT_0068ba2f)[iVar2] = 200;
  FUN_004b5320(uVar1,0x40,1);
  return;
}
