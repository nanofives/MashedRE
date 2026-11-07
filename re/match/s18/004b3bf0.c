// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b3bf0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs18_20261107/results.tsv. */
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
void FUN_004c0e50(int p0);
bool FUN_004cc160(undefined4 p0, undefined4 p1);
undefined4 FUN_004cc230(undefined4 p0, int p1, undefined4 p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 p2, undefined4 p3);
int FUN_004e7420(int p0);
// MATCH: 0x004b3bf0 FUN_004b3bf0
int FUN_004b3bf0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  iVar1 = FUN_004cc230(2,1,param_1);
  if (iVar1 == 0) {
    return 0;
  }
  iVar2 = FUN_004cc5e0(iVar1,0x10,param_2,param_3);
  if (iVar2 != 0) {
    iVar3 = FUN_004e7420(iVar1);
    if (iVar3 != 0) {
      FUN_004c0e50(*(undefined4 *)(iVar3 + 4));
    }
  }
  FUN_004cc160(iVar1,0);
  return iVar3;
}
