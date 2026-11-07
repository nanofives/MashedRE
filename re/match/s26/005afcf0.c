// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005afcf0: 31.4% (regs 60.8%, ops 76.1%), 515 vs 566 insns (1635 B). Budget stop: raw converter body made to compile under the DDK only (FUN_005b0a90 returns a pointer, FUN_005b0970/FUN_005af860 return pointers; the converter had them void -> C2069). Callee arities from Ghidra (e.g. FUN_005ad6a0 called with 1 and 5 args, FUN_005aa060 with 4, FUN_004522d0 with args) are wrong, so C++ fails with 60 errors (arity/pointer-vs-undefined4) and the deferred-add-esp pattern seen in 005af690 recurs. Not restructured. Class (b), ran out of budget. cxx: FAIL. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef long LONG;
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(undefined1, DAT_005e6ce0, 0x005e6ce0);
MGLOBAL(undefined1, DAT_005e6cf0, 0x005e6cf0);
MGLOBAL(undefined1, DAT_005e6d60, 0x005e6d60);
MGLOBAL(undefined1, DAT_005e6d90, 0x005e6d90);
MGLOBAL(undefined1, DAT_005e6de0, 0x005e6de0);
void FUN_004522d0(void);
int FUN_005a9e40(int p0, undefined4 p1);
bool FUN_005a9ff0(undefined4 p0, undefined4 p1, int p2);
undefined4 FUN_005aa060(undefined4 p0, undefined4 * p1, undefined4 * p2);
int * FUN_005aa560(int p0, undefined4 * p1, uint p2, undefined1 * p3, undefined * p4, undefined4 p5);
uint FUN_005ac650(uint * p0);
uint FUN_005ace70(undefined4 * p0);
undefined4 * FUN_005ad6a0(int p0);
void FUN_005ade90(int * p0);
undefined4 FUN_005aea00(void);
int * FUN_005af860(int * p0, int * * p1, int * * p2, int p3, int p4);
undefined4 FUN_005b0360(int p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
undefined4 FUN_005b04b0(int p0, undefined4 p1, int p2, int * * * * * * p3, int * * * * * * p4, undefined4 p5);
int * FUN_005b0970(int p0, int * * p1, int p2, int p3);
int * FUN_005b0a90(int * p0, undefined4 p1);
void FUN_005b0b10(int p0);
void FUN_005b0b40(undefined4 * p0);
undefined4 * FUN_005b0b60(undefined4 * p0, int p1);
void FUN_005b1030(int p0, undefined4 p1, int p2, undefined4 p3);
__declspec(dllimport) LONG __stdcall InterlockedIncrement(LONG * p0);
// MATCH: 0x005afcf0 FUN_005afcf0
undefined4 FUN_005afcf0(int param_1,uint *param_2)

{
  int *piVar1;
  undefined4 *puVar2;
  int **ppiVar3;
  int *piVar4;
  undefined2 uVar5;
  undefined4 uVar6;
  int ****ppppiVar7;
  int iVar8;
  int iVar9;
  int *****pppppiVar10;
  int *piVar11;
  int *piVar12;
  int **ppiVar13;
  int *piVar14;
  uint uVar15;
  int **ppiVar16;
  int **ppiVar17;
  uint uVar18;
  int ****local_8c;
  int ****local_88;
  uint local_84;
  undefined4 **local_80;
  undefined4 **local_7c;
  undefined4 **local_78;
  int ***local_74;
  undefined4 ****local_70;
  undefined4 ****local_6c;
  undefined4 local_68;
  int local_64;
  undefined4 uStack_60;
  uint uStack_5c;
  undefined4 uStack_58;
  undefined4 *local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined1 local_3c;
  undefined1 local_3b;
  undefined4 local_38;
  undefined4 local_34;
  undefined1 local_30;
  undefined1 local_2f;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined1 local_20;
  undefined1 local_1f;
  undefined4 local_1c;
  undefined4 local_18;
  undefined1 local_14;
  undefined1 local_13;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  uVar18 = 0;
  *(undefined4 *)(param_1 + 0x40) = 0;
  *(undefined4 *)(param_1 + 0x44) = 0x20;
  if (*(int *)(param_1 + 0xc) == 0) {
    return 0;
  }
  local_8c = (int ****)&local_8c;
  param_2[2] = *(uint *)(*(int *)(param_1 + 0xc) + 0x14);
  local_88 = (int ****)&local_8c;
  if (param_2[2] < 2) {
    local_10 = 0;
    local_4c = FUN_005ad6a0(&DAT_005e6cf0);
    local_54 = *(undefined4 **)(param_1 + 0x14);
    local_7c = &local_54;
    local_50 = *local_54;
    local_c = 0xffffffff;
    local_48 = 0;
    local_44 = 0;
    local_40 = 0;
    local_3c = 0;
    local_3b = 0;
    local_30 = 0;
    local_2f = 0;
    local_38 = 0;
    local_34 = 0;
    local_2c = 0;
    local_28 = 0;
    local_24 = 0;
    local_20 = 0;
    local_1f = 0;
    local_14 = 0;
    local_13 = 0;
    local_1c = 0;
    local_18 = 0;
    local_4 = 0;
    local_8 = 0;
  }
  else {
    uVar6 = FUN_005ad6a0(&DAT_005e6de0,0,0,0xffffffff,0);
    uVar6 = FUN_005aa060(0,&DAT_005e6ce0,0,uVar6);
    local_7c = (int **)FUN_005b0a90((int *)&local_8c,uVar6);
    if (local_7c == (int **)0x0) {
      return 0;
    }
  }
  ppppiVar7 = (int ****)FUN_005ad6a0(&DAT_005e6d90);
  local_74 = (int ***)ppppiVar7;
  iVar8 = FUN_005ad6a0(&DAT_005e6d60);
  local_70 = &local_70;
  local_6c = &local_70;
  local_68 = 0;
  local_64 = iVar8;
  iVar9 = FUN_005a9ff0(0,ppppiVar7,&local_70);
  if (iVar9 == 0) {
    return 0;
  }
  iVar9 = FUN_005b0360(param_1,ppppiVar7,local_7c,&local_8c,&local_70);
  if (iVar9 == 0) {
    FUN_005ade90(&local_70);
    return 0;
  }
  iVar8 = FUN_005b04b0(param_1,ppppiVar7,iVar8,local_7c,&local_8c,&local_70);
  if (iVar8 == 0) {
    FUN_005b0b40(&local_8c);
    FUN_005ade90(&local_70);
    return 0;
  }
  FUN_005ade90(&local_70);
  uVar15 = 0;
  local_78 = *(undefined4 ***)(*(int *)(param_1 + 0xc) + 0x18);
  ppiVar13 = (int **)0xffffffff;
  local_84 = 0;
  local_80 = (int **)0xffffffff;
  pppppiVar10 = (int *****)local_8c;
  if ((int *****)local_8c != &local_8c) {
    do {
      if (pppppiVar10[-0x11] == (int ****)local_74) {
        ppiVar16 = pppppiVar10[-0x12][0x15][3];
        if (ppiVar16 == (int **)0x0) {
          *param_2 = *param_2 | 1;
        }
        else {
          if (uVar18 < (uint)*local_78[(int)pppppiVar10[-1] * 10 + 5]) {
            uVar18 = *local_78[(int)pppppiVar10[-1] * 10 + 5];
          }
          ppiVar13 = local_80;
          if (ppiVar16 < local_80) {
            ppiVar13 = ppiVar16;
            local_80 = ppiVar16;
          }
        }
        if (uVar15 < (uint)*local_78[(int)pppppiVar10[-1] * 10 + 5]) {
          if (uVar15 != 0) {
            *param_2 = *param_2 | 3;
          }
          uVar15 = **(uint **)(*(int *)(*(int *)(param_1 + 0xc) + 0x18) + 0x14 +
                              (int)pppppiVar10[-1] * 0x28);
        }
      }
      if (local_84 < *(ushort *)(pppppiVar10[-0x12] + 0x11)) {
        local_84 = (uint)*(ushort *)(pppppiVar10[-0x12] + 0x11);
      }
      pppppiVar10 = (int *****)*pppppiVar10;
    } while (pppppiVar10 != &local_8c);
  }
  *(int ***)(param_1 + 0x44) = ppiVar13;
  *(uint *)(param_1 + 0x40) = uVar18;
  uVar18 = FUN_005aea00(local_84,0x1080e);
  ppiVar13 = local_7c;
  local_84 = uVar18;
  if (uVar18 == 0) {
    FUN_005b0b40(&local_8c);
    return 0;
  }
  ppiVar16 = local_78;
  if (local_7c != &local_54) {
    iVar8 = FUN_005a9e40(local_7c[1],uVar18);
    if (iVar8 == 0) {
      FUN_004522d0(uVar18);
      FUN_005b0b40(&local_8c);
      return 0;
    }
    InterlockedIncrement((LONG *)(param_1 + 4));
    *(undefined4 *)(uVar18 + 8) = *(undefined4 *)(param_1 + 0xc);
    *(undefined4 *)(uVar18 + 0xc) = *(undefined4 *)(param_1 + 0x10);
    *(int *)(uVar18 + 0x10) = param_1 + 0x28;
    piVar11 = (int *)FUN_005aa560(ppiVar13[1],0,4,uVar18,*(undefined4 *)(param_1 + 0x50),
                                  *(undefined4 *)(param_1 + 0x58));
    *ppiVar13 = piVar11;
    if (piVar11 == (int *)0x0) {
      FUN_004522d0(uVar18);
      FUN_005b0b40(&local_8c);
      return 0;
    }
    piVar1 = (int *)(param_1 + 0x18);
    piVar11[0xb] = *(int *)(param_1 + 0x18);
    (*ppiVar13)[0xc] = (int)piVar1;
    *(int **)(*piVar1 + 4) = *ppiVar13 + 0xb;
    *piVar1 = (int)(*ppiVar13 + 0xb);
    FUN_005b1030(*(undefined4 *)(param_1 + 0x14),0,*ppiVar13,0);
    ppiVar16 = local_78;
  }
  do {
    local_78 = ppiVar16;
    local_80 = (undefined4 **)FUN_005b0b60(&local_8c,local_74);
    ppiVar3 = local_80;
    if (local_80 == (int **)0x0) {
      for (puVar2 = *(undefined4 **)(param_1 + 0x20); puVar2 != (undefined4 *)(param_1 + 0x20);
          puVar2 = (undefined4 *)*puVar2) {
        puVar2[8] = 1;
      }
      FUN_004522d0(uVar18);
      FUN_005b0b40(&local_8c);
      return 1;
    }
LAB_005b0132:
    do {
      ppiVar16 = ppiVar3;
      ppiVar17 = local_78;
      if (*ppiVar16 == (int *)0x0) {
        ppiVar17 = (int **)ppiVar16[0x11];
        ppiVar3 = ppiVar17;
        local_78 = ppiVar16;
        if (ppiVar17 != (int **)0x0) goto LAB_005b0132;
      }
      iVar8 = FUN_005a9e40(ppiVar17[1],uVar18);
      if (iVar8 == 0) {
LAB_005b0333:
        FUN_004522d0(uVar18);
        FUN_005b0b40(&local_8c);
        return 0;
      }
      *(undefined4 *)(uVar18 + 8) = *(undefined4 *)(param_1 + 0xc);
      *(int *)(uVar18 + 0x10) = param_1 + 0x28;
      if ((int ***)ppiVar17[2] == local_74) {
        iVar8 = *(int *)(*(int *)(param_1 + 0xc) + 0x18) + (int)ppiVar17[0x12] * 0x28;
        if (ppiVar17[0x11][2] == local_64) {
          uStack_60 = *(undefined4 *)(iVar8 + 0x14);
          uStack_58 = *(undefined4 *)(iVar8 + 0xc);
          uStack_5c = (uint)*(ushort *)(iVar8 + 0x1a);
          *(int ***)(iVar8 + 0x14) = ppiVar17 + 3;
          uVar6 = FUN_005ace70(ppiVar17 + 3);
          *(undefined4 *)(iVar8 + 0xc) = uVar6;
          uVar5 = FUN_005ac650(*(undefined4 *)(iVar8 + 0x14));
          *(undefined2 *)(iVar8 + 0x1a) = uVar5;
        }
        piVar11 = (int *)FUN_005b0970(param_1,ppiVar17,iVar8,local_84);
        *ppiVar17 = piVar11;
        if (ppiVar17[0x11][2] == local_64) {
          *(undefined4 *)(iVar8 + 0x14) = uStack_60;
          *(undefined2 *)(iVar8 + 0x1a) = (undefined2)uStack_5c;
          *(undefined4 *)(iVar8 + 0xc) = uStack_58;
        }
        if (*ppiVar17 == (int *)0x0) {
          FUN_004522d0(local_84);
          FUN_005b0b40(&local_8c);
          return 0;
        }
        (*ppiVar17)[0x13] = (int)ppiVar17[0x12];
        piVar1 = *(int **)(param_1 + 0x20);
        piVar11 = (int *)(param_1 + 0x20);
        ppiVar13 = local_7c;
        uVar18 = local_84;
        if (piVar1 != piVar11) {
          piVar4 = piVar1;
          piVar14 = (int *)0x0;
          do {
            piVar12 = piVar4;
            if (ppiVar17[0x12] < (int *)piVar12[8]) break;
            piVar4 = (int *)*piVar12;
            piVar14 = piVar12;
          } while ((int *)*piVar12 != piVar11);
          if (piVar14 != (int *)0x0) {
            (*ppiVar17)[0xb] = *piVar14;
            (*ppiVar17)[0xc] = (int)piVar14;
            *(int **)(*piVar14 + 4) = *ppiVar17 + 0xb;
            *piVar14 = (int)(*ppiVar17 + 0xb);
            goto LAB_005b02bf;
          }
        }
        (*ppiVar17)[0xb] = (int)piVar1;
        (*ppiVar17)[0xc] = (int)piVar11;
        *(int **)(*piVar11 + 4) = *ppiVar17 + 0xb;
        *piVar11 = (int)(*ppiVar17 + 0xb);
      }
      else {
        piVar11 = (int *)FUN_005af860(ppiVar17[1],ppiVar17 + 3,ppiVar17 + 10,ppiVar17[0x12],param_1)
        ;
        *ppiVar17 = piVar11;
        if (piVar11 == (int *)0x0) goto LAB_005b0333;
        piVar1 = (int *)(param_1 + 0x18);
        piVar11[0xb] = *(int *)(param_1 + 0x18);
        (*ppiVar17)[0xc] = (int)piVar1;
        *(int **)(*piVar1 + 4) = *ppiVar17 + 0xb;
        *piVar1 = (int)(*ppiVar17 + 0xb);
      }
LAB_005b02bf:
      if ((int **)ppiVar17[0x11] == ppiVar13) {
        piVar11 = ppiVar17[0x12];
      }
      else {
        piVar11 = (int *)0x0;
      }
      FUN_005b1030((int *)*ppiVar17[0x11],piVar11,*ppiVar17,0);
      ppiVar3 = local_80;
      ppiVar16 = local_78;
    } while (ppiVar17 != local_80);
    while (local_78 = ppiVar3, local_78 != ppiVar13) {
      ppiVar3 = (int **)local_78[0x11];
      FUN_005b0b10(local_78);
      ppiVar16 = local_78;
    }
  } while( true );
}
