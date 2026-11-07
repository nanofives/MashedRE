// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005abd30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef int code(int, undefined4, int, undefined4);
undefined4 FUN_005abf80(int p0);
undefined4 FUN_005ac5f0(int * p0, int * p1);
uint FUN_005aea10(int p0, undefined4 p1);
void FUN_005aea40(int p0);
undefined4 FUN_005b3b30(undefined4 * p0, int p1, undefined4 p2, int p3);
undefined4 FUN_005b3b60(undefined4 * p0, int p1, int p2);
uint FUN_005b3b80(undefined4 * p0, int p1, int p2, int p3, int p4);
// MATCH: 0x005abd30 FUN_005abd30

undefined4 FUN_005abd30(int param_1,undefined4 param_2,int param_3,undefined4 param_4)

{
  code *pcVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  int iVar8;
  undefined4 local_1c [7];
  
  iVar3 = param_1 + 0x2c;
  pcVar1 = *(code **)(*(int *)(param_1 + 0xc) + 0x14);
  if (pcVar1 == (code *)0x0) {
    return 0;
  }
  iVar2 = FUN_005ac5f0((int *)(param_1 + 0x10),(int *)iVar3);
  iVar5 = param_3;
  iVar8 = 0;
  if (iVar2 == 0) {
    puVar7 = local_1c;
    puVar6 = (undefined4 *)(param_1 + 0x10);
    for (iVar5 = 7; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar7 = *puVar6;
      puVar6 = puVar6 + 1;
      puVar7 = puVar7 + 1;
    }
    param_2 = FUN_005b3b30(local_1c,param_3,param_2,iVar3);
    local_1c[2] = param_4;
    param_4 = FUN_005b3b60(local_1c,param_3,iVar3);
    iVar5 = FUN_005aea10(param_4,0x30806);
    if (iVar5 == 0) {
      return 0;
    }
    iVar3 = FUN_005b3b80(local_1c,param_3,0,iVar3,iVar5);
    iVar8 = iVar5;
    if (iVar3 == 0) {
      FUN_005aea40(iVar5);
      return 0;
    }
  }
  uVar4 = (*pcVar1)(param_1,param_2,iVar5,param_4);
  if (iVar8 != 0) {
    do {
      iVar3 = FUN_005abf80(uVar4);
    } while (iVar3 != 0);
    FUN_005aea40(iVar8);
  }
  return uVar4;
}
