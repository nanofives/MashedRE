// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x004f2570: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00912118, 0x00912118);
undefined4 FUN_004cc9f0(int p0);
int FUN_004f2e70(uint p0, undefined4 * p1, undefined4 p2, int p3);
void FUN_004f3030(int * p0, int p1, int p2);
int FUN_004f3150(int * p0, short * p1, undefined4 p2, undefined4 p3, int p4);
// MATCH: 0x004f2570 FUN_004f2570
undefined4 FUN_004f2570(int param_1,uint param_2,int *param_3,int param_4)

{
  undefined2 *puVar1;
  int **ppiVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int *piVar8;
  int iVar9;
  int *piVar10;
  int iVar11;
  int *piVar12;
  undefined2 *puVar13;
  int iVar14;
  int iVar15;
  uint uVar16;
  undefined4 *puVar17;
  undefined4 *puVar18;
  int iVar19;
  int iVar20;
  int iVar21;
  int iVar22;
  int iVar23;
  uint uVar24;
  int local_4c;
  uint local_48;
  int local_3c;
  uint uStack_38;
  uint uStack_28;
  int local_1c;
  undefined4 local_18;
  undefined4 local_14;
  int *local_10;
  int local_c [3];
  
  local_1c = 0;
  local_48 = 0;
  local_3c = 0;
  local_4c = 0;
  local_10 = (int *)0x0;
  local_c[0] = 0;
  local_c[1] = 0;
  local_c[2] = 0;
  local_18 = 0;
  local_14 = 0;
  iVar6 = FUN_004f2e70(param_2,&local_18,&local_1c,param_1);
  uVar7 = 0;
  if (param_2 != 0) {
    do {
      iVar11 = *(int *)(iVar6 + uVar7 * 4);
      *(int **)(iVar11 + 0x10) = (&local_10)[*(byte *)(iVar11 + 0x20)];
      iVar11 = *(int *)(iVar6 + uVar7 * 4);
      iVar9 = *(int *)(iVar11 + 0x10);
      if (iVar9 != 0) {
        *(int *)(iVar9 + 0x14) = iVar11;
      }
      piVar8 = *(int **)(iVar6 + uVar7 * 4);
      uVar7 = uVar7 + 1;
      (&local_10)[*(byte *)(piVar8 + 8)] = piVar8;
      *(undefined4 *)(*(int *)(iVar6 + -4 + uVar7 * 4) + 0x14) = 0;
    } while (uVar7 < param_2);
  }
  piVar8 = (int *)(**(code **)(DAT_007d3ff8 + 0x118))
                            (*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8),0x10502);
  iVar11 = param_2 * 2 + 2;
  piVar8[1] = 0;
  piVar8[2] = iVar11;
  iVar9 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar11 * 4,0x10502);
  *piVar8 = iVar9;
  piVar10 = (int *)(**(code **)(DAT_007d3ff8 + 0x118))
                             (*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8),0x10502);
  piVar10[2] = iVar11;
  piVar10[1] = 0;
  iVar11 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar11 * 4,0x10502);
  *piVar10 = iVar11;
  if (param_2 == 0) {
LAB_004f2d7c:
    (**(code **)(DAT_007d3ff8 + 0x10c))(*piVar10);
    *piVar10 = 0;
    (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8),piVar10);
    (**(code **)(DAT_007d3ff8 + 0x10c))(*piVar8);
    *piVar8 = 0;
    (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8),piVar8);
    iVar11 = local_1c;
    while (iVar11 != 0) {
      iVar9 = *(int *)(iVar11 + 0xc);
      (**(code **)(DAT_007d3ff8 + 0x11c))(local_14,iVar11);
      iVar11 = iVar9;
    }
    FUN_004cc9f0(local_14);
    uVar7 = 0;
    local_14 = 0;
    if (param_2 != 0) {
      do {
        (**(code **)(DAT_007d3ff8 + 0x11c))(local_18,*(undefined4 *)(iVar6 + uVar7 * 4));
        *(undefined4 *)(iVar6 + uVar7 * 4) = 0;
        uVar7 = uVar7 + 1;
      } while (uVar7 < param_2);
    }
    FUN_004cc9f0(local_18);
    local_18 = 0;
    (**(code **)(DAT_007d3ff8 + 0x10c))(iVar6);
    return 1;
  }
LAB_004f269b:
  iVar9 = 0;
  uStack_38 = 0;
  iVar11 = (-(uint)(param_4 != 0) & 0xfffffffd) + 3;
  if (local_10 == (int *)0x0) {
    iVar14 = 1;
    iVar22 = local_c[0];
    while (iVar22 == 0) {
      piVar12 = local_c + iVar14;
      iVar14 = iVar14 + 1;
      iVar22 = *piVar12;
    }
    piVar12 = (&local_10)[iVar14];
    ppiVar2 = &local_10 + iVar14;
    iVar22 = *(int *)(piVar12[3] + 4);
    if ((iVar22 == 0) || (*(int *)(iVar22 + 0x18) != 0)) {
      iVar14 = 0;
    }
    else {
      iVar14 = 1;
    }
    iVar15 = *(int *)(piVar12[3] + 8);
    if ((iVar15 != 0) && (*(int *)(iVar15 + 0x18) == 0)) {
      iVar9 = 1;
    }
    if (1 < (uint)(iVar14 + iVar9)) {
      iVar9 = *(int *)(piVar12[2] + 4);
      if ((iVar9 == 0) || (*(int *)(iVar9 + 0x18) != 0)) {
        iVar9 = 0;
      }
      else {
        iVar9 = 1;
      }
      iVar14 = *(int *)(piVar12[2] + 8);
      if ((iVar14 == 0) || (*(int *)(iVar14 + 0x18) != 0)) {
        iVar14 = 0;
      }
      else {
        iVar14 = 1;
      }
      if (1 < (uint)(iVar9 + iVar14)) {
        uStack_28 = 1;
        uVar7 = uStack_28;
        goto LAB_004f2a3c;
      }
    }
    iVar9 = *(int *)(piVar12[1] + 4);
    if ((iVar9 == 0) || (*(int *)(iVar9 + 0x18) != 0)) {
      iVar14 = 0;
    }
    else {
      iVar14 = 1;
    }
    iVar3 = *(int *)(piVar12[1] + 8);
    if ((iVar3 == 0) || (*(int *)(iVar3 + 0x18) != 0)) {
      iVar19 = 0;
    }
    else {
      iVar19 = 1;
    }
    if (1 < (uint)(iVar14 + iVar19)) {
      if ((iVar22 == 0) || (*(int *)(iVar22 + 0x18) != 0)) {
        iVar14 = 0;
      }
      else {
        iVar14 = 1;
      }
      if ((iVar15 == 0) || (*(int *)(iVar15 + 0x18) != 0)) {
        iVar19 = 0;
      }
      else {
        iVar19 = 1;
      }
      if (1 < (uint)(iVar14 + iVar19)) {
        uStack_28 = 2;
        uVar7 = uStack_28;
        goto LAB_004f2a3c;
      }
    }
    iVar14 = *(int *)(piVar12[2] + 4);
    if ((iVar14 == 0) || (*(int *)(iVar14 + 0x18) != 0)) {
      iVar19 = 0;
    }
    else {
      iVar19 = 1;
    }
    iVar4 = *(int *)(piVar12[2] + 8);
    if ((iVar4 == 0) || (*(int *)(iVar4 + 0x18) != 0)) {
      iVar20 = 0;
    }
    else {
      iVar20 = 1;
    }
    if (1 < (uint)(iVar19 + iVar20)) {
      if ((iVar9 == 0) || (*(int *)(iVar9 + 0x18) != 0)) {
        iVar19 = 0;
      }
      else {
        iVar19 = 1;
      }
      if ((iVar3 == 0) || (*(int *)(iVar3 + 0x18) != 0)) {
        iVar20 = 0;
      }
      else {
        iVar20 = 1;
      }
      if (1 < (uint)(iVar19 + iVar20)) {
        uStack_28 = 0;
        uVar7 = uStack_28;
        goto LAB_004f2a3c;
      }
    }
    if ((iVar9 == 0) || (*(int *)(iVar9 + 0x18) != 0)) {
      iVar19 = 0;
    }
    else {
      iVar19 = 1;
    }
    if ((iVar3 == 0) || (*(int *)(iVar3 + 0x18) != 0)) {
      iVar20 = 0;
    }
    else {
      iVar20 = 1;
    }
    if ((iVar14 == 0) || (*(int *)(iVar14 + 0x18) != 0)) {
      iVar23 = 0;
    }
    else {
      iVar23 = 1;
    }
    if ((iVar4 == 0) || (*(int *)(iVar4 + 0x18) != 0)) {
      iVar21 = 0;
    }
    else {
      iVar21 = 1;
    }
    if ((uint)(iVar23 + iVar21) < (uint)(iVar19 + iVar20)) {
      if ((iVar9 == 0) || (*(int *)(iVar9 + 0x18) != 0)) {
        iVar9 = 0;
      }
      else {
        iVar9 = 1;
      }
      if ((iVar3 == 0) || (*(int *)(iVar3 + 0x18) != 0)) {
        iVar14 = 0;
      }
      else {
        iVar14 = 1;
      }
      if ((iVar22 == 0) || (*(int *)(iVar22 + 0x18) != 0)) {
        uVar7 = 0;
      }
      else {
        uVar7 = 1;
      }
      if ((iVar15 == 0) || (*(int *)(iVar15 + 0x18) != 0)) {
        uStack_28 = (uVar7 < (uint)(iVar9 + iVar14)) + 1;
        uVar7 = uStack_28;
      }
      else {
        uStack_28 = (uVar7 + 1 < (uint)(iVar9 + iVar14)) + 1;
        uVar7 = uStack_28;
      }
    }
    else {
      if ((iVar14 == 0) || (*(int *)(iVar14 + 0x18) != 0)) {
        iVar9 = 0;
      }
      else {
        iVar9 = 1;
      }
      if ((iVar4 == 0) || (*(int *)(iVar4 + 0x18) != 0)) {
        iVar14 = 0;
      }
      else {
        iVar14 = 1;
      }
      if ((iVar22 == 0) || (*(int *)(iVar22 + 0x18) != 0)) {
        iVar22 = 0;
      }
      else {
        iVar22 = 1;
      }
      if ((iVar15 == 0) || (*(int *)(iVar15 + 0x18) != 0)) {
        iVar15 = 0;
      }
      else {
        iVar15 = 1;
      }
      uStack_28 = (uint)((uint)(iVar9 + iVar14) <= (uint)(iVar22 + iVar15));
      uVar7 = uStack_28;
    }
LAB_004f2a3c:
    do {
      uVar16 = 0;
      do {
        uVar16 = uVar16 + 1;
        iVar9 = *(int *)(iVar6 + -4 + uVar16 * 4);
        *(undefined4 *)(iVar9 + 0x1c) = *(undefined4 *)(iVar9 + 0x18);
      } while (uVar16 < param_2);
      iVar9 = iVar11 + 1;
      uVar16 = uVar7;
      if (iVar11 == 0) {
LAB_004f2a75:
        uVar16 = uVar16 % 3;
      }
      else {
        if (iVar11 == 1) {
          uVar16 = uVar7 + 1;
          goto LAB_004f2a75;
        }
        uVar16 = uStack_28;
        if (iVar11 == 2) {
          uVar16 = uVar7 + 2;
          goto LAB_004f2a75;
        }
      }
      if (uVar16 == 0) {
        local_3c = (*ppiVar2)[2];
        local_4c = (*ppiVar2)[1];
      }
      else if (uVar16 == 1) {
        local_3c = (*ppiVar2)[3];
        local_4c = (*ppiVar2)[2];
      }
      else if (uVar16 == 2) {
        local_3c = (*ppiVar2)[1];
        local_4c = (*ppiVar2)[3];
      }
      iVar11 = param_1 + **ppiVar2 * 0x14;
      *(undefined2 *)*piVar8 = *(undefined2 *)(iVar11 + (uVar16 % 3) * 2);
      *(undefined2 *)(*piVar8 + 2) = *(undefined2 *)(iVar11 + ((uVar16 + 1) % 3) * 2);
      *(undefined2 *)(*piVar8 + 4) = *(undefined2 *)(iVar11 + ((uVar16 + 2) % 3) * 2);
      piVar8[1] = 3;
      FUN_004f3030(*ppiVar2,&local_10,iVar9);
      iVar11 = FUN_004f3150(piVar8,local_3c,&local_10,param_1,iVar9);
      uVar24 = iVar11 + 1 + local_48;
      iVar11 = *(int *)(local_4c + 4);
      if (iVar9 < 4) {
        if ((iVar11 == 0) || (*(int *)(iVar11 + 0x1c) != 0)) {
          iVar22 = 0;
        }
        else {
          iVar22 = 1;
        }
        if ((*(int *)(local_4c + 8) == 0) || (*(int *)(*(int *)(local_4c + 8) + 0x1c) != 0)) {
LAB_004f2b9d:
          iVar14 = 0;
        }
        else {
          iVar14 = 1;
        }
      }
      else {
        if ((iVar11 == 0) || (*(int *)(iVar11 + 0x18) != 0)) {
          iVar22 = 0;
        }
        else {
          iVar22 = 1;
        }
        if ((*(int *)(local_4c + 8) == 0) || (*(int *)(*(int *)(local_4c + 8) + 0x18) != 0))
        goto LAB_004f2b9d;
        iVar14 = 1;
      }
      iVar11 = iVar9;
      if (iVar22 + iVar14 == 0) goto LAB_004f2c27;
      *(undefined2 *)*piVar10 = *(undefined2 *)(*piVar8 + 2);
      *(undefined2 *)(*piVar10 + 2) = *(undefined2 *)*piVar8;
      piVar10[1] = 2;
      iVar22 = FUN_004f3150(piVar10,local_4c,&local_10,param_1,iVar9);
      uVar24 = uVar24 + iVar22;
      uVar5 = piVar10[1];
      if ((uVar5 & 1) != 0) {
        *(undefined2 *)(*piVar10 + uVar5 * 2) = *(undefined2 *)(*piVar10 + -4 + uVar5 * 2);
        piVar10[1] = piVar10[1] + 1;
      }
      if (uStack_38 < uVar24) {
        uStack_38 = uVar24;
        uStack_28 = uVar16;
      }
    } while (iVar9 < 4);
    piVar12 = (int *)(**(code **)(DAT_007d3ff8 + 0x118))
                               (*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8),0x30502);
    piVar12[3] = *param_3;
    *param_3 = (int)piVar12;
    iVar9 = piVar8[1];
    iVar11 = piVar10[1];
    piVar12[1] = 0;
    iVar11 = iVar9 + -2 + iVar11;
    piVar12[2] = iVar11;
    iVar11 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar11 * 4,0x30502);
    *piVar12 = iVar11;
    uVar7 = piVar10[1];
    while (2 < uVar7) {
      *(undefined2 *)(*piVar12 + piVar12[1] * 2) = *(undefined2 *)(*piVar10 + -2 + piVar10[1] * 2);
      piVar12[1] = piVar12[1] + 1;
      uVar7 = piVar10[1] - 1;
      piVar10[1] = uVar7;
    }
    puVar17 = (undefined4 *)*piVar8;
    puVar18 = (undefined4 *)(*piVar12 + piVar12[1] * 2);
    for (uVar7 = piVar8[1] & 0x3fffffff; uVar7 != 0; uVar7 = uVar7 - 1) {
      *puVar18 = *puVar17;
      puVar17 = puVar17 + 1;
      puVar18 = puVar18 + 1;
    }
    for (iVar11 = 0; iVar11 != 0; iVar11 = iVar11 + -1) {
      *(undefined1 *)puVar18 = *(undefined1 *)puVar17;
      puVar17 = (undefined4 *)((int)puVar17 + 1);
      puVar18 = (undefined4 *)((int)puVar18 + 1);
    }
    piVar12[1] = piVar12[2];
    local_48 = uVar24;
    goto LAB_004f2d68;
  }
  iVar11 = *local_10;
  piVar12 = (int *)(**(code **)(DAT_007d3ff8 + 0x118))
                             (*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8),0x30502);
  piVar12[3] = *param_3;
  *param_3 = (int)piVar12;
  piVar12[2] = 3;
  piVar12[1] = 3;
  puVar13 = (undefined2 *)(**(code **)(DAT_007d3ff8 + 0x108))(0xc,0x30502);
  *piVar12 = (int)puVar13;
  puVar1 = (undefined2 *)(param_1 + iVar11 * 0x14);
  *puVar13 = *puVar1;
  *(undefined2 *)(*piVar12 + 2) = puVar1[1];
  *(undefined2 *)(*piVar12 + 4) = puVar1[2];
  local_10[6] = 1;
  local_10[7] = 1;
  local_10 = (int *)local_10[4];
  if (local_10 != (int *)0x0) {
    local_10[5] = 0;
  }
  local_48 = local_48 + 1;
  goto LAB_004f2d68;
LAB_004f2c27:
  if (uStack_38 < uVar24) {
    uStack_38 = uVar24;
    uStack_28 = uVar16;
  }
  if (3 < iVar9) goto LAB_004f2cfb;
  goto LAB_004f2a3c;
LAB_004f2cfb:
  puVar17 = (undefined4 *)
            (**(code **)(DAT_007d3ff8 + 0x118))
                      (*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8),0x30502);
  puVar17[3] = *param_3;
  *param_3 = (int)puVar17;
  puVar17[2] = piVar8[1];
  puVar17[1] = piVar8[1];
  puVar18 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(piVar8[1] << 2,0x30502);
  *puVar17 = puVar18;
  puVar17 = (undefined4 *)*piVar8;
  for (uVar7 = piVar8[1] & 0x3fffffff; uVar7 != 0; uVar7 = uVar7 - 1) {
    *puVar18 = *puVar17;
    puVar17 = puVar17 + 1;
    puVar18 = puVar18 + 1;
  }
  for (iVar11 = 0; local_48 = uVar24, iVar11 != 0; iVar11 = iVar11 + -1) {
    *(undefined1 *)puVar18 = *(undefined1 *)puVar17;
    puVar17 = (undefined4 *)((int)puVar17 + 1);
    puVar18 = (undefined4 *)((int)puVar18 + 1);
  }
LAB_004f2d68:
  if (param_2 <= local_48) goto LAB_004f2d7c;
  goto LAB_004f269b;
}
