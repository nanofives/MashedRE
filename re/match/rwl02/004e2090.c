// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e2090: best 29.7% (regs 48.3%), residual class b. structure only (899 B), raw converter form, varargs FUN_004d7ff0, callee protos fixed; 29.7. */
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
#ifndef __cplusplus
#define true 1
#endif
int FUN_004d4310(int p0);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
undefined4 * FUN_004d8480(void * p0);
undefined4 * FUN_004e2420(int * p0, int p1);
int FUN_004e2470(int p0, int p1, int p2, int p3, int p4);
uint FUN_004e2500(uint * p0, int p1);
void FUN_004e2530(int p0, int p1, uint p2);
undefined4 FUN_004e2650(int p0);
undefined4 FUN_004e2930(int p0, int * p1);
undefined4 FUN_004e2b60(int p0, int * p1);
undefined4 FUN_004e2bf0(int p0, int p1);
// MATCH: 0x004e2090 FUN_004e2090
int FUN_004e2090(int param_1)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int uVar5;
  int iVar16;
  uint *puVar6;
  int iVar7;
  int iVar8;
  int *piVar9;
  int *piVar10;
  undefined4 *puVar11;
  int *piVar12;
  int *piVar13;
  undefined4 *puVar14;
  int iVar15;
  int local_18;
  int local_14;
  uint local_10;
  int local_c;
  int local_8;
  undefined4 local_4;
  
  local_8 = *(int *)(param_1 + 4);
  local_c = 0;
  piVar10 = (int *)(*(int *)(param_1 + 8) + -0x28 + local_8 * 0x28);
  piVar12 = piVar10 + 7;
  do {
    iVar8 = *piVar10;
    uVar3 = 0;
    puVar1 = (uint *)(iVar8 + 0x20);
    uVar2 = *puVar1;
    if (uVar2 != 0) {
      piVar13 = *(int **)(iVar8 + 0x24);
      do {
        if ((undefined4 *)*piVar13 == (undefined4 *)0x0) {
          local_8 = 1;
          local_4 = FUN_004d7ff0(0x1f,*(undefined4 *)*piVar10,uVar3);
          FUN_004d8480(&local_8);
          return 0x1f;
        }
        uVar3 = uVar3 + 1;
        piVar9 = piVar13;
        for (uVar4 = uVar3; uVar4 < uVar2; uVar4 = uVar4 + 1) {
          piVar9 = piVar9 + 3;
          if ((undefined4 *)*piVar13 == (undefined4 *)*piVar9) {
            local_8 = 1;
            local_4 = FUN_004d7ff0(0x1e,*(undefined4 *)*piVar10,*(undefined4 *)*piVar9);
            FUN_004d8480(&local_8);
            return 0x1e;
          }
        }
        piVar13 = piVar13 + 3;
      } while (uVar3 < uVar2);
    }
    iVar16 = FUN_004d4310(param_1);
    uVar5 = (int)FUN_004e2420(piVar10,iVar16);
    *(int *)(*piVar12 + 8) = uVar5;
    if (*(int *)(*piVar12 + 8) == 0) {
      local_8 = 1;
      local_4 = FUN_004d7ff0(0x20);
      FUN_004d8480(&local_8);
      return 0x20;
    }
    local_10 = 0;
    if (piVar12[-6] != 0) {
      local_14 = 0;
      do {
        iVar15 = *(int *)(piVar12[-5] + local_10 * 4);
        if (iVar15 != -1) {
          iVar15 = *(int *)(param_1 + 8) + iVar15 * 0x28;
          puVar11 = (undefined4 *)(*(int *)(*piVar10 + 0x30) + local_14);
          uVar3 = 0;
          puVar6 = *(uint **)(*(int *)(iVar15 + 0x1c) + 8);
          if (*puVar6 != 0) {
            local_18 = 0;
            if (*puVar6 == 0) {
              puVar14 = (undefined4 *)0x0;
              goto LAB_004e219f;
            }
            do {
              puVar14 = (undefined4 *)(puVar6[3] + local_18);
LAB_004e219f:
              iVar7 = FUN_004e2500(puVar1,*puVar14);
              if (iVar7 == -1) {
                iVar7 = puVar11[2];
              }
              else {
                iVar7 = *(int *)(puVar11[1] + iVar7 * 4);
              }
              if (iVar7 == 0) {
                iVar7 = FUN_004e2470(((int *)*piVar12)[2],*puVar14,puVar14[1],
                                     *(int *)*piVar12,puVar14[8]);
                if (iVar7 == 0) goto LAB_004e21eb;
              }
              else if (puVar14[1] == 1) {
                if (iVar7 != 1) {
                  local_8 = 1;
                  local_4 = FUN_004d7ff0(0x1d,*(undefined4 *)*puVar14,**(undefined4 **)puVar14[8],
                                         *(undefined4 *)*piVar10,local_10,*puVar11);
                  FUN_004d8480(&local_8);
                  return 0x1d;
                }
              }
              else if (iVar7 == 2) {
                FUN_004e2530(param_1,iVar15,*puVar14);
              }
              uVar3 = uVar3 + 1;
              local_18 = local_18 + 0x24;
              puVar6 = *(uint **)(*(int *)(iVar15 + 0x1c) + 8);
            } while (uVar3 < *puVar6);
          }
        }
        local_10 = local_10 + 1;
        local_14 = local_14 + 0xc;
      } while (local_10 < (uint)piVar12[-6]);
    }
    uVar3 = 0;
    if (*puVar1 != 0) {
      iVar15 = 0;
      do {
        iVar7 = *(int *)(*(int *)(iVar8 + 0x28) + uVar3 * 4);
        if ((iVar7 != 0) &&
           (iVar7 = FUN_004e2470(((int *)*piVar12)[2],
                                 *(int *)(*(int *)(iVar8 + 0x24) + iVar15),iVar7,
                                 *(int *)*piVar12,(int)piVar10), iVar7 == 0)) {
LAB_004e21eb:
          local_8 = 1;
          local_4 = FUN_004d7ff0(0x20);
          FUN_004d8480(&local_8);
          return 0x20;
        }
        uVar3 = uVar3 + 1;
        iVar15 = iVar15 + 0xc;
      } while (uVar3 < *puVar1);
    }
    piVar10 = piVar10 + -10;
    piVar12 = piVar12 + -10;
    local_8 = local_8 + -1;
    if (local_8 == 0) {
      iVar8 = FUN_004e2650(param_1);
      if (((iVar8 == 0) && (iVar8 = FUN_004e2930(param_1,&local_c), iVar8 == 0)) &&
         (iVar8 = FUN_004e2b60(param_1,&local_c), iVar8 == 0)) {
        iVar8 = FUN_004e2bf0(param_1,local_c);
      }
      return iVar8;
    }
  } while( true );
}
