// FLAGS: /O2 /GS /Ob1
// h2: 23.8% via match_permute under /O2 /GS /Ob1 (1544 compiles; Ghidra form 15.7%). 730 B nested-search function with goto labels from Ghidra, not restructured (budget). Class (b).
/* MATCH-RWL02 0x004e2650: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl02_20261107/results.tsv. */
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
typedef unsigned char byte;
#ifndef __cplusplus
#define true 1
#endif
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004e2650 FUN_004e2650
undefined4 FUN_004e2650(int param_1)

{
  int iVar1;
  int iVar2;
  uint *puVar3;
  int *piVar4;
  uint *puVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int *piVar9;
  uint uVar10;
  int *piVar11;
  uint uVar12;
  int local_20;
  uint local_1c;
  int local_18;
  uint local_10;
  int local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  local_c = *(int *)(param_1 + 4);
  piVar9 = *(int **)(param_1 + 8);
  do {
    iVar1 = *piVar9;
    uVar8 = 0;
    puVar5 = (uint *)(iVar1 + 0x20);
    if (*puVar5 != 0) {
      iVar7 = 0;
      do {
        if (*(int *)(iVar7 + 4 + *(int *)(iVar1 + 0x24)) != 0) {
          iVar2 = *(int *)(iVar7 + *(int *)(iVar1 + 0x24));
          uVar12 = *puVar3;
          puVar3 = *(uint **)(piVar9[7] + 8);
          if (uVar12 != 0) {
            piVar11 = (int *)puVar3[3];
            uVar10 = 0;
            if (uVar12 != 0) {
              do {
                if (*piVar11 == iVar2) goto LAB_004e26e8;
                piVar11 = piVar11 + 9;
                uVar10 = uVar10 + 1;
              } while (uVar10 < uVar12);
            }
          }
          piVar11 = (int *)(puVar3[3] + uVar12 * 0x24);
          *puVar3 = uVar12 + 1;
          piVar11[5] = (int)puVar3;
          *piVar11 = iVar2;
          piVar11[1] = 0;
          piVar11[2] = 1;
          piVar11[4] = 0;
          piVar11[3] = 0;
          piVar11[6] = 0;
          piVar11[7] = -1;
          piVar11[8] = (int)piVar9;
LAB_004e26e8:
          if (piVar11 == (int *)0x0) {
            local_8 = 1;
            local_4 = FUN_004d7ff0(0x20);
            FUN_004d8480(&local_8);
            return 0x20;
          }
        }
        uVar8 = uVar8 + 1;
        iVar7 = iVar7 + 0xc;
      } while (uVar8 < *puVar5);
    }
    local_1c = 0;
    if (piVar9[1] != 0) {
      local_18 = 0;
      do {
        iVar7 = *(int *)(piVar9[2] + local_1c * 4);
        if (iVar7 != -1) {
          iVar7 = *(int *)(param_1 + 8) + iVar7 * 0x28;
          iVar2 = *(int *)(*piVar9 + 0x30) + local_18;
          puVar3 = *(uint **)(*(int *)8 + (iVar7 + 0x1c));
          local_10 = 0;
          if (*puVar3 != 0) {
            local_20 = 0;
            if (*puVar3 == 0) {
              piVar11 = (int *)0x0;
              goto LAB_004e277c;
            }
            do {
              piVar11 = (int *)(puVar3[3] + local_20);
LAB_004e277c:
              uVar8 = 0;
              if (*puVar5 == 0) {
LAB_004e27a9:
                iVar6 = *(int *)(iVar2 + 8);
              }
              else {
                piVar4 = *(int **)(iVar1 + 0x24);
                do {
                  if (*piVar4 == *piVar11) {
                    if (uVar8 == 0xffffffff) goto LAB_004e27a9;
                    iVar6 = *(int *)(*(int *)(iVar2 + 4) + uVar8 * 4);
                    goto LAB_004e27bc;
                  }
                  uVar8 = uVar8 + 1;
                  piVar4 = piVar4 + 3;
                } while (uVar8 < *puVar5);
                iVar6 = *(int *)(iVar2 + 8);
              }
LAB_004e27bc:
              if (iVar6 != 2) {
                if (!(iVar6 == 0)) {
                  puVar3 = *(uint **)(piVar9[7] + 8);
                  iVar6 = *piVar11;
                  uVar8 = *puVar3;
                  if (uVar8 != 0) {
                    piVar4 = (int *)puVar3[3];
                    uVar12 = 0;
                    if (uVar8 != 0) {
                      do {
                        if (*piVar4 == iVar6) goto LAB_004e281e;
                        piVar4 = piVar4 + 9;
                        uVar12 = uVar12 + 1;
                      } while (uVar12 < uVar8);
                    }
                  }
                  piVar4 = (int *)(puVar3[3] + uVar8 * 0x24);
                  *puVar3 = uVar8 + 1;
                  *piVar4 = iVar6;
                  piVar4[1] = 0;
                  piVar4[2] = 1;
                  piVar4[4] = 0;
                  piVar4[5] = (int)puVar3;
                  piVar4[6] = 0;
                  piVar4[7] = -1;
                  piVar4[8] = (int)piVar9;
                  piVar4[3] = 0;
LAB_004e281e:
                  if (piVar4 == (int *)0x0) {
                    local_8 = 1;
                    local_4 = FUN_004d7ff0(0x20);
                    FUN_004d8480(&local_8);
                    return 0x20;
                  }
LAB_004e2826:
                  piVar4[6] = piVar4[6] | 1 << ((byte)local_1c & 0x1f);
                } else {
                  puVar3 = *(uint **)(piVar9[7] + 8);
                  uVar8 = *puVar3;
                  if (uVar8 != 0) {
                    piVar4 = (int *)puVar3[3];
                    uVar12 = 0;
                    if (uVar8 != 0) {
                      do {
                        if (*piVar4 == *piVar11) goto LAB_004e2826;
                        piVar4 = piVar4 + 9;
                        uVar12 = uVar12 + 1;
                      } while (uVar12 < uVar8);
                    }
                  }
                }
              }
              local_20 = local_20 + 0x24;
              local_10 = local_10 + 1;
              puVar3 = *(uint **)(*(int *)(iVar7 + 0x1c) + 8);
            } while (local_10 < *puVar3);
          }
        }
        local_1c = local_1c + 1;
        local_18 = local_18 + 0xc;
      } while (local_1c < (uint)piVar9[1]);
    }
    piVar9 = piVar9 + 10;
    local_c = local_c + -1;
    if (local_c == 0) {
      return 0;
    }
  } while( true );
}
