// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* RWC-MATCH run 40: compiles (was C2115/C2069). FUN_0053e430, FUN_0053e730, FUN_0053e760 prototypes copied from their own
   sources in re/match/rwl03. 28.1% rw9044, 24.1% ddk, not worked further. */
/* MATCH-RWL03 0x0053dea0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef long double float10;
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_005cd03c, 0x005cd03c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005ceac0, 0x005ceac0);
MGLOBAL(undefined1, FUN_0053e6d0, 0x0053e6d0);
MGLOBAL(undefined1, DAT_005cd120, 0x005cd120);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
void FUN_0053de50(int * p0);
void FUN_0053e430(int p0, int p1, float p2, int * p3);
float10 FUN_0053e5d0(int p0, uint * p1, int p2);
float10 FUN_0053e690(float p0, float p1);
void FUN_0053e6f0(int p0);
undefined4 * FUN_0053e730(undefined2 p0, undefined2 p1);
undefined4 * FUN_0053e760(undefined4 p0, undefined4 p1, undefined4 p2);
void FUN_005c24e0(undefined1 * p0, uint p1, uint p2, undefined * p3);
MGLOBAL(int, _DAT_005cd03c, 0x005cd03c);
MGLOBAL(int, _DAT_005cd120, 0x005cd120);
MGLOBAL(int, _DAT_005ceac0, 0x005ceac0);
// MATCH: 0x0053dea0 FUN_0053dea0
/* [C1 2026-05-20] SAH (Surface Area Heuristic) BVH split for the in-build tree. */

undefined4 * FUN_0053dea0(uint *param_1)

{
  ushort *puVar1;
  int *piVar2;
  float fVar3;
  float fVar4;
  undefined4 *puVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  int *piVar11;
  int iVar12;
  uint uVar13;
  uint *puVar14;
  uint *puVar15;
  float10 fVar16;
  int local_a4;
  int local_9c;
  float local_98;
  float local_90;
  uint local_8c;
  int local_88;
  uint local_84;
  uint local_80;
  int local_7c;
  float local_78;
  int local_74;
  float local_70;
  undefined4 local_6c;
  float local_68 [4];
  undefined4 *local_58;
  float local_54;
  float local_50;
  int local_4c;
  int local_48;
  uint local_3c [5];
  uint auStack_28 [7];
  int local_c;
  
  uVar6 = param_1[0xb];
  if (uVar6 < 4) {
    uVar13 = param_1[0xc];
    puVar5 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(8,0);
    if (puVar5 != (undefined4 *)0x0) {
      *(short *)(puVar5 + 1) = (short)uVar6;
      *(short *)((int)puVar5 + 6) = (short)uVar13;
      *puVar5 = 0xffffffff;
      return puVar5;
    }
  }
  else if (param_1[4] < 0x20) {
    iVar12 = 0;
    uVar6 = 0;
    piVar11 = (int *)(param_1[2] + param_1[0xc] * 8);
    if (*param_1 != 0) {
      iVar7 = 0;
      do {
        uVar6 = uVar6 + 1;
        *(undefined4 *)(param_1[1] + 4 + iVar7) = 0;
        iVar7 = iVar7 + 0xc;
      } while (uVar6 < *param_1);
    }
    uVar6 = 0;
    if (param_1[0xb] != 0) {
      do {
        iVar7 = 0;
        do {
          puVar1 = (ushort *)(*piVar11 + iVar7);
          iVar7 = iVar7 + 2;
          piVar2 = (int *)(param_1[1] + 4 + (uint)*puVar1 * 0xc);
          *piVar2 = *piVar2 + 1;
        } while (iVar7 < 6);
        uVar6 = uVar6 + 1;
        piVar11 = piVar11 + 2;
      } while (uVar6 < param_1[0xb]);
    }
    uVar6 = 0;
    uVar13 = 0;
    if (*param_1 != 0) {
      iVar7 = 0;
      do {
        if (*(int *)(param_1[1] + iVar7 + 4) != 0) {
          uVar6 = uVar6 + 1;
          *(uint *)((param_1[0xe] - 4) + uVar6 * 4) = param_1[1] + iVar7;
        }
        uVar13 = uVar13 + 1;
        iVar7 = iVar7 + 0xc;
      } while (uVar13 < *param_1);
    }
    param_1[0xd] = uVar6;
    iVar7 = 0;
    local_98 = 3.4028235e+38;
    local_7c = 0;
    local_68[0] = (float)param_1[5] - (float)param_1[8];
    local_90 = 0.0;
    iVar8 = 0;
    local_68[1] = (float)param_1[6] - (float)param_1[9];
    local_68[2] = (float)param_1[7] - (float)param_1[10];
    do {
      if (*(float *)((int)local_68 + iVar7) < *(float *)((int)local_68 + iVar8)) {
        iVar7 = iVar8;
      }
      iVar8 = iVar8 + 4;
    } while (iVar8 < 0xc);
    local_58 = (undefined4 *)((int)local_68 + iVar7);
    local_54 = *(float *)((int)local_68 + iVar7) * _DAT_005cd03c;
    do {
      fVar16 = (float10)FUN_0053e690(*(undefined4 *)((int)local_68 + iVar12),*local_58);
      local_70 = (float)fVar16;
      uVar6 = 0;
      if (param_1[0xd] != 0) {
        do {
          uVar6 = uVar6 + 1;
          piVar11 = *(int **)((param_1[0xe] - 4) + uVar6 * 4);
          piVar11[2] = *(int *)(*piVar11 + iVar12);
        } while (uVar6 < param_1[0xd]);
      }
      FUN_005c24e0(param_1[0xe],param_1[0xd],4,FUN_0053e6d0);
      uVar10 = param_1[0xe];
      uVar9 = uVar6 >> 1;
      uVar6 = uVar6 >> 2;
      uVar13 = uVar6 + uVar9;
      local_50 = *(float *)(*(int *)(uVar10 + uVar9 * 4) + 8);
      local_78 = *(float *)(*(int *)(uVar10 + uVar13 * 4) + 8) -
                 *(float *)(*(int *)(uVar6 * 4 + uVar10) + 8);
      if (local_54 <= local_78) {
        local_78 = _DAT_005cd120 / local_78;
        for (uVar10 = uVar6; uVar10 <= uVar13; uVar10 = uVar10 + 1) {
          iVar7 = *(int *)(param_1[0xe] + uVar10 * 4);
          fVar3 = local_50 - *(float *)(iVar7 + 8);
          if (fVar3 < DAT_005d757c) {
            fVar3 = -fVar3;
          }
          *(float *)(iVar7 + 8) = fVar3;
        }
        FUN_005c24e0(param_1[0xe] + uVar6 * 4,(uVar13 - uVar6) + 1,4,FUN_0053e6d0);
      }
      else {
        local_78 = 0.0;
        uVar13 = uVar9;
        uVar6 = uVar9;
      }
      iVar7 = -1;
      local_9c = 0x32;
      local_a4 = local_7c;
      while ((local_7c = local_a4, uVar6 <= uVar13 && (0 < local_9c))) {
        fVar3 = *(float *)(**(int **)(param_1[0xe] + uVar6 * 4) + iVar12);
        local_68[3] = fVar3 * local_78;
        local_74 = (int)ROUND(fVar3 * local_78);
        if (local_74 != iVar7) {
          uVar10 = 0;
          if (param_1[0xd] != 0) {
            do {
              piVar11 = *(int **)(param_1[0xe] + uVar10 * 4);
              fVar4 = *(float *)(*piVar11 + iVar12) - fVar3;
              piVar11[1] = 0;
              if (fVar4 < DAT_005d757c != (fVar4 == DAT_005d757c)) {
                puVar14 = (uint *)(*(int *)(param_1[0xe] + uVar10 * 4) + 4);
                *puVar14 = *puVar14 | 1;
              }
              if (DAT_005d757c <= fVar4) {
                puVar14 = (uint *)(*(int *)(param_1[0xe] + uVar10 * 4) + 4);
                *puVar14 = *puVar14 | 2;
              }
              uVar10 = uVar10 + 1;
            } while (uVar10 < param_1[0xd]);
          }
          FUN_0053e430(param_1,iVar12,fVar3,&local_4c);
          if ((local_4c != 0) && (local_48 != 0)) {
            fVar16 = (float10)FUN_0053e5d0(param_1,&local_4c,iVar12);
            if (fVar16 * (float10)local_70 < (float10)local_98) {
              local_98 = (float)(fVar16 * (float10)local_70);
              local_90 = fVar3;
              local_7c = iVar12;
            }
          }
          local_9c = local_9c + -1;
          iVar7 = local_74;
        }
        uVar6 = uVar6 + 1;
        local_a4 = local_7c;
      }
      iVar12 = iVar12 + 4;
    } while (iVar12 < 0xc);
    if (_DAT_005ceac0 <= local_98) {
      uVar6 = param_1[0xb];
      if (uVar6 < 0xf0) {
        puVar5 = (undefined4 *)FUN_0053e730(uVar6,param_1[0xc]);
        return puVar5;
      }
      local_80 = param_1[8];
      local_84 = param_1[5];
      local_8c = uVar6 >> 1;
      local_88 = uVar6 - local_8c;
      local_a4 = 0;
    }
    else {
      uVar6 = 0;
      if (param_1[0xd] != 0) {
        do {
          piVar11 = *(int **)(param_1[0xe] + uVar6 * 4);
          fVar3 = *(float *)(*piVar11 + local_a4) - local_90;
          piVar11[1] = 0;
          if (fVar3 < DAT_005d757c != (fVar3 == DAT_005d757c)) {
            puVar14 = (uint *)(*(int *)(param_1[0xe] + uVar6 * 4) + 4);
            *puVar14 = *puVar14 | 1;
          }
          if (DAT_005d757c <= fVar3) {
            puVar14 = (uint *)(*(int *)(param_1[0xe] + uVar6 * 4) + 4);
            *puVar14 = *puVar14 | 2;
          }
          uVar6 = uVar6 + 1;
        } while (uVar6 < param_1[0xd]);
      }
      FUN_0053e430(param_1,local_a4,local_90,&local_8c);
      FUN_0053e6f0(param_1);
    }
    param_1[4] = param_1[4] + 1;
    puVar5 = (undefined4 *)FUN_0053e760(local_a4,local_84,local_80);
    if (puVar5 != (undefined4 *)0x0) {
      puVar14 = param_1;
      puVar15 = local_3c;
      for (iVar12 = 0xf; iVar12 != 0; iVar12 = iVar12 + -1) {
        *puVar15 = *puVar14;
        puVar14 = puVar14 + 1;
        puVar15 = puVar15 + 1;
      }
      auStack_28[6] = local_8c;
      *(uint *)((int)auStack_28 + local_a4) = local_84;
      iVar12 = FUN_0053dea0(local_3c);
      puVar5[1] = iVar12;
      if (iVar12 == 0) {
        (**(code **)(DAT_007d3ff8 + 0x10c))(puVar5);
        return (undefined4 *)0x0;
      }
      puVar14 = param_1;
      puVar15 = local_3c;
      for (iVar12 = 0xf; iVar12 != 0; iVar12 = iVar12 + -1) {
        *puVar15 = *puVar14;
        puVar14 = puVar14 + 1;
        puVar15 = puVar15 + 1;
      }
      auStack_28[6] = local_88;
      local_c = param_1[0xc] + local_8c;
      *(uint *)((int)auStack_28 + local_a4 + 0xc) = local_80;
      iVar12 = FUN_0053dea0(local_3c);
      puVar5[2] = iVar12;
      if (iVar12 == 0) {
        FUN_0053de50(puVar5);
        return (undefined4 *)0x0;
      }
      return puVar5;
    }
  }
  else {
    if (uVar6 < 0xf0) {
      puVar5 = (undefined4 *)FUN_0053e730(uVar6,param_1[0xc]);
      return puVar5;
    }
    local_70 = 3.9937e-43;
    local_6c = FUN_004d7ff0(0);
    FUN_004d8480(&local_70);
  }
  return (undefined4 *)0x0;
}
