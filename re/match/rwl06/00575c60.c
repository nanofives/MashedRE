// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00575c60: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef int code();
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
undefined4 * FUN_004c4600(undefined4 * p0, undefined4 * p1, undefined4 * p2);
uint FUN_00575fe0(int p0, undefined4 * p1, undefined4 p2, int p3, undefined4 * p4, int p5, undefined4 * p6, float * p7, float * p8, uint p9);
// MATCH: 0x00575c60 FUN_00575c60
int FUN_00575c60(undefined4 *param_1,undefined4 *param_2,float *param_3,float *param_4,
                float *param_5,undefined4 param_6)

{
  int iVar1;
  float fVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
  int iVar11;
  float *pfVar12;
  undefined4 *puVar13;
  undefined4 *puVar14;
  undefined4 *puVar15;
  float *pfVar16;
  float *pfVar17;
  undefined4 *puVar18;
  float *local_20;
  undefined4 *p3b;
  float *local_1c;
  uint local_18;
  
  iVar8 = (int)param_1;
  iVar1 = *(int *)((int)param_1 + 0x10);
  iVar10 = *(int *)((int)param_1 + 0xc);
  local_1c = param_3;
  fVar2 = *(float *)((int)param_1 + 0x70);
  puVar18 = (undefined4 *)(iVar10 + iVar1 * 0x14);
  iVar3 = *(int *)((int)param_1 + 0x14);
  iVar4 = *(int *)((int)param_1 + 0x18);
  iVar5 = *(int *)((int)param_1 + 0x20);
  puVar13 = (undefined4 *)(*(int *)((int)param_1 + 0x1c) * 0x40 + iVar4);
  pfVar12 = (float *)(*(int *)((int)param_1 + 0x24) + iVar1 * 0x20);
  puVar15 = (undefined4 *)*param_2;
  if (*(short *)(param_2 + 1) == -1) {
    if ((*(byte *)(puVar15[0x17] + 0x40) & 2) == 0) {
      puVar14 = puVar13;
      for (iVar10 = 0x10; iVar10 != 0; iVar10 = iVar10 + -1) {
        *puVar14 = *puVar15;
        puVar15 = puVar15 + 1;
        puVar14 = puVar14 + 1;
      }
    }
    else {
      puVar13[4] = 0;
      puVar13[10] = 0x3f800000;
      puVar13[5] = 0x3f800000;
      *puVar13 = 0x3f800000;
      puVar13[2] = 0;
      puVar13[1] = 0;
      puVar13[9] = 0;
      puVar13[8] = 0;
      puVar13[6] = 0;
      puVar13[0xe] = 0;
      puVar13[0xd] = 0;
      puVar13[0xc] = 0;
      puVar13[3] = puVar13[3] | 0x20003;
    }
    pfVar12 = (float *)(*(int *)((int)param_1 + 0x24) + iVar1 * 0x20);
    for (iVar10 = 8; iVar10 != 0; iVar10 = iVar10 + -1) {
      *pfVar12 = *param_4;
      param_4 = param_4 + 1;
      pfVar12 = pfVar12 + 1;
    }
    FUN_00575fe0((int)param_1,param_2,
                 (((uint)puVar18 >> 0x10) << 0x10) | *(ushort *)(param_2 + 1),*param_2,
                 puVar13,0,puVar18,param_3,param_5,param_6);
    return *(int *)((int)param_1 + 0x10) - iVar1;
  }
  p3b = (undefined4 *)puVar15[4];
  local_18 = 0;
  uVar9 = (uint)*(ushort *)(puVar15 + 3);
  param_1 = puVar18;
  local_20 = pfVar12;
  if (uVar9 != 0) {
    do {
      if (((undefined4 *)(iVar5 * 0x40 + iVar4) <= puVar13) ||
         ((undefined4 *)(iVar10 + iVar3 * 0x14) <= puVar18)) {
        *(undefined4 *)(iVar8 + 0x78) = 1;
        break;
      }
      puVar15 = (undefined4 *)*p3b;
      puVar14 = (undefined4 *)p3b[2];
      iVar6 = puVar15[0x17];
      if ((*(byte *)(iVar6 + 0x40) & 2) == 0) {
        if (puVar14 == (undefined4 *)0x0) {
          puVar14 = puVar13;
          for (iVar11 = 0x10; puVar18 = param_1, iVar11 != 0; iVar11 = iVar11 + -1) {
            *puVar14 = *puVar15;
            puVar15 = puVar15 + 1;
            puVar14 = puVar14 + 1;
          }
        }
        else {
          FUN_004c4600(puVar13,puVar15,puVar14);
          pfVar12 = local_20;
        }
      }
      else {
        puVar15 = puVar13;
        for (iVar11 = 0x10; puVar18 = param_1, iVar11 != 0; iVar11 = iVar11 + -1) {
          *puVar15 = *puVar14;
          puVar14 = puVar14 + 1;
          puVar15 = puVar15 + 1;
        }
      }
      if (local_1c == (float *)0x0) {
        if (uVar9 != 1) {
          (**(code **)(iVar6 + 0x10))(*p3b,puVar13,0,pfVar12);
          pfVar12 = local_20;
          goto LAB_00575eaa;
        }
        pfVar16 = param_4;
        pfVar17 = pfVar12;
        for (iVar11 = 8; puVar15 = param_1, iVar11 != 0; iVar11 = iVar11 + -1) {
          *pfVar17 = *pfVar16;
          pfVar16 = pfVar16 + 1;
          pfVar17 = pfVar17 + 1;
        }
LAB_00575e25:
        if ((*(byte *)(iVar6 + 0x40) & 1) == 0) {
          puVar15[4] = (undefined4)puVar13;
          puVar18 = puVar15 + 5;
          pfVar12 = pfVar12 + 8;
          *puVar15 = *param_2;
          puVar15[1] = param_2[1];
          *(undefined2 *)(puVar15 + 1) = (undefined2)local_18;
          puVar13 = puVar13 + 0x10;
          uVar7 = *p3b;
          puVar15[3] = 0;
          puVar15[2] = uVar7;
          param_1 = puVar18;
          local_20 = pfVar12;
        }
        else {
          FUN_00575fe0(iVar8,param_2,local_18,*p3b,puVar13,0,puVar15,local_1c,param_5,param_6);
          puVar18 = (undefined4 *)(*(int *)(iVar8 + 0xc) + *(int *)(iVar8 + 0x10) * 0x14);
          puVar13 = (undefined4 *)(*(int *)(iVar8 + 0x1c) * 0x40 + *(int *)(iVar8 + 0x18));
          pfVar12 = (float *)(*(int *)(iVar8 + 0x10) * 0x20 + *(int *)(iVar8 + 0x24));
          param_1 = puVar18;
          local_20 = pfVar12;
        }
      }
      else {
        pfVar16 = local_1c;
        pfVar17 = pfVar12;
        for (iVar11 = 8; puVar18 = param_1, iVar11 != 0; iVar11 = iVar11 + -1) {
          *pfVar17 = *pfVar16;
          pfVar16 = pfVar16 + 1;
          pfVar17 = pfVar17 + 1;
        }
LAB_00575eaa:
        puVar15 = puVar18;
        if (((uVar9 == 1) || (pfVar12 == param_5)) ||
           (((pfVar12[4] - *param_5 < fVar2 != (pfVar12[4] - *param_5 == fVar2) &&
             (((param_5[4] - *pfVar12 < fVar2 != (param_5[4] - *pfVar12 == fVar2) &&
               (pfVar12[5] - param_5[1] < fVar2 != (pfVar12[5] - param_5[1] == fVar2))) &&
              (param_5[5] - pfVar12[1] < fVar2 != (param_5[5] - pfVar12[1] == fVar2))))) &&
            ((pfVar12[6] - param_5[2] < fVar2 != (pfVar12[6] - param_5[2] == fVar2) &&
             (param_5[6] - pfVar12[2] < fVar2 != (param_5[6] - pfVar12[2] == fVar2)))))))
        goto LAB_00575e25;
      }
      if (local_1c != (float *)0x0) {
        local_1c = local_1c + 8;
      }
      local_18 = local_18 + 1;
      p3b = p3b + 3;
    } while (local_18 < uVar9);
  }
  iVar10 = ((int)puVar18 - *(int *)(iVar8 + 0xc)) / 0x14;
  *(int *)(iVar8 + 0x10) = iVar10;
  *(int *)(iVar8 + 0x1c) = (int)puVar13 - *(int *)(iVar8 + 0x18) >> 6;
  return iVar10 - iVar1;
}
