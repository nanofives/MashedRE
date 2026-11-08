// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 24.6% (regs 45.6%), class b. Not worked beyond raw output (budget spent on higher-yield rows). Two source copies are visible in the original (param_2 given vs default palette 0x005d8b90, different register allocation, flag 0x007d4154 = 0 vs 1 only when index 0x100): needs a hand rewrite as two specialised bodies; compare loop keeps its counter in [ebp-4] in the first copy. */
/* MATCH-RWL01 0x004cb330: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(undefined4, DAT_007d4568, 0x007d4568);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(undefined4, DAT_007d4154, 0x007d4154);
MGLOBAL(undefined4, DAT_005d8b90, 0x005d8b90);
MGLOBAL(undefined4, DAT_007d4158, 0x007d4158);
// MATCH: 0x004cb330 FUN_004cb330

bool FUN_004cb330(int param_1,int *param_2)

{
  undefined4 uVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  int *piVar6;
  undefined4 *puVar7;
  int local_8;
  
  iVar3 = (&DAT_007d4158)[param_1];
  if (param_2 == (int *)0x0) {
    if (iVar3 == 0) {
LAB_004cb41c:
      uVar1 = (**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d4568,0x30411);
      (&DAT_007d4158)[param_1] = uVar1;
    }
    else {
      piVar2 = (int *)&DAT_005d8b90;
      iVar4 = 0x10;
      while (*(int *)(iVar3 + -0x5d8b90 + (int)piVar2) == *piVar2) {
        piVar2 = piVar2 + 1;
        iVar4 = iVar4 + -1;
        if (iVar4 == 0) {
          return true;
        }
      }
      if (iVar3 == 0) goto LAB_004cb41c;
    }
    puVar5 = &DAT_005d8b90;
    puVar7 = (undefined4 *)(&DAT_007d4158)[param_1];
    for (iVar3 = 0x10; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar7 = *puVar5;
      puVar5 = puVar5 + 1;
      puVar7 = puVar7 + 1;
    }
    iVar3 = (**(code **)(*DAT_007d4110 + 0xb0))(DAT_007d4110,param_1,&DAT_005d8b90);
    if (param_1 == 0x100) {
      DAT_007d4154 = 1;
    }
    goto LAB_004cb479;
  }
  if (iVar3 == 0) {
LAB_004cb381:
    uVar1 = (**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d4568,0x30411);
    (&DAT_007d4158)[param_1] = uVar1;
  }
  else {
    local_8 = 0x10;
    piVar2 = param_2;
    while (*(int *)((iVar3 - (int)param_2) + (int)piVar2) == *piVar2) {
      piVar2 = piVar2 + 1;
      local_8 = local_8 + -1;
      if (local_8 == 0) {
        return true;
      }
    }
    if (iVar3 == 0) goto LAB_004cb381;
  }
  piVar2 = param_2;
  piVar6 = (int *)(&DAT_007d4158)[param_1];
  for (iVar3 = 0x10; iVar3 != 0; iVar3 = iVar3 + -1) {
    *piVar6 = *piVar2;
    piVar2 = piVar2 + 1;
    piVar6 = piVar6 + 1;
  }
  iVar3 = (**(code **)(*DAT_007d4110 + 0xb0))(DAT_007d4110,param_1,param_2);
  if (param_1 == 0x100) {
    DAT_007d4154 = 0;
    return -1 < iVar3;
  }
LAB_004cb479:
  return -1 < iVar3;
}
