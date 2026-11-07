// FLAGS: /O1
/* MATCH-S25 0x005abe30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs25_20261107/results.tsv. */
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
typedef int code(int, int *, int, int);
void FUN_004522d0(int p0);
undefined4 FUN_005ac5f0(int * p0, int * p1);
void FUN_005ac740(int p0);
undefined4 FUN_005aea00(int p0, undefined4 p1);
undefined4 FUN_005b3b30(int p0, int p1, undefined4 p2, int * p3);
uint FUN_005b3b80(int * p0, int p1, int p2, int p3, int p4);
// MATCH: 0x005abe30 FUN_005abe30
int FUN_005abe30(int param_1,int param_2,undefined4 param_3,int param_4)

{
  code *pcVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int *piVar7;
  bool bVar8;
  int local_2c;
  int local_1c [7];
  
  iVar4 = param_4;
  pcVar1 = *(code **)(*(int *)(param_1 + 0xc) + 0x18);
  local_2c = 0;
  if (pcVar1 == (code *)0x0) {
    return 0;
  }
  if (param_2 == 0) {
    param_2 = param_1 + 0x2c;
  }
  if (*(int *)(param_2 + 4) == 0) {
    *(undefined4 *)(param_2 + 4) = *(undefined4 *)(param_1 + 0x30);
  }
  bVar8 = *(int *)(param_2 + 0x10) == 0;
  iVar2 = FUN_005b3b30(param_2,param_4,param_3,piVar6);
  iVar5 = *(int *)(param_1 + 0x34) - iVar2;
  iVar3 = FUN_005ac5f0(piVar6,(int *)param_2);
  piVar6 = (int *)(param_1 + 0x2c);
  if ((iVar3 == 0) && (param_4 = FUN_005aea00(iVar5,0x30806), local_2c = param_4, param_4 == 0)) {
    return 0;
  }
  piVar7 = local_1c;
  for (iVar3 = 7; iVar3 != 0; iVar3 = iVar3 + -1) {
    *piVar7 = *piVar6;
    piVar6 = piVar6 + 1;
    piVar7 = piVar7 + 1;
  }
  local_1c[2] = iVar5;
  iVar3 = (*pcVar1)(param_1,local_1c,iVar2,param_4);
  if (iVar3 == 0) {
    return 0;
  }
  if (local_2c != 0) {
    iVar4 = FUN_005b3b80(local_1c,param_4,iVar2,param_2,iVar4);
    if (iVar4 == 0) {
      if (bVar8) {
        FUN_005ac740(param_2);
      }
      FUN_004522d0(local_2c);
      return 0;
    }
    if (bVar8) {
      FUN_005ac740(param_2);
    }
    FUN_004522d0(local_2c);
  }
  return param_1;
}
