// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00551190: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef void (*PFV)(void);
typedef int (*PFN_a2)(int,int);
typedef int (*PFN_a3)(int,int,int);
typedef void (*PFN_fr)(int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_00550c30(void);
void FUN_00550c50(void);
void FUN_00550d80(void);
void FUN_00550da0(void);
void FUN_00550f20(void);
void FUN_005512b0(void);
void FUN_00551050(void);
void FUN_00551180(void);
void FUN_00551290(void);
void FUN_005512d0(void);
void FUN_00551090(void);
void FUN_005514e0(void);
int FUN_005504d0(undefined4 p0);
undefined4 FUN_00551330(int p0, int p1, undefined4 p2, undefined4 p3);
// MATCH: 0x00551190 FUN_00551190

int FUN_00551190(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  iVar1 = FUN_005504d0(param_3);
  if (iVar1 != 0) {
    return 0;
  }
  iVar1 = (**(PFN_a2 *)(DAT_007d3ff8 + 0x108))(0x5c,0x401be);
  if (iVar1 != 0) {
    *(PFV *)(iVar1 + 0x14) = FUN_00550c30;
    *(PFV *)(iVar1 + 0x18) = FUN_00551290;
    *(PFV *)(iVar1 + 0x28) = FUN_00550c50;
    *(PFV *)(iVar1 + 0x2c) = FUN_00550d80;
    *(PFV *)(iVar1 + 0x30) = FUN_00550da0;
    *(PFV *)(iVar1 + 0x34) = FUN_00550f20;
    *(PFV *)(iVar1 + 0x38) = FUN_00551050;
    *(PFV *)(iVar1 + 0x3c) = FUN_00551090;
    *(PFV *)(iVar1 + 0x40) = FUN_00551180;
    *(PFV *)(iVar1 + 0x44) = FUN_005514e0;
    *(PFV *)(iVar1 + 0x48) = FUN_005512b0;
    *(PFV *)(iVar1 + 0x4c) = FUN_005512d0;
    iVar2 = (**(PFN_a3 *)(DAT_007d3ff8 + 0x114))(param_1,0x60,0x401be);
    *(int *)(iVar1 + 0x58) = iVar2;
    if (0 < param_1) {
      piVar3 = (int *)(iVar2 + 0x38);
      iVar2 = param_1;
      do {
        *piVar3 = iVar1;
        piVar3 = piVar3 + 0x18;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
    }
    iVar2 = FUN_00551330(iVar1,param_1,param_3,param_2);
    if (iVar2 != 1) {
      (**(PFN_fr *)(DAT_007d3ff8 + 0x10c))(*(undefined4 *)(iVar1 + 0x58));
      (**(PFN_fr *)(DAT_007d3ff8 + 0x10c))(iVar1);
      return 0;
    }
  }
  return iVar1;
}
