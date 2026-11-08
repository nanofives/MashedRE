// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00538bd0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
int FUN_00538c30(int p0, int p1, int p2);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
int RpWorldForAllWorldSectors(int p0, int (*p1)(int, int, int), void *p2);
// MATCH: 0x00538bd0 FUN_00538bd0
undefined4 FUN_00538bd0(int param_1)

{
  int iVar1;
  undefined4 loc[2];
  
  iVar1 = param_1;
  if (param_1 != 0) {
    param_1 = 0;
    RpWorldForAllWorldSectors(iVar1,FUN_00538c30,&param_1);
    return param_1;
  }
  loc[0] = 0x12a;
  loc[1] = FUN_004d7ff0(0x80000016);
  FUN_004d8480(loc);
  return 0xffffffff;
}
