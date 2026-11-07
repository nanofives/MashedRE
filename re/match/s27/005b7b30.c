// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b7b30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs27_20261107/results.tsv. */
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
typedef unsigned char byte;
void FUN_005b0f90(int p0, undefined4 p1, int * p2);
#ifdef __cplusplus
void FUN_005b11d0(...);
#else
void FUN_005b11d0(byte * p0);
#endif
void FUN_005b29e0(int p0, uint p1);
// MATCH: 0x005b7b30 FUN_005b7b30
void FUN_005b7b30(uint *param_1,int param_2)

{
  uint uVar1;
  uint *puVar2;
  int iVar3;
  
  iVar3 = param_2;
  puVar2 = param_1;
  uVar1 = *param_1;
  if ((uVar1 & 8) != 0) {
    param_1 = (uint *)0x1;
    *puVar2 = uVar1 & 0xfffffff7;
    FUN_005b0f90(param_2,0,(int *)&param_1);
  }
  FUN_005b29e0(iVar3 + 0x5c,puVar2[1]);
  FUN_005b11d0(iVar3 + 0x1f8,puVar2);
  return;
}
