// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x004f1fb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00912118, 0x00912118);
MGLOBAL(undefined1, FUN_004f2420, 0x004f2420);
undefined4 FUN_004cc7f0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 FUN_004cc9f0(int p0);
void FUN_004f0970(undefined4 p0);
undefined4 FUN_004f2570(int p0, uint p1, int * p2, int p3);
undefined4 FUN_004f3440(int * p0, int p1);
void FUN_005c24e0(undefined1 * p0, uint p1, uint p2, undefined * p3);
// MATCH: 0x004f1fb0 FUN_004f1fb0
undefined4 * FUN_004f1fb0(int *param_1,undefined4 *param_2,uint param_3)

{
  undefined2 uVar1;
  int iVar2;
  int *piVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  uint *puVar6;
  undefined4 uVar7;
  undefined4 *puVar8;
  uint uVar9;
  int *piVar10;
  ushort uVar11;
  int iVar12;
  uint uVar13;
  int iVar14;
  uint uVar15;
  int iVar16;
  uint *puVar17;
  undefined4 *puVar18;
  int *piVar19;
  undefined4 *puVar20;
  undefined4 *puVar21;
  uint uStack_18;
  int *piStack_14;
  
  iVar14 = (int)param_1;
  uVar11 = 0;
  piVar3 = (int *)(**(code **)(DAT_007d3ff8 + 0x108))(*(int *)((int)param_1 + 4) << 2,0x10502);
  puVar4 = (undefined4 *)0x0;
  if (piVar3 != (int *)0x0) {
    if (*(int *)(iVar14 + 4) != 0) {
      iVar12 = 0;
      do {
        iVar16 = *(int *)(iVar14 + 8) + iVar12;
        iVar12 = iVar12 + 0x14;
        piVar3[(int)puVar4] = iVar16;
        puVar4 = (undefined4 *)((int)puVar4 + 1);
      } while (puVar4 < *(undefined4 **)(iVar14 + 4));
    }
    FUN_005c24e0(piVar3,*(undefined4 *)(iVar14 + 4),4,FUN_004f2420);
    uVar9 = *(uint *)(iVar14 + 4);
    iVar12 = 1;
    if (1 < uVar9) {
      iVar16 = *(int *)(*piVar3 + 8);
      uVar13 = 1;
      if (1 < uVar9) {
        do {
          if (*(int *)(piVar3[uVar13] + 8) != iVar16) {
            iVar12 = iVar12 + 1;
            iVar16 = *(int *)(piVar3[uVar13] + 8);
          }
          uVar13 = uVar13 + 1;
        } while (uVar13 < uVar9);
      }
    }
    iVar16 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar12 * 4,0x1010502);
    puVar5 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(iVar12 * 0xc,0x10502);
    puVar17 = puVar5 + 1;
    puVar5[2] = *(undefined4 *)(*piVar3 + 8);
    *puVar5 = 0;
    *puVar17 = 0;
    iVar12 = 1;
    if ((1 < *(uint *)(iVar14 + 4)) &&
       (uStack_18 = 0, puVar6 = puVar17, piVar19 = piVar3, *(uint *)(iVar14 + 4) != 1)) {
      do {
        uVar9 = *(uint *)(piVar19[1] + 8);
        if (*(uint *)(*piVar19 + 8) != uVar9) {
          puVar6[4] = uVar9;
          puVar6[2] = 0;
          puVar6[3] = uStack_18 + 1;
          iVar12 = iVar12 + 1;
          *puVar6 = (uStack_18 - *puVar6) + 1;
          puVar6 = puVar6 + 3;
        }
        uStack_18 = uStack_18 + 1;
        piVar19 = piVar19 + 1;
      } while (uStack_18 < *(int *)(iVar14 + 4) - 1U);
    }
    puVar5[iVar12 * 3 + -2] = *(int *)(iVar14 + 4) - puVar5[iVar12 * 3 + -2];
    uVar7 = FUN_004cc7f0(0x10,*(uint *)(iVar14 + 4) / 10 + 5,4,0x10502);
    *(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8) = uVar7;
    param_1 = (int *)0x0;
    piStack_14 = piVar3;
    for (; iVar12 != 0; iVar12 = iVar12 + -1) {
      puVar8 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(*puVar17 * 0x14,0x10502);
      uVar9 = 0;
      puVar4 = puVar8;
      if (*puVar17 != 0) {
        do {
          puVar18 = (undefined4 *)*piStack_14;
          puVar20 = puVar4;
          for (iVar14 = 5; iVar14 != 0; iVar14 = iVar14 + -1) {
            *puVar20 = *puVar18;
            puVar18 = puVar18 + 1;
            puVar20 = puVar20 + 1;
          }
          piStack_14 = piStack_14 + 1;
          uVar9 = uVar9 + 1;
          puVar4 = puVar4 + 5;
        } while (uVar9 < *puVar17);
      }
      FUN_004f2570(puVar8,*puVar17,&param_1,param_2);
      FUN_004f3440(&param_1,param_3);
      piVar19 = param_1;
      if (param_1 != (int *)0x0) {
        do {
          piVar10 = (int *)(**(code **)(DAT_007d3ff8 + 0x108))(piVar19[1] * 2 + 0xc,0x10502);
          piVar10[2] = puVar17[1];
          iVar14 = piVar19[1];
          piVar10[1] = iVar14;
          *piVar10 = (int)(piVar10 + 3);
          uVar9 = 0;
          if (iVar14 != 0) {
            do {
              *(undefined2 *)(*piVar10 + uVar9 * 2) = *(undefined2 *)(*piVar19 + uVar9 * 2);
              uVar9 = uVar9 + 1;
            } while (uVar9 < (uint)piVar10[1]);
          }
          uVar9 = (uint)uVar11;
          uVar11 = uVar11 + 1;
          *(int **)(iVar16 + uVar9 * 4) = piVar10;
          piVar19 = (int *)piVar19[3];
          piVar10 = param_1;
        } while (piVar19 != (int *)0x0);
        while (param_1 = piVar10, piVar10 != (int *)0x0) {
          param_1 = (int *)piVar10[3];
          (**(code **)(DAT_007d3ff8 + 0x10c))(*piVar10);
          *piVar10 = 0;
          (**(code **)(DAT_007d3ff8 + 0x11c))
                    (*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8),piVar10);
          piVar10 = param_1;
        }
      }
      (**(code **)(DAT_007d3ff8 + 0x10c))(puVar8);
      puVar17 = puVar17 + 3;
    }
    FUN_004cc9f0(*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8));
    uVar9 = 0;
    iVar12 = 0;
    uVar13 = (uint)uVar11;
    *(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8) = 0;
    iVar14 = 0x10;
    if (uVar13 != 0) {
      do {
        iVar2 = *(int *)(*(int *)(iVar16 + uVar9 * 4) + 4);
        iVar12 = iVar12 + iVar2;
        uVar9 = uVar9 + 1;
        iVar14 = iVar14 + 0xc + iVar2 * 2;
      } while (uVar9 < uVar13);
    }
    param_3 = uVar13;
    puVar4 = (undefined4 *)FUN_004f0970(iVar14);
    *puVar4 = 1;
    *(ushort *)(puVar4 + 1) = uVar11;
    uVar9 = 0;
    uVar1 = *(undefined2 *)(DAT_00912118 + DAT_007d3ff8);
    puVar4[3] = 0;
    *(undefined2 *)((int)puVar4 + 6) = uVar1;
    puVar4[2] = iVar12;
    *(short *)(DAT_00912118 + DAT_007d3ff8) = *(short *)(DAT_00912118 + DAT_007d3ff8) + 1;
    puVar8 = puVar4 + 4 + uVar13 * 3;
    puVar18 = puVar4 + 4;
    if (uVar13 != 0) {
      do {
        *puVar18 = puVar8;
        puVar18[1] = *(undefined4 *)(*(int *)(iVar16 + uVar9 * 4) + 4);
        puVar18[2] = *(undefined4 *)(*(int *)(iVar16 + uVar9 * 4) + 8);
        puVar20 = *(undefined4 **)(iVar16 + uVar9 * 4);
        uVar13 = puVar20[1] * 2;
        puVar20 = (undefined4 *)*puVar20;
        puVar21 = puVar8;
        for (uVar15 = uVar13 >> 2; uVar15 != 0; uVar15 = uVar15 - 1) {
          *puVar21 = *puVar20;
          puVar20 = puVar20 + 1;
          puVar21 = puVar21 + 1;
        }
        for (uVar13 = uVar13 & 3; uVar13 != 0; uVar13 = uVar13 - 1) {
          *(undefined1 *)puVar21 = *(undefined1 *)puVar20;
          puVar20 = (undefined4 *)((int)puVar20 + 1);
          puVar21 = (undefined4 *)((int)puVar21 + 1);
        }
        param_2 = (undefined4 *)((int)puVar8 + puVar18[1] * 2);
        (**(code **)(DAT_007d3ff8 + 0x10c))(*(undefined4 *)(iVar16 + uVar9 * 4));
        *(undefined4 *)(iVar16 + uVar9 * 4) = 0;
        uVar9 = uVar9 + 1;
        puVar8 = param_2;
        puVar18 = puVar18 + 3;
      } while (uVar9 < param_3);
    }
    (**(code **)(DAT_007d3ff8 + 0x10c))(piVar3);
    (**(code **)(DAT_007d3ff8 + 0x10c))(iVar16);
    (**(code **)(DAT_007d3ff8 + 0x10c))(puVar5);
  }
  return puVar4;
}
