// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e2530: hand-written, Ghidra structure (params reused as index/offset, iVar1 = table pointer). 22.5%, structure only. Residual (b): original keeps one 4-byte local (table pointer) and param_2/param_3 homes as offset/index with ebx=param_2; ours spills differently. Permuter 344 compiles best 26.1% (not kept). */
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
// MATCH: 0x004e2530 FUN_004e2530
void FUN_004e2530(int param_1, int *param_2, unsigned int param_3)
{
  int *iVar1;
  unsigned int uVar2;
  int iVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  unsigned int uVar7;
  int iVar8;
  int *piVar9;
  int *piVar10;

  iVar6 = param_3;
  piVar5 = param_2;
  iVar1 = (int *)(*param_2 + 0x20);
  uVar2 = **(unsigned int **)(param_2[7] + 8);
  if (uVar2 != 0) {
    piVar10 = (int *)(*(unsigned int **)(param_2[7] + 8))[3];
    uVar7 = 0;
    if (uVar2 != 0) {
      while (*piVar10 != param_3) {
        piVar10 = piVar10 + 9;
        uVar7 = uVar7 + 1;
        if (uVar2 <= uVar7) {
          return;
        }
      }
      iVar3 = piVar10[2];
      piVar10[2] = iVar3 + -1;
      if (iVar3 + -1 == 0) {
        piVar4 = *(int **)(param_2[7] + 8);
        iVar3 = piVar4[3] + *piVar4 * 0x24;
        if (piVar10 != (int *)(iVar3 + -0x24)) {
          piVar9 = (int *)(iVar3 + -0x24);
          for (iVar8 = 9; iVar8 != 0; iVar8 = iVar8 + -1) {
            *piVar10 = *piVar9;
            piVar9 = piVar9 + 1;
            piVar10 = piVar10 + 1;
          }
        }
        *piVar4 = *piVar4 + -1;
        param_3 = 0;
        if (param_2[1] != 0) {
          param_2 = (int *)0;
          do {
            iVar3 = *(int *)(piVar5[2] + param_3 * 4);
            if (iVar3 != -1) {
              uVar2 = iVar1[0];
              iVar8 = *(int *)(*piVar5 + 0x30) + (int)param_2;
              uVar7 = 0;
              if (uVar2 == 0) {
LAB_004e25f9:
                iVar8 = *(int *)(iVar8 + 8);
              }
              else {
                piVar10 = (int *)iVar1[1];
                do {
                  if (*piVar10 == iVar6) {
                    if (uVar7 == 0xffffffff) goto LAB_004e25f9;
                    iVar8 = *(int *)(*(int *)(iVar8 + 4) + uVar7 * 4);
                    goto LAB_004e2604;
                  }
                  uVar7 = uVar7 + 1;
                  piVar10 = piVar10 + 3;
                } while (uVar7 < uVar2);
                iVar8 = *(int *)(iVar8 + 8);
              }
LAB_004e2604:
              if (iVar8 == 0) {
                FUN_004e2530(param_1, (int *)(*(int *)(param_1 + 8) + iVar3 * 0x28), iVar6);
              }
            }
            param_3 = param_3 + 1;
            param_2 = (int *)((int)param_2 + 0xc);
          } while (param_3 < (unsigned int)piVar5[1]);
        }
      }
    }
  }
}


