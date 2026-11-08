// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00538c80: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
void FUN_00538d60(undefined4 p0, undefined4 *p1, undefined4 p2, undefined4 p3);
void FUN_00539900(undefined4 p0, undefined4 *p1, undefined4 p2, undefined4 p3);
void FUN_00539ec0(undefined4 p0, undefined4 *p1, undefined4 *p2, undefined4 p3, undefined4 p4);
undefined4 *RpAtomicGetWorldBoundingSphere(undefined4 p0);
// MATCH: 0x00538c80 FUN_00538c80

undefined4
FUN_00538c80(undefined4 param_1,undefined4 *param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  undefined4 loc[4];
  
  switch(param_2[6]) {
  case 1:
    FUN_00538d60(param_1,param_2,param_3,param_4);
    return param_1;
  default:
    return param_1;
  case 3:
    FUN_00539ec0(param_1,param_2,param_2,param_3,param_4);
    return param_1;
  case 4:
    FUN_00539900(param_1,param_2,param_3,param_4);
    return param_1;
  case 5:
    puVar1 = RpAtomicGetWorldBoundingSphere(*param_2);
    loc[0] = *puVar1;
    loc[1] = puVar1[1];
    loc[2] = puVar1[2];
    loc[3] = puVar1[3];
    FUN_00539ec0(param_1,loc,param_2,param_3,param_4);
    return param_1;
  }
}
