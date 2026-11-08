// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e7420: best 37.9% (regs 29.2%), residual class c. structure only (2207 B). Ghidra artifact `&stack0x0 != 0x2c` replaced by `local_2c != 0` (UNVERIFIED, not the port body as is); orig frame 0x38 vs 0xc. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
void FUN_004e6d00(void);
void FUN_004e7cc0(void);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7200, 0x007d7200);
MGLOBAL(int, DAT_007d7230, 0x007d7230);
MGLOBAL(int, DAT_007d722c, 0x007d722c);
MGLOBAL(undefined4, DAT_0061867c, 0x0061867c);
void FUN_004e6d40(void);
void FUN_004c0740(int p0, int p1);
undefined4 FUN_004c0de0(int p0);
uint FUN_004cbd30(uint p0, void * p1, uint p2);
undefined4 FUN_004cc5e0(uint p0, int p1, void * p2, void * p3);
undefined4 FUN_004cc790(undefined4 p0, void * p1, undefined4 p2);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
int FUN_004d8060(void * p0, void * p1);
undefined4 * FUN_004d8480(void * p0);
int FUN_004e1b60(void * p0, int p1, int p2);
int FUN_004e6710(int p0, void (*p1)(void), undefined4 p2);
undefined4 FUN_004e6920(void * p0);
int FUN_004e6d80(void);
int * FUN_004e7010(int * p0);
undefined4 FUN_004e8ea0(int p0);
int FUN_004e9330(uint p0);
int FUN_005c4670(int p0);
undefined4 * FUN_005c47b0(void * p0);
int * FUN_005c47e0(undefined4 p0, void * p1);
int FUN_004e7060(undefined4 p0, void * p1, void * p2);
undefined4 FUN_004e6e00(int p0);
int FUN_004e66d0(int p0, void (*p1)(void), undefined4 p2);
int FUN_004e6760(int p0, void (*p1)(void), undefined4 p2);
uint FUN_004e4c30(int p0);
// MATCH: 0x004e7420 FUN_004e7420

int FUN_004e7420(int param_1)

{
  undefined4 *puVar1;
  int *piVar2;
  undefined4 *puVar3;
  int *piVar4;
  int *piVar5;
  undefined4 *puVar6;
  int iVar7;
  int iVar8;
  undefined4 uVar9;
  int iVar10;
  int iVar11;
  uint local_44;
  uint local_40;
  undefined1 local_3c [4];
  undefined1 local_38 [4];
  undefined4 local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  undefined4 *local_24;
  undefined4 local_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  undefined4 local_10;
  int local_c;
  int iStack_8;
  int iStack_4;
  
  iVar10 = param_1;
  iVar7 = FUN_004cc5e0(param_1,1,local_38,&local_40);
  if (iVar7 == 0) {
    local_24 = (undefined4 *)0x2;
    local_20 = FUN_004d7ff0(0x8000001a);
    FUN_004d8480(&local_24);
    return 0;
  }
  if ((local_40 < 0x35000) || (0x37002 < local_40)) {
    local_14 = 2;
    local_10 = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&local_14);
  }
  else {
    iVar7 = FUN_004cbd30(iVar10,&local_c,0xc);
    if (iVar7 != 0xc) {
      local_34 = 2;
      local_30 = FUN_004d7ff0(0x8000001a);
      FUN_004d8480(&local_34);
      return 0;
    }
    iVar7 = FUN_004e6d80();
    iVar11 = 0;
    if (iVar7 != 0) {
      iVar8 = FUN_004cc5e0(iVar10,0xe,0,local_3c);
      if (iVar8 == 0) {
        FUN_004d8060(&DAT_0061867c,(void *)iVar7);
        puVar3 = *(undefined4 **)(iVar7 + 8);
        do {
          if (puVar3 == (undefined4 *)(iVar7 + 8)) break;
          puVar6 = (undefined4 *)*puVar3;
          puVar1 = puVar3 + -0x10;
          FUN_004e6920(puVar1);
          puVar3 = puVar6;
        } while (puVar1 != (undefined4 *)0x0);
      }
      else {
        iVar8 = (int)FUN_005c47e0(iVar10,&local_24);
        if (iVar8 == 0) {
          FUN_004d8060(&DAT_0061867c,(void *)iVar7);
          puVar3 = *(undefined4 **)(iVar7 + 8);
          do {
            if (puVar3 == (undefined4 *)(iVar7 + 8)) break;
            puVar6 = (undefined4 *)*puVar3;
            puVar1 = puVar3 + -0x10;
            FUN_004e6920(puVar1);
            puVar3 = puVar6;
          } while (puVar1 != (undefined4 *)0x0);
          FUN_004e6760(iVar7,FUN_004e6d00,0);
          FUN_004e6710(iVar7,FUN_004e6d40,0);
          if (*(int *)(iVar7 + 4) != 0) {
            FUN_004c0de0(*(int *)(iVar7 + 4));
          }
          (**(void (**)(int,int))(DAT_007d3ff8 + 0x11c))
                    (*(undefined4 *)(DAT_007d7200 + 4 + DAT_007d3ff8),iVar7);
          local_34 = 2;
          local_30 = FUN_004d7ff0(0x8000001a);
          FUN_004d8480(&local_34);
          return 0;
        }
        *(undefined4 *)(iVar7 + 4) = *local_24;
        iVar8 = FUN_004cc5e0(iVar10,0x1a,0,local_3c);
        if (iVar8 != 0) {
          iVar8 = FUN_004cc5e0(iVar10,1,&local_34,&local_44);
          if (iVar8 != 0) {
            if ((local_44 < 0x35000) || (0x37002 < local_44)) {
              local_14 = 2;
              local_10 = FUN_004d7ff0(0x80000004);
              FUN_004d8480(&local_14);
            }
            else {
              iVar8 = FUN_004cbd30(iVar10,&param_1,4);
              if (iVar8 == 4) {
                local_28 = 0;
                if (param_1 < 1) {
                  local_2c = 0;
                }
                else {
                  local_2c = (**(int (**)(int,int))(DAT_007d3ff8 + 0x108))(param_1 * 4,0x3000f);
                  if (local_2c == 0) {
                    uStack_1c = 2;
                    uStack_18 = FUN_004d7ff0(0x80000013,param_1 * 4);
                    FUN_004d8480(&uStack_1c);
                    goto LAB_004e77de;
                  }
                }
                if (0 < param_1) {
                  do {
                    iVar8 = FUN_004cc5e0(iVar10,0xf,0,&local_44);
                    if (iVar8 == 0) {
                      iVar10 = 0;
                      if (0 < local_28) {
                        do {
                          FUN_004e8ea0(*(undefined4 *)(local_2c + iVar10 * 4));
                          iVar10 = iVar10 + 1;
                        } while (iVar10 < local_28);
                      }
                      if (local_2c != 0) {
                        (**(void (**)(int))(DAT_007d3ff8 + 0x10c))(local_2c);
                        local_2c = 0;
                      }
                      goto LAB_004e77de;
                    }
                    if ((local_44 < 0x35000) || (0x37002 < local_44)) {
                      FUN_004e7010(&local_2c);
                      goto LAB_004e77de;
                    }
                    uVar9 = FUN_004e9330(iVar10);
                    *(undefined4 *)(local_2c + iVar11 * 4) = uVar9;
                    if (*(int *)(local_2c + iVar11 * 4) == 0) {
                      FUN_004e7010(&local_2c);
                      goto LAB_004e77de;
                    }
                    local_28 = local_28 + 1;
                    iVar11 = iVar11 + 1;
                  } while (iVar11 < param_1);
                }
                if (local_2c != 0) {
                  iVar11 = 0;
                  if (0 < local_c) {
                    do {
                      iVar8 = FUN_004cc5e0(iVar10,0x14,0,&local_40);
                      if ((iVar8 == 0) ||
                         (iVar8 = FUN_004e7060(iVar10,&local_24,&local_2c), iVar8 == 0)) {
                        iVar10 = 0;
                        if (0 < local_28) {
                          do {
                            FUN_004e8ea0(*(undefined4 *)(local_2c + iVar10 * 4));
                            iVar10 = iVar10 + 1;
                          } while (iVar10 < local_28);
                        }
                        goto LAB_004e7bd5;
                      }
                      uVar9 = *(undefined4 *)(iVar7 + 8);
                      piVar2 = (int *)(iVar7 + 8);
                      puVar3 = (undefined4 *)(iVar8 + 0x40);
                      *(int **)(iVar8 + 0x44) = piVar2;
                      iVar11 = iVar11 + 1;
                      *puVar3 = uVar9;
                      *(undefined4 **)(*piVar2 + 4) = puVar3;
                      *piVar2 = (int)puVar3;
                      *(int *)(iVar8 + 0x3c) = iVar7;
                    } while (iVar11 < local_c);
                  }
                  iVar11 = 0;
                  if (0 < iStack_8) {
                    do {
                      iVar8 = FUN_004cc5e0(iVar10,1,0,0);
                      if ((((iVar8 == 0) || (iVar8 = FUN_004cc790(iVar10,&param_1,4), iVar8 == 0))
                          || (iVar8 = FUN_004cc5e0(iVar10,0x12,0,0), iVar8 == 0)) ||
                         (iVar8 = FUN_004e4c30(iVar10), iVar8 == 0)) {
                        iVar10 = 0;
                        if (0 < local_28) {
                          do {
                            FUN_004e8ea0(*(undefined4 *)(local_2c + iVar10 * 4));
                            iVar10 = iVar10 + 1;
                          } while (iVar10 < local_28);
                        }
                        goto LAB_004e7bd5;
                      }
                      FUN_004c0740(iVar8,local_24[param_1]);
                      piVar2 = (int *)(iVar7 + 0x10);
                      iVar11 = iVar11 + 1;
                      piVar5 = (int *)(iVar8 + DAT_007d7230);
                      iVar8 = *piVar2;
                      piVar4 = piVar5 + 1;
                      piVar5[2] = (int)piVar2;
                      *piVar4 = iVar8;
                      *(int **)(*piVar2 + 4) = piVar4;
                      *piVar2 = (int)piVar4;
                      *piVar5 = iVar7;
                    } while (iVar11 < iStack_8);
                  }
                  iVar11 = 0;
                  if (0 < iStack_4) {
                    do {
                      iVar8 = FUN_004cc5e0(iVar10,1,0,0);
                      if (((iVar8 == 0) || (iVar8 = FUN_004cc790(iVar10,&param_1,4), iVar8 == 0)) ||
                         ((iVar8 = FUN_004cc5e0(iVar10,5,0,0), iVar8 == 0 ||
                          (iVar8 = FUN_005c4670(iVar10), iVar8 == 0)))) {
                        iVar10 = 0;
                        if (0 < local_28) {
                          do {
                            FUN_004e8ea0(*(undefined4 *)(local_2c + iVar10 * 4));
                            iVar10 = iVar10 + 1;
                          } while (iVar10 < local_28);
                        }
LAB_004e7bd5:
                        if (local_2c != 0) {
                          (**(void (**)(int))(DAT_007d3ff8 + 0x10c))(local_2c);
                          local_2c = 0;
                        }
                        FUN_005c47b0(&local_24);
                        FUN_004d8060(&DAT_0061867c,(void *)iVar7);
                        FUN_004e66d0(iVar7,FUN_004e7cc0,0);
                        FUN_004e6760(iVar7,FUN_004e6d00,0);
                        FUN_004e6710(iVar7,FUN_004e6d40,0);
                        if (*(int *)(iVar7 + 4) != 0) {
                          FUN_004c0de0(*(int *)(iVar7 + 4));
                        }
                        (**(void (**)(int,int))(DAT_007d3ff8 + 0x11c))
                                  (*(undefined4 *)(DAT_007d7200 + 4 + DAT_007d3ff8),iVar7);
                        local_14 = 2;
                        local_10 = FUN_004d7ff0(0x8000001a);
                        FUN_004d8480(&local_14);
                        return 0;
                      }
                      FUN_004c0740(iVar8,local_24[param_1]);
                      piVar2 = (int *)(iVar7 + 0x18);
                      iVar11 = iVar11 + 1;
                      piVar5 = (int *)(iVar8 + DAT_007d722c);
                      iVar8 = *piVar2;
                      piVar4 = piVar5 + 1;
                      piVar5[2] = (int)piVar2;
                      *piVar4 = iVar8;
                      *(int **)(*piVar2 + 4) = piVar4;
                      *piVar2 = (int)piVar4;
                      *piVar5 = iVar7;
                    } while (iVar11 < iStack_4);
                  }
                  iVar11 = 0;
                  if (0 < local_28) {
                    do {
                      FUN_004e8ea0(*(undefined4 *)(local_2c + iVar11 * 4));
                      iVar11 = iVar11 + 1;
                    } while (iVar11 < local_28);
                  }
                  if (local_2c != 0) {
                    (**(void (**)(int))(DAT_007d3ff8 + 0x10c))(local_2c);
                    local_2c = 0;
                  }
                  FUN_005c47b0(&local_24);
                  iVar10 = FUN_004e1b60(&DAT_0061867c,iVar10,iVar7);
                  if (iVar10 == 0) {
                    FUN_004e6e00(iVar7);
                    local_14 = 2;
                    local_10 = FUN_004d7ff0(0x8000001a);
                    FUN_004d8480(&local_14);
                    return 0;
                  }
                  return iVar7;
                }
              }
            }
          }
LAB_004e77de:
          FUN_005c47b0(&local_24);
          FUN_004d8060(&DAT_0061867c,(void *)iVar7);
          FUN_004e66d0(iVar7,FUN_004e7cc0,0);
          FUN_004e6760(iVar7,FUN_004e6d00,0);
          FUN_004e6710(iVar7,FUN_004e6d40,0);
          if (*(int *)(iVar7 + 4) != 0) {
            FUN_004c0de0(*(int *)(iVar7 + 4));
          }
          (**(void (**)(int,int))(DAT_007d3ff8 + 0x11c))
                    (*(undefined4 *)(DAT_007d7200 + 4 + DAT_007d3ff8),iVar7);
          local_14 = 2;
          local_10 = FUN_004d7ff0(0x8000001a);
          FUN_004d8480(&local_14);
          return 0;
        }
        FUN_005c47b0(&local_24);
        FUN_004d8060(&DAT_0061867c,(void *)iVar7);
        puVar3 = *(undefined4 **)(iVar7 + 8);
        do {
          if (puVar3 == (undefined4 *)(iVar7 + 8)) break;
          puVar6 = (undefined4 *)*puVar3;
          puVar1 = puVar3 + -0x10;
          FUN_004e6920(puVar1);
          puVar3 = puVar6;
        } while (puVar1 != (undefined4 *)0x0);
      }
      FUN_004e6760(iVar7,FUN_004e6d00,0);
      FUN_004e6710(iVar7,FUN_004e6d40,0);
      if (*(int *)(iVar7 + 4) != 0) {
        FUN_004c0de0(*(int *)(iVar7 + 4));
      }
      (**(void (**)(int,int))(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d7200 + 4 + DAT_007d3ff8),iVar7);
      local_34 = 2;
      local_30 = FUN_004d7ff0(0x8000001a);
      FUN_004d8480(&local_34);
      return 0;
    }
  }
  return 0;
}
