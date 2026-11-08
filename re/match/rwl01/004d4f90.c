// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004d4f90: best 28.3% (regs ~50%, ops 80%), class (b). Vararg-list walker (NULL-terminated stack args) with three separate error blocks; written in Ghidra shape with the null check as the outer if (error 0x16 as the final else), FUN_004d5190 returning int, error words as a 2-word array. The original keeps the object in ebp and the arg-list cursor in memory; ours allocates edi/esi differently. Not enough budget for a register-faithful rewrite. */
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
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int * FUN_004d3e90(int * p0, int * p1);
int FUN_004d5190(int * p0, int * p1, int p2);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004d4f90 FUN_004d4f90
int * FUN_004d4f90(uint *param_1,int *param_2,int param_3)

{
  uint uVar1;
  int *piVar2;
  int iVar3;
  int *puVar4;
  int *piVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  int *piVar9;
  undefined4 uVar10;
  int local_18;
  int *local_10;
  int local_8;
  undefined4 loc[2];
  
  piVar2 = (int *)param_1;
  if (param_1 != (uint *)0x0) {
  if (*param_1 == 0) {
    uVar10 = 0x34;
  }
  else {
    local_8 = 0;
    if (param_3 == 0) {
      return (int *)0x0;
    }
    puVar4 = &param_3;
    do {
      piVar6 = puVar4 + 1;
      puVar4 = puVar4 + 1;
      local_8 = local_8 + 1;
    } while (*piVar6 != 0);
    if (local_8 == 0) {
      return (int *)0x0;
    }
    iVar7 = param_1[1];
    if ((uint)(iVar7 + local_8) <= *(uint *)(DAT_00911ae0 + 0x38 + DAT_007d3ff8)) {
      local_10 = &param_3;
      iVar8 = iVar7 * 0x28;
      local_18 = 0;
      iVar3 = param_3;
      piVar6 = (int *)0x0;
      do {
        piVar5 = (int *)(piVar2[2] + iVar8);
        iVar3 = FUN_004d5190(piVar2,piVar5,iVar3);
        if (iVar3 == 0) break;
        if (piVar6 != (int *)0x0) {
          if ((piVar5 == (int *)0x0) || (piVar9 = piVar5, *piVar5 == 0)) {
            piVar9 = (int *)0x0;
          }
          if ((*piVar6 == 0) || (piVar6[1] == 0)) {
            param_1 = (uint *)0x0;
          }
          else {
            param_1 = (uint *)piVar6[2];
          }
          if (((((*piVar2 == 0) || (param_1 == (uint *)0x0)) || (*param_1 != 0xffffffff)) ||
              ((piVar9 == (int *)0x0 || (*piVar9 == 0)))) ||
             ((uVar1 = ((int)piVar9 - piVar2[2]) / 0x28, (int *)(piVar2[2] + uVar1 * 0x28) != piVar9
              || (((uint)piVar2[1] <= uVar1 || (uVar1 == 0xffffffff)))))) {
            FUN_004d3e90(piVar5,piVar2);
            break;
          }
          *param_1 = uVar1;
        }
        iVar8 = iVar8 + 0x28;
        local_18 = local_18 + 1;
        local_10 = local_10 + 1;
        iVar3 = *local_10;
        piVar6 = piVar5;
      } while (iVar3 != 0);
      if (local_18 == local_8) {
        if (param_2 != (int *)0x0) {
          *param_2 = iVar7;
        }
        return piVar2;
      }
      if (local_18 == 0) {
        return (int *)0x0;
      }
      iVar7 = (iVar7 + local_18) * 0x28;
      do {
        iVar7 = iVar7 + -0x28;
        FUN_004d3e90((int *)(piVar2[2] + iVar7),piVar2);
        local_18 = local_18 + -1;
      } while (local_18 != 0);
      return (int *)0x0;
    }
    uVar10 = 0x2a;
  }
  loc[0] = 1;
  loc[1] = FUN_004d7ff0(uVar10);
  FUN_004d8480(loc);
    return (int *)0x0;
  }
  loc[0] = 1;
  loc[1] = FUN_004d7ff0(0x80000016);
  FUN_004d8480(loc);
  return (int *)0x0;
}
