// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ad0b0 (h3): 56.2% exact, regs 73.4%. Residual class (b) callbacks/arity typed (FUN_005ad320 3 args, FUN_005aea00 uint,uint), permuter reordered independent stores (56.2%); original reloads param_1 in the list search loop. */
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
typedef unsigned short ushort;
typedef unsigned char byte;
uint FUN_005ad320(uint p0, uint p1, int p2);
int FUN_005adf30(undefined4 p0, undefined4 p1);
uint FUN_005aea00(uint a, uint b);
// MATCH: 0x005ad0b0 FUN_005ad0b0
int * FUN_005ad0b0(int param_1,int param_2,int (*param_3)(uint, uint),uint param_4)

{
  int *piVar1;
  int *piVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  ushort uVar8;
  int iVar9;
  uint uStack_8;
  
  iVar4 = *(int *)(param_2 + 0x4c);
  piVar1 = (int *)(iVar4 + 4);
  for (piVar6 = *(int **)(iVar4 + 4); piVar6 != piVar1; piVar6 = (int *)*piVar6) {
    if (piVar6[-5] == param_1) {
      piVar6[-2] = piVar6[-2] + 1;
      return piVar6 + -5;
    }
  }
  iVar5 = FUN_005ad320(param_1,param_2,1);
  if (iVar5 != 0) {
    iVar5 = ((uint)*(ushort *)(param_1 + 0x16) + (uint)*(ushort *)(param_1 + 0x14)) * 8 + 0x1c;
    if (!(param_3 == 0)) {
      piVar6 = (int *)(*param_3)((uint)iVar5,param_4);
      if (piVar6 != (int *)0x0) {
        piVar6[4] = 1;
LAB_005ad165:
        piVar6[1] = (int)(piVar6 + 7);
        uVar3 = *(ushort *)(param_1 + 0x14);
        piVar6[2] = (int)(piVar6 + 7 + (uint)uVar3 * 2);
        *piVar6 = param_1;
        piVar6[3] = 0;
        uVar3 = *(ushort *)(*(int *)(param_2 + 0x4c) + 0xc);
        if (*(ushort *)(param_1 + 0x14) != 0) {
          iVar5 = 0;
          uStack_8 = (uint)*(ushort *)(param_1 + 0x14);
          param_4 = 0;
          do {
            uVar8 = 0;
            if (uVar3 != 0) {
              iVar9 = 0;
              do {
                iVar7 = FUN_005adf30(*(undefined4 *)(iVar9 + *(int *)(iVar4 + 0x10)),
                                     *(undefined4 *)(param_4 + *(int *)(param_1 + 0x10)));
                if (iVar7 == 0) {
                  *(ushort *)(piVar6[1] + 4 + iVar5) = uVar8;
                  *(undefined4 *)(piVar6[1] + iVar5) =
                       *(undefined4 *)(iVar9 + 0x14 + *(int *)(iVar4 + 0x10));
                }
                uVar8 = uVar8 + 1;
                iVar9 = iVar9 + 0x18;
              } while (uVar8 < uVar3);
            }
            iVar5 = iVar5 + 8;
            param_4 = param_4 + 0x14;
            uStack_8 = uStack_8 - 1;
          } while (uStack_8 != 0);
        }
        uVar3 = *(ushort *)(*(int *)(param_2 + 0x4c) + 0xe);
        iVar5 = 0;
        if (*(ushort *)(param_1 + 0x16) != 0) {
          param_4 = (uint)*(ushort *)(param_1 + 0x16);
          param_2 = 0;
          do {
            uVar8 = 0;
            if (uVar3 != 0) {
              iVar9 = 0;
              do {
                iVar7 = FUN_005adf30(*(undefined4 *)(iVar9 + *(int *)(iVar4 + 0x14)),
                                     *(undefined4 *)(*(int *)(param_1 + 0xc) + param_2));
                if (iVar7 == 0) {
                  *(ushort *)(piVar6[2] + 4 + iVar5) = uVar8;
                  *(undefined4 *)(piVar6[2] + iVar5) =
                       *(undefined4 *)(iVar9 + 0x14 + *(int *)(iVar4 + 0x14));
                }
                uVar8 = uVar8 + 1;
                iVar9 = iVar9 + 0x18;
              } while (uVar8 < uVar3);
            }
            iVar5 = iVar5 + 8;
            param_2 = param_2 + 0x14;
            param_4 = param_4 - 1;
          } while (param_4 != 0);
        }
        piVar2 = piVar6 + 5;
        piVar6[6] = (int)piVar1;
        *piVar2 = iVar4;
        iVar4 = *piVar1;
        *(int **)(*piVar1 + 4) = piVar2;
        *piVar1 = (int)piVar2;
        return piVar6;
      }
    } else {
      piVar6 = (int *)FUN_005aea00((uint)iVar5,0x3080a);
      if (piVar6 != (int *)0x0) {
        piVar6[4] = 0;
        goto LAB_005ad165;
      }
    }
  }
  return (int *)0x0;
}
