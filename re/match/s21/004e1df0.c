// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004e1df0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
typedef unsigned int uint;
#ifndef __cplusplus
#define true 1
#endif
undefined4 * FUN_004cc050(undefined4 * p0, int p1);
undefined4 FUN_004cc400(undefined4 p0, undefined4 * p1, undefined4 * p2, uint * p3, uint * p4);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
// MATCH: 0x004e1df0 FUN_004e1df0
undefined4 FUN_004e1df0(undefined4 param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int local_4;
  
  iVar1 = param_2;
  iVar2 = FUN_004cc5e0(param_2,3,&param_2,0);
  if (iVar2 != 0) {
    while( true ) {
      if (param_2 == 0) {
        return param_1;
      }
      iVar2 = FUN_004cc400(iVar1,0,&local_4,0,0);
      if ((iVar2 == 0) || (iVar2 = FUN_004cc050(iVar1,local_4), iVar2 == 0)) break;
      param_2 = param_2 + (-0xc - local_4);
    }
  }
  return 0;
}
