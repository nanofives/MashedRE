// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e2bf0: raw converter body (Ghidra structure, gotos kept), comments stripped, prototypes loosened for C++ (void-pointer or int params), F1/F2 typedefs for the engine vtable calls. Score 38.2% structure only (class b: source shape not found, size above budget). */
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
#ifndef __cplusplus
#define true 1
#endif
uint FUN_004d4250(int p0);
int FUN_004d42b0(int p0);
void FUN_004e3160(undefined4 * p0, uint p1, uint p2, int p3, uint p4, uint p5);
// MATCH: 0x004e2bf0 FUN_004e2bf0
undefined4 FUN_004e2bf0(int param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int *piVar4;
  int *piVar5;
  undefined4 uVar6;
  uint *puVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  uint uVar10;
  uint uVar11;
  int *piVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  uint uVar16;
  uint local_14;
  int local_10;
  uint local_c;
  int local_8;
  
  puVar9 = (undefined4 *)param_2;
  iVar1 = param_1;
  iVar13 = 0;
  uVar10 = 0;
  local_8 = 0;
  puVar7 = *(uint **)(*(int *)(*(int *)(param_1 + 8) + 0x1c) + 8);
  uVar14 = *puVar7;
  if (uVar14 != 0) {
    iVar15 = 0;
    if (uVar14 == 0) {
      iVar2 = 0;
      goto LAB_004e2c21;
    }
    do {
      iVar2 = puVar7[3] + iVar15;
LAB_004e2c21:;
      if ((*(int *)(iVar2 + 4) == 1) || (*(int *)(iVar2 + 4) == 2)) {
        iVar13 = iVar13 + 1;
      }
      uVar10 = uVar10 + 1;
      iVar15 = iVar15 + 0x24;
      local_8 = iVar13;
    } while (uVar10 < uVar14);
  }
  uVar14 = 0;
  for (puVar8 = (undefined4 *)param_2; puVar8 != (undefined4 *)0x0; puVar8 = (undefined4 *)puVar8[2]) {
    if ((undefined4 *)param_2 != puVar8) {
      puVar3 = (undefined4 *)param_2;
      do {
        if (*(int *)*puVar3 == *(int *)*puVar8) {
          if (puVar3 != puVar8) goto LAB_004e2c69;
          break;
        }
        puVar3 = (undefined4 *)puVar3[2];
      } while (puVar3 != puVar8);
    }
    uVar14 = uVar14 + 1;
LAB_004e2c69:;
  }
  iVar13 = FUN_004d4250(0);
  puVar3 = (undefined4 *)FUN_004d42b0(0);
  puVar8 = puVar3;
  for (uVar10 = (uint)(iVar13 - (int)puVar3) >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
    *puVar8 = 0;
    puVar8 = puVar8 + 1;
  }
  for (uVar10 = iVar13 - (int)puVar3 & 3; uVar10 != 0; uVar10 = uVar10 - 1) {
    *(undefined1 *)puVar8 = 0;
    puVar8 = (undefined4 *)((int)puVar8 + 1);
  }
  iVar13 = FUN_004d42b0(*(int *)(param_1 + 0xc) * 0x1c + 0x14);
  *(int *)(param_1 + 0x14) = iVar13;
  *(undefined2 *)(iVar13 + 2) = *(undefined2 *)(param_1 + 0xc);
  *(int *)(*(int *)(param_1 + 0x14) + 4) = param_1;
  *(undefined4 *)(param_1 + 0x10) = 0;
  param_2 = (int *)0x0;
  puVar8 = puVar9;
  do {
    if (puVar8 == (undefined4 *)0x0) {
      local_c = 0;
      if (*(int *)(param_1 + 4) != 0) {
        local_10 = 0;
        do {
          piVar4 = (int *)(*(int *)(param_1 + 8) + local_10);
          if (*(int *)(param_1 + 0xc) != 0) {
            iVar13 = FUN_004d42b0(*(int *)(param_1 + 0xc) << 2);
            piVar4[3] = iVar13;
          }
          iVar13 = FUN_004d42b0(*(int *)(param_1 + 0xc) * 4 + 4);
          piVar4[4] = iVar13;
          if (*(int *)(*piVar4 + 0x20) != 0) {
            iVar13 = FUN_004d42b0(*(int *)(*piVar4 + 0x20) * 4);
            piVar4[6] = iVar13;
          }
          if (*(int *)(*piVar4 + 0x34) != 0) {
            iVar13 = FUN_004d42b0(*(int *)(*piVar4 + 0x34) + 3U & 0xfffffffc);
            piVar4[5] = iVar13;
          }
          uVar11 = 0;
          uVar16 = 0xffffffff;
          puVar7 = *(uint **)(piVar4[7] + 8);
          uVar10 = *puVar7;
          if (uVar10 != 0) {
            local_14 = 0;
            do {
              piVar12 = (int *)0x0;
              if (uVar11 < uVar10) {
                piVar5 = (int *)(puVar7[3] + local_14);
              }
              else {
                piVar5 = (int *)0x0;
              }
              uVar10 = 0;
              if (uVar14 != 0) {
                do {
                  if (param_2[uVar10 * 2] == *piVar5) {
                    piVar12 = param_2 + uVar10 * 2;
                    goto LAB_004e2df6;
                  }
                  uVar10 = uVar10 + 1;
                } while (uVar10 < uVar14);
                piVar12 = (int *)0x0;
              }
LAB_004e2df6:;
              *(int **)(piVar4[3] + piVar5[7] * 4) = piVar12;
              *(int *)(piVar4[4] + 4 + piVar5[7] * 4) = piVar5[6];
              uVar16 = uVar16 & piVar5[6];
              uVar11 = uVar11 + 1;
              puVar7 = *(uint **)(piVar4[7] + 8);
              local_14 = local_14 + 0x24;
              uVar10 = *puVar7;
            } while (uVar11 < uVar10);
          }
          local_14 = 0;
          *(uint *)piVar4[4] = uVar16;
          if (*(int *)(*piVar4 + 0x20) != 0) {
            iVar13 = 0;
            do {
              *(undefined4 *)(piVar4[6] + local_14 * 4) = 0xffffffff;
              uVar10 = 0;
              if (*(uint *)(param_1 + 0xc) != 0) {
                piVar12 = (int *)piVar4[3];
                do {
                  if (((int *)*piVar12 != (int *)0x0) &&
                     (*(int *)*piVar12 == *(int *)(*(int *)(*piVar4 + 0x24) + iVar13))) {
                    *(uint *)(piVar4[6] + local_14 * 4) = uVar10;
                    break;
                  }
                  uVar10 = uVar10 + 1;
                  piVar12 = piVar12 + 1;
                } while (uVar10 < *(uint *)(param_1 + 0xc));
              }
              local_14 = local_14 + 1;
              iVar13 = iVar13 + 0xc;
            } while (local_14 < *(uint *)(*piVar4 + 0x20));
          }
          local_c = local_c + 1;
          local_10 = local_10 + 0x28;
        } while (local_c < *(uint *)(param_1 + 4));
      }
      if (local_8 != 0) {
        uVar6 = FUN_004d42b0(local_8 * 0xc);
        *(undefined4 *)(param_1 + 0x1c) = uVar6;
      }
      iVar13 = *(int *)(param_1 + 8);
      iVar15 = 0;
      uVar14 = 0;
      puVar7 = *(uint **)(*(int *)(iVar13 + 0x1c) + 8);
      if (*puVar7 != 0) {
        iVar2 = 0;
        param_1 = 0;
        if (*puVar7 == 0) {
          puVar9 = (undefined4 *)0x0;
          goto LAB_004e2f00;
        }
        do {
          puVar9 = (undefined4 *)(puVar7[3] + param_1);
LAB_004e2f00:;
          if ((puVar9[1] == 1) || (puVar9[1] == 2)) {
            puVar8 = (undefined4 *)(*(int *)(iVar1 + 0x1c) + iVar2);
            iVar15 = iVar15 + 1;
            iVar2 = iVar2 + 0xc;
            *puVar8 = *puVar9;
            puVar8[1] = puVar9[1];
            puVar8[2] = puVar9[7];
          }
          uVar14 = uVar14 + 1;
          param_1 = param_1 + 0x24;
          puVar7 = *(uint **)(*(int *)(iVar13 + 0x1c) + 8);
        } while (uVar14 < *puVar7);
      }
      *(int *)(iVar1 + 0x18) = iVar15;
      FUN_004e3160(*(undefined4 **)(iVar1 + 0x1c),iVar15,0xc,0,0,0xffffffff);
      return 0;
    }
    if (puVar9 != puVar8) {
      puVar3 = puVar9;
      do {
        if (*(int *)*puVar3 == *(int *)*puVar8) {
          if (puVar3 != puVar8) goto LAB_004e2d1e;
          break;
        }
        puVar3 = (undefined4 *)puVar3[2];
      } while (puVar3 != puVar8);
    }
    iVar13 = *(int *)*puVar8;
    piVar4 = (int *)FUN_004d42b0(8);
    if (param_2 == (int *)0x0) {
      param_2 = piVar4;
    }
    *piVar4 = iVar13;
    piVar4[1] = *(int *)(iVar13 + 8);
LAB_004e2d1e:;
    puVar8 = (undefined4 *)puVar8[2];
  } while( true );
}
