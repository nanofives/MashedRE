// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 57.1% (regs 64.1%, permuter best (240 s). Not classified beyond structure/register differences (large function, budget spent). Class: b. */
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
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(int, DAT_007ddcac, 0x007ddcac);
MGLOBAL(void *, PTR_PTR_00634550, 0x00634550);
MGLOBAL(undefined1, DAT_007ddb30, 0x007ddb30);
MGLOBAL(undefined1, DAT_007ddb4c, 0x007ddb4c);
MGLOBAL(undefined4, DAT_007ddb68, 0x007ddb68);
MGLOBAL(undefined4, DAT_007ddb6c, 0x007ddb6c);
undefined4 FUN_005ac5f0(int * p0, int * p1);
void FUN_005ac980(undefined4 * p0, undefined4 * p1, int p2);
// MATCH: 0x005b3760 FUN_005b3760
uint FUN_005b3760(int param_1,int param_2,int param_3,int param_4,int *param_5,int *param_6,
                 uint param_7,uint param_8,uint param_9)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  undefined **ppuVar5;
  undefined *puVar6;
  uint uVar7;
  int local_38 [7];
  int local_1c [7];
  
  uVar3 = param_8;
  if (param_8 != param_7) {
    if (param_8 == 0) {
      FUN_005ac980((undefined4 *)param_1,(undefined4 *)local_38,1);
      local_38[0] = param_2;
      FUN_005ac980((undefined4 *)param_3,(undefined4 *)local_1c,1);
      local_1c[0] = param_4;
      puVar6 = &DAT_007ddb30;
      uVar7 = 0;
      do {
        if (((*(int *)(puVar6 + 0x38) != 0) && (iVar2 = FUN_005ac5f0(local_38,(int *)puVar6), iVar2 != 0))
           && (iVar2 = FUN_005ac5f0(local_1c,(int *)(puVar6 + 0x1c)), iVar2 != 0)) {
          uVar3 = 0;
          if (*(int *)(puVar6 + 0x38) != 0) {
            piVar4 = (int *)(puVar6 + 0x3c);
            do {
              iVar2 = *piVar4;
              piVar4 = piVar4 + 1;
              param_5[uVar3] = iVar2;
              uVar3 = uVar3 + 1;
            } while (uVar3 < *(uint *)(puVar6 + 0x38));
          }
          return *(uint *)(puVar6 + 0x38);
        }
        uVar7 = uVar7 + 0x4c;
        puVar6 = puVar6 + 0x4c;
      } while (uVar7 < 0x17c);
      uVar7 = 0;
      do {
        piVar4 = (int *)(&PTR_PTR_00634550)[uVar7 * 2];
        if (((param_1 == *piVar4) && (param_3 == piVar4[1])) &&
           ((param_2 == param_4 || ((*(byte *)(piVar4 + 2) & 1) != 0)))) {
          *param_5 = (int)(&PTR_PTR_00634550 + uVar7 * 2);
          return 1;
        }
        uVar7 = uVar7 + 1;
      } while (uVar7 < 0x33);
    }
    param_8 = 0;
    ppuVar5 = (undefined **)(&PTR_PTR_00634550);
    do {
      piVar4 = (int *)*ppuVar5;
      if (param_1 == *piVar4) {
        uVar7 = 0;
        if (uVar3 != 0) {
          do {
            if ((undefined **)param_6[uVar7] == ppuVar5) goto LAB_005b390b;
            uVar7 = uVar7 + 1;
          } while (uVar7 < uVar3);
        }
        if ((param_3 == piVar4[1]) && ((param_2 == param_4 || ((*(byte *)(piVar4 + 2) & 1) != 0))))
        {
          param_6[uVar3] = (int)(&PTR_PTR_00634550 + param_8 * 2);
          for (uVar7 = uVar3 * 4 + 4 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
            *param_5 = *param_6;
            param_6 = param_6 + 1;
            param_5 = param_5 + 1;
          }
          for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
            *(char *)param_5 = (char)*param_6;
            param_6 = (int *)((int)param_6 + 1);
            param_5 = (int *)((int)param_5 + 1);
          }
          if (uVar3 == 0) {
            return 1;
          }
          return uVar3;
        }
        param_6[uVar3] = (int)ppuVar5;
        if (uVar3 == param_9) {
          return 0;
        }
        iVar2 = param_2;
        if (((*ppuVar5)[8] & 1) != 0) {
          iVar2 = param_4;
        }
        uVar7 = FUN_005b3760(*(undefined4 *)(*ppuVar5 + 4),iVar2,param_3,param_4,param_5,param_6,
                             param_7,uVar3 + 1,param_9);
        if (uVar7 != 0) {
          param_9 = uVar7;
        }
      }
LAB_005b390b:
      iVar2 = DAT_007ddcac;
      ppuVar5 = ppuVar5 + 2;
      param_8 = param_8 + 1;
    } while (param_8 < 0x33);
    if (param_9 != 0xffffffff) {
      if (uVar3 == 0) {
        param_9 = param_9 + 1;
        iVar1 = DAT_007ddcac * 0x4c;
        FUN_005ac980((undefined4 *)param_1,(undefined4 *)(&DAT_007ddb30 + iVar1),1);
        *(int *)(&iVar1 + DAT_007ddb4c) = param_4;
        FUN_005ac980((undefined4 *)param_3,(undefined4 *)(&DAT_007ddb4c + iVar1),1);
        (&DAT_007ddb68)[iVar2 * 0x13] = param_9;
        *(int *)(&DAT_007ddb30 + iVar1) = param_2;
        if (param_9 != 0) {
          uVar3 = 0;
          piVar4 = (int *)(&DAT_007ddb6c + iVar2 * 0x13);
          do {
            *piVar4 = param_5[uVar3];
            uVar7 = uVar3 + 1;
            uVar3 = uVar7;
            piVar4 = piVar4 + 1;
          } while (uVar7 < param_9);
        }
        DAT_007ddcac = DAT_007ddcac + 1;
        if (DAT_007ddcac == 5) {
          DAT_007ddcac = 0;
        }
      }
      return param_9;
    }
  }
  return 0;
}
