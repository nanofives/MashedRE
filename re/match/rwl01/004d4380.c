// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004d4380: 17.2% regs 28.5%.  class (b) Raw Ghidra form only (budget); ops 72.8% but frame and register allocation differ (original sub esp,0x14 + push ebx).. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
MGLOBAL(int *, DAT_007d4718, 0x007d4718);
MGLOBAL(int *, DAT_007d4714, 0x007d4714);
undefined4 FUN_004d4910(int p0, undefined4 p1);
undefined4 FUN_004d4a20(int p0, int p1);
void FUN_004d4bf0(int * p0, int p1);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
int FUN_004e2090(int p0);
// MATCH: 0x004d4380 FUN_004d4380
int * FUN_004d4380(int *param_1)

{
  int *piVar1;
  int iVar2;
  code *pcVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  int *piVar7;
  int iVar8;
  int iVar9;
  uint *puVar10;
  uint uVar11;
  undefined4 *puVar12;
  int iVar13;
  int *piVar14;
  int *piVar15;
  uint uVar16;
  undefined4 uVar17;
  int *local_14;
  undefined4 local_10;
  int local_c;
  int *local_8;
  undefined4 local_4;
  
  piVar4 = param_1;
  if (param_1 == (int *)0x0) {
    local_8 = (int *)0x1;
    local_4 = FUN_004d7ff0(0x80000016);
    FUN_004d8480((undefined4 *)&local_8);
    return (int *)0x0;
  }
  if (*param_1 == 0) {
    local_8 = (int *)0x1;
    local_4 = FUN_004d7ff0(0x34);
    FUN_004d8480((undefined4 *)&local_8);
  }
  else {
    piVar7 = (int *)param_1[1];
    if (piVar7 == (int *)0x0) {
LAB_004d47f2:
      *piVar4 = 0;
      return piVar4;
    }
    local_c = 0;
    if ((piVar7 <= (int *)param_1[10]) || (*(int *)(param_1[2] + param_1[10] * 0x28) == 0)) {
      local_8 = (int *)0x1;
      local_4 = FUN_004d7ff0(0x24);
      FUN_004d8480((undefined4 *)&local_8);
      return (int *)0x0;
    }
    param_1 = (int *)0x0;
    local_8 = (int *)0x0;
    while (piVar15 = (int *)0xffffffff, piVar7 != (int *)0x0) {
      piVar14 = (int *)piVar4[2];
      local_14 = piVar7;
      do {
        iVar13 = *(int *)(*piVar14 + 0x20);
        if (iVar13 != 0) {
          puVar10 = *(uint **)(*piVar14 + 0x24);
          do {
            piVar1 = (int *)*puVar10;
            if ((local_8 < piVar1) && (piVar1 < piVar15)) {
              piVar15 = piVar1;
            }
            puVar10 = puVar10 + 3;
            iVar13 = iVar13 + -1;
          } while (iVar13 != 0);
        }
        piVar14 = piVar14 + 10;
        local_14 = (int *)((int)local_14 - 1);
      } while (local_14 != (int *)0x0);
      local_14 = (int *)0x0;
      if (piVar15 == (int *)0xffffffff) break;
      param_1 = (int *)((int)param_1 + 1);
      local_8 = piVar15;
    }
    iVar8 = *(int *)(DAT_00911ae0 + 0x38 + DAT_007d3ff8);
    uVar16 = iVar8 * 0xb4;
    iVar13 = (int)piVar7 * 0x28;
    if (piVar7 != (int *)0x0) {
      iVar13 = iVar13 + *(int *)(piVar4[2] + 4) * (int)piVar7 * 4;
    }
    iVar5 = (((int)piVar7 + 5U) * (int)param_1 + ((int)param_1 + 1) * (int)piVar7) * 4;
    if (piVar7 != (int *)0x0) {
      piVar15 = (int *)piVar4[2];
      local_14 = piVar7;
      do {
        iVar9 = *piVar15;
        iVar2 = *(int *)(iVar9 + 0x34);
        if (iVar2 != 0) {
          iVar5 = iVar5 + iVar2;
        }
        piVar15 = piVar15 + 10;
        iVar5 = iVar5 + *(int *)(iVar9 + 0x20) * 4;
        local_14 = (int *)((int)local_14 - 1);
      } while (local_14 != (int *)0x0);
    }
    uVar11 = iVar5 + (int)param_1 * 0x1c + 0x14 +
             iVar8 * 0xc + ((int)param_1 * 0x34 + 0x14) * (int)piVar7 + iVar13;
    if (uVar16 < uVar11) {
      uVar16 = uVar11;
    }
    if ((uVar16 <= (uint)piVar4[9]) || (iVar13 = FUN_004d4910((int)piVar4,uVar16), iVar13 != 0)) {
      DAT_007d4714 = (int *)(piVar4[8] + uVar16);
      DAT_007d4718 = (int *)0x0;
      piVar7 = (int *)piVar4[2];
      uVar11 = 0;
      local_14 = piVar4;
      local_10 = 0;
      if (piVar4[1] != 0) {
        do {
          if (*piVar7 != 0) {
            *(undefined4 *)(piVar7[7] + 4) = 0;
            *(undefined4 *)piVar7[7] = 0;
          }
          piVar7 = piVar7 + 10;
          uVar11 = uVar11 + 1;
        } while (uVar11 < (uint)piVar4[1]);
      }
      piVar7 = (int *)piVar4[2];
      uVar11 = 0;
      if (piVar4[1] != 0) {
        do {
          if ((*piVar7 != 0) && (iVar13 = piVar7[1], iVar13 != 0)) {
            piVar15 = (int *)piVar7[2];
            do {
              if (*piVar15 != -1) {
                piVar14 = *(int **)(piVar4[2] + 0x1c + *piVar15 * 0x28);
                *piVar14 = *piVar14 + 1;
              }
              piVar15 = piVar15 + 1;
              iVar13 = iVar13 + -1;
            } while (iVar13 != 0);
          }
          piVar7 = piVar7 + 10;
          uVar11 = uVar11 + 1;
        } while (uVar11 < (uint)piVar4[1]);
      }
      uVar11 = piVar4[10];
      if (**(int **)(piVar4[2] + 0x1c + uVar11 * 0x28) != 0) {
        uVar17 = 0x24;
LAB_004d45ea:
        local_8 = (int *)0x1;
        local_4 = FUN_004d7ff0(uVar17);
        FUN_004d8480((undefined4 *)&local_8);
        return (int *)0x0;
      }
      uVar6 = 0;
      if (piVar4[1] != 0) {
        puVar12 = (undefined4 *)(piVar4[2] + 0x1c);
        do {
          if ((uVar6 != uVar11) && (*(int *)*puVar12 == 0)) {
            local_8 = (int *)0x1;
            local_4 = FUN_004d7ff0(0x22);
            FUN_004d8480((undefined4 *)&local_8);
            return (int *)0x0;
          }
          uVar6 = uVar6 + 1;
          puVar12 = puVar12 + 10;
        } while (uVar6 < (uint)piVar4[1]);
      }
      FUN_004d4bf0((int *)&local_14,uVar11);
      uVar11 = 0;
      if (piVar4[1] != 0) {
        piVar7 = (int *)(piVar4[2] + 0x1c);
        do {
          if (*(int *)*piVar7 != ((int *)*piVar7)[1]) {
            uVar17 = 0x1c;
            goto LAB_004d45ea;
          }
          uVar11 = uVar11 + 1;
          piVar7 = piVar7 + 10;
        } while (uVar11 < (uint)piVar4[1]);
      }
      piVar4[10] = 0;
      iVar13 = *(int *)(DAT_00911ae0 + 0x38 + DAT_007d3ff8);
      local_14 = (int *)(piVar4[2] + iVar13 * 0x28);
      param_1 = (int *)piVar4[1];
      DAT_007d4714 = local_14 + iVar13 * 0x20 + (int)param_1 * 3 + -3;
      piVar7 = (int *)(piVar4[8] + -0xc + uVar16);
      if (-1 < (int)param_1 + -1) {
        iVar13 = ((int)param_1 + -1) * 0x28;
        piVar15 = DAT_007d4714;
        do {
          DAT_007d4714 = piVar15 + -3;
          *piVar7 = *piVar15;
          piVar7[1] = piVar15[1];
          piVar7[2] = piVar15[2];
          *(int **)(iVar13 + 0x1c + piVar4[2]) = piVar7;
          piVar7 = piVar7 + -3;
          param_1 = (int *)((int)param_1 + -1);
          iVar13 = iVar13 + -0x28;
          piVar15 = DAT_007d4714;
        } while (param_1 != (int *)0x0);
      }
      iVar13 = piVar4[2];
      param_1 = (int *)0x0;
      piVar7 = (int *)(iVar13 + piVar4[1] * 0x28);
      if (piVar4[1] != 0) {
        iVar8 = 0;
        do {
          iVar5 = *(int *)(iVar13 + 4 + iVar8);
          piVar15 = local_14;
          piVar14 = piVar7;
          if (iVar5 == 0) {
            *(undefined4 *)(iVar13 + 8 + iVar8) = 0;
          }
          else {
            for (; iVar5 != 0; iVar5 = iVar5 + -1) {
              *piVar14 = *piVar15;
              piVar15 = piVar15 + 1;
              piVar14 = piVar14 + 1;
            }
            *(int **)(iVar8 + 8 + piVar4[2]) = piVar7;
          }
          iVar13 = piVar4[2];
          local_14 = local_14 + 0x20;
          iVar5 = *(int *)(iVar13 + 4 + iVar8);
          local_c = local_c + iVar5;
          iVar8 = iVar8 + 0x28;
          piVar7 = piVar7 + iVar5;
          param_1 = (int *)((int)param_1 + 1);
        } while ((int *)param_1 < (int *)piVar4[1]);
      }
      DAT_007d4718 = local_14 + local_c;
      local_8 = DAT_007d4714;
      iVar13 = FUN_004e2090((int)piVar4);
      if (iVar13 == 0) {
        iVar13 = FUN_004d4910((int)piVar4,(int)DAT_007d4718 - piVar4[8]);
        if (iVar13 == 0) {
LAB_004d486f:
          FUN_004d4a20((int)piVar4,(int)piVar4);
          return (int *)0x0;
        }
        uVar16 = 0;
        if (piVar4[1] != 0) {
          iVar13 = 0;
          do {
            uVar16 = uVar16 + 1;
            *(undefined4 *)(iVar13 + 0x1c + piVar4[2]) = 0;
            iVar13 = iVar13 + 0x28;
          } while (uVar16 < (uint)piVar4[1]);
        }
        param_1 = (int *)(piVar4[1] + -1);
        if (-1 < (int)param_1) {
          iVar13 = (int)param_1 * 0x28;
          do {
            iVar8 = piVar4[2];
            iVar5 = *(int *)(iVar13 + iVar8);
            iVar9 = *(int *)(iVar5 + 0x3c);
            *(int *)(iVar5 + 0x3c) = iVar9 + 1;
            if (((iVar9 == 0) && (*(code **)(iVar5 + 8) != (code *)0x0)) &&
               (iVar9 = (**(code **)(iVar5 + 8))(iVar5), iVar9 == 0)) {
LAB_004d4818:
              uVar16 = piVar4[1];
              uVar11 = (uVar16 - (int)param_1) - 1;
              goto LAB_004d482b;
            }
            if ((*(code **)(iVar5 + 0x10) != (code *)0x0) &&
               (iVar8 = (**(code **)(iVar5 + 0x10))(iVar13 + iVar8), iVar8 == 0)) {
              iVar13 = *(int *)(iVar5 + 0x3c) + -1;
              *(int *)(iVar5 + 0x3c) = iVar13;
              if ((iVar13 == 0) && (*(code **)(iVar5 + 0xc) != (code *)0x0)) {
                (**(code **)(iVar5 + 0xc))(iVar5);
              }
              goto LAB_004d4818;
            }
            iVar13 = iVar13 + -0x28;
            param_1 = (int *)((int)param_1 + -1);
          } while (-1 < (int)param_1);
        }
        iVar13 = piVar4[1] + -1;
        if (-1 < iVar13) {
          iVar8 = iVar13 * 0x28;
          do {
            pcVar3 = *(code **)(*(int *)(iVar8 + piVar4[2]) + 0x18);
            if ((pcVar3 != (code *)0x0) && (iVar5 = (*pcVar3)(iVar8 + piVar4[2],piVar4), iVar5 == 0)
               ) {
              uVar11 = piVar4[1];
              uVar16 = uVar11;
LAB_004d482b:
              uVar11 = uVar16 - uVar11;
              if (uVar11 < uVar16) {
                iVar13 = uVar11 * 0x28;
                do {
                  iVar8 = *(int *)(piVar4[2] + iVar13);
                  if (*(code **)(iVar8 + 0x14) != (code *)0x0) {
                    (**(code **)(iVar8 + 0x14))((int *)(piVar4[2] + iVar13));
                  }
                  iVar5 = *(int *)(iVar8 + 0x3c) + -1;
                  *(int *)(iVar8 + 0x3c) = iVar5;
                  if ((iVar5 == 0) && (*(code **)(iVar8 + 0xc) != (code *)0x0)) {
                    (**(code **)(iVar8 + 0xc))(iVar8);
                  }
                  uVar11 = uVar11 + 1;
                  iVar13 = iVar13 + 0x28;
                } while (uVar11 < (uint)piVar4[1]);
              }
              goto LAB_004d486f;
            }
            iVar13 = iVar13 + -1;
            iVar8 = iVar8 + -0x28;
          } while (-1 < iVar13);
        }
        goto LAB_004d47f2;
      }
    }
  }
  return (int *)0x0;
}
