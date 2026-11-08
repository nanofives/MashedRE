// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e2930: raw converter body (Ghidra structure, gotos kept), comments stripped, prototypes loosened for C++ (void-pointer or int params), F1/F2 typedefs for the engine vtable calls. Score 34.3% structure only (class b: source shape not found, size above budget). */
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
uint FUN_004d4250(int p0);
undefined4 FUN_004d7ff0(undefined4 p0);
void FUN_004d8480(void * p0);
// MATCH: 0x004e2930 FUN_004e2930
undefined4 FUN_004e2930(int param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  uint *puVar6;
  int *piVar7;
  int *piVar8;
  uint uVar9;
  int iVar10;
  int iVar11;
  uint uVar12;
  int local_18;
  uint local_14;
  uint local_10;
  int local_c;
  int local_8;
  undefined4 local_4;
  
  local_c = *(int *)(param_1 + 4);
  piVar8 = (int *)(*(int *)(param_1 + 8) + 0x1c);
  do {
    uVar12 = 0;
    if (**(int **)(*piVar8 + 8) != 0) {
      iVar10 = 0;
      do {
        if (uVar12 < **(uint **)(*piVar8 + 8)) {
          iVar11 = (*(uint **)(*piVar8 + 8))[3] + iVar10;
        }
        else {
          iVar11 = 0;
        }
        if (*(int *)(iVar11 + 0xc) == 0) {
          piVar5 = (int *)FUN_004d4250(0x10);
          if (piVar5 == (int *)0x0) {
            piVar5 = (int *)0x0;
          }
          else {
            piVar5[1] = 0;
            *piVar5 = 0;
            piVar5[3] = 0;
            piVar5[2] = *param_2;
            *param_2 = (int)piVar5;
          }
          *(int **)(iVar11 + 0xc) = piVar5;
          if (piVar5 == (int *)0x0) {
            local_8 = 1;
            local_4 = FUN_004d7ff0(0x20);
            FUN_004d8480(&local_8);
            return 0x20;
          }
          *(int *)(iVar11 + 0x10) = *piVar5;
          *piVar5 = iVar11;
        }
        uVar12 = uVar12 + 1;
        iVar10 = iVar10 + 0x24;
      } while (uVar12 < **(uint **)(*piVar8 + 8));
    }
    local_10 = 0;
    if (piVar8[-6] != 0) {
      do {
        iVar10 = *(int *)(piVar8[-5] + local_10 * 4);
        if (iVar10 != -1) {
          local_14 = 0;
          local_8 = *(int *)(param_1 + 8) + iVar10 * 0x28;
          puVar6 = *(uint **)(*(int *)(local_8 + 0x1c) + 8);
          if (*puVar6 != 0) {
            local_18 = 0;
            if (*puVar6 == 0) {
              piVar5 = (int *)0x0;
              goto LAB_004e2a16;
            }
            do {
              piVar5 = (int *)(puVar6[3] + local_18);
LAB_004e2a16:
              if (piVar5[1] != 0) {
                uVar12 = **(uint **)(*piVar8 + 8);
                if (uVar12 != 0) {
                  piVar7 = (int *)(*(uint **)(*piVar8 + 8))[3];
                  uVar9 = 0;
                  if (uVar12 != 0) {
                    do {
                      if (*piVar7 == *piVar5) {
                        if ((1 << ((byte)local_10 & 0x1f) & piVar7[6]) != 0) {
                          iVar10 = piVar5[3];
                          if (iVar10 == 0) {
                            piVar1 = (int *)piVar7[3];
                            piVar5[4] = *piVar1;
                            *piVar1 = (int)piVar5;
                            piVar5[3] = piVar7[3];
                          }
                          else {
                            for (iVar11 = *(int *)(iVar10 + 0xc); iVar11 != 0;
                                iVar11 = *(int *)(iVar11 + 0xc)) {
                              iVar10 = iVar11;
                            }
                            iVar11 = piVar7[3];
                            for (iVar2 = *(int *)(piVar7[3] + 0xc); iVar2 != 0;
                                iVar2 = *(int *)(iVar2 + 0xc)) {
                              iVar11 = iVar2;
                            }
                            if (iVar10 != iVar11) {
                              iVar4 = *(int *)(iVar10 + 4);
                              iVar2 = iVar10;
                              while (iVar3 = iVar4, iVar3 != 0) {
                                iVar2 = iVar3;
                                iVar4 = *(int *)(iVar3 + 4);
                              }
                              *(int *)(iVar2 + 4) = iVar11;
                              *(int *)(iVar11 + 0xc) = iVar10;
                              piVar5 = param_2;
                              for (iVar10 = *param_2; iVar10 != iVar11;
                                  iVar10 = *(int *)(iVar10 + 8)) {
                                piVar5 = (int *)(iVar10 + 8);
                              }
                              *piVar5 = *(int *)(*piVar5 + 8);
                            }
                          }
                        }
                        break;
                      }
                      piVar7 = piVar7 + 9;
                      uVar9 = uVar9 + 1;
                    } while (uVar9 < uVar12);
                  }
                }
              }
              local_14 = local_14 + 1;
              local_18 = local_18 + 0x24;
              puVar6 = *(uint **)(*(int *)(local_8 + 0x1c) + 8);
            } while (local_14 < *puVar6);
          }
        }
        local_10 = local_10 + 1;
      } while (local_10 < (uint)piVar8[-6]);
    }
    piVar8 = piVar8 + 10;
    local_c = local_c + -1;
    if (local_c == 0) {
      return 0;
    }
  } while( true );
}
