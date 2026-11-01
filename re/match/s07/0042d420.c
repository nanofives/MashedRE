// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042d420: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs07_20261101/results.tsv. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
void FUN_00467020(int p0);
void FUN_004c0740(int p0, int p1);
int FUN_004c0b30(void);
undefined1 * FUN_004c1d30(void);
int FUN_004c77c0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
// MATCH: 0x0042d420 FUN_0042d420
int FUN_0042d420(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (int)FUN_004c1d30();
  if (iVar1 != 0) {
    uVar2 = FUN_004c0b30();
    FUN_004c0740(iVar1,uVar2);
    uVar2 = FUN_004c77c0(param_1,param_2,0,2);
    *(undefined4 *)(iVar1 + 0x60) = uVar2;
    if (param_3 != 0) {
      uVar2 = FUN_004c77c0(param_1,param_2,0,1);
      *(undefined4 *)(iVar1 + 100) = uVar2;
    }
    if (((*(int *)(iVar1 + 4) != 0) && (*(int *)(iVar1 + 0x60) != 0)) &&
       ((param_3 == 0 || (*(int *)(iVar1 + 100) != 0)))) {
      return iVar1;
    }
  }
  FUN_00467020(iVar1);
  return 0;
}
