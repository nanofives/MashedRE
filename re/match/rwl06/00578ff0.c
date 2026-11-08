// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h3 0x00578ff0: best 13.8% (regs 30.6%, ops 66.0%), class (b). compiles (earlier cast fixes); raw Ghidra form; structure only. */
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
typedef unsigned char byte;
typedef long double float10;
MGLOBAL(undefined1, DAT_005cc9b4, 0x005cc9b4);
MGLOBAL(undefined1, DAT_005cc328, 0x005cc328);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005cc558, 0x005cc558);
MGLOBAL(float, DAT_005ceabc, 0x005ceabc);
float10 FUN_005667c0(float * p0, float * p1);
undefined4 FUN_00579b50(int p0, int p1, float p2);
MGLOBAL(float, _DAT_005cc9b4, 0x005cc9b4);
MGLOBAL(float, _DAT_005cc328, 0x005cc328);
MGLOBAL(float, _DAT_005cc320, 0x005cc320);
MGLOBAL(float, _DAT_005cc558, 0x005cc558);
double fabs(double);
#pragma intrinsic(fabs)
// MATCH: 0x00578ff0 FUN_00578ff0
/* [C1 2026-05-20] Signature recovered: `uint FUN_00578ff0(Manifold *raw_manifolds, uint raw_count,
   int enable_merge,
 */

uint FUN_00578ff0(undefined4 *param_1,uint param_2,int param_3,undefined4 *param_4,uint param_5,
                 float param_6)

{
  undefined4 uVar1;
  float fVar2;
  uint *puVar3;
  uint **ppuVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int *piVar9;
  float *pfVar10;
  float *pfVar11;
  uint *puVar12;
  undefined4 *puVar13;
  float *pfVar14;
  undefined4 *puVar15;
  float fVar16;
  float fVar17;
  uint local_bc;
  float local_b4;
  float *local_b0;
  float local_ac;
  float local_a8;
  float local_a4;
  uint local_a0;
  float *local_9c;
  float *local_98;
  uint local_94;
  float *local_90;
  float local_8c;
  float local_88;
  float local_84;
  float local_7c;
  float local_78;
  float local_74;
  int local_70;
  uint local_6c;
  float local_68;
  float local_64;
  undefined4 *local_60;
  float local_5c;
  float local_58;
  float local_54;
  float local_50;
  undefined4 uStack_4c;
  uint local_48;
  undefined4 uStack_44;
  uint *local_40 [16];
  
  local_b0 = (float *)0x0;
  local_90 = (float *)0x0;
  local_98 = (float *)0x0;
  local_9c = (float *)0x0;
  if (param_2 == 0) {
    return 0;
  }
  if (param_5 == 0) {
    return 0;
  }
  if (param_2 == 1) {
    puVar13 = param_1;
    if (param_1[0x2b] == 0) {
      return 0;
    }
    goto LAB_005793b0;
  }
  if (param_3 != 0) {
    iVar6 = 0x10;
    if (param_2 < 5) {
      puVar12 = (uint *)0x0;
      uVar5 = 0;
      local_bc = 1;
      local_40[0] = (uint *)0x0;
      if (param_2 != 0) {
        puVar3 = param_1 + 0x37;
        uVar8 = param_2;
        do {
          if (puVar3[-0xc] != 0) {
            *puVar3 = (uint)puVar12;
            puVar12 = puVar3 + -0x37;
            uVar5 = uVar5 + 1;
          }
          puVar3 = puVar3 + 0x38;
          uVar8 = uVar8 - 1;
        } while (uVar8 != 0);
        local_40[0] = puVar12;
        if (1 < uVar5) goto LAB_00579108;
      }
    }
    else {
      local_bc = 0x10;
      ppuVar4 = local_40;
      for (; iVar6 != 0; iVar6 = iVar6 + -1) {
        *ppuVar4 = (uint *)0x0;
        ppuVar4 = ppuVar4 + 1;
      }
      if (param_2 != 0) {
        puVar12 = param_1 + 0xb;
        uVar5 = param_2;
        do {
          if (puVar12[0x20] != 0) {
            puVar12[0x2c] = (uint)local_40[*puVar12 & 0xf];
            local_40[*puVar12 & 0xf] = puVar12 + -0xb;
          }
          puVar12 = puVar12 + 0x38;
          uVar5 = uVar5 - 1;
        } while (uVar5 != 0);
      }
LAB_00579108:
      puVar12 = local_40[0];
      uVar5 = 0;
      if (local_bc != 0) {
        do {
          for (puVar3 = local_40[uVar5]; puVar3 != (uint *)0x0; puVar3 = (uint *)puVar3[0x37]) {
            for (uVar8 = puVar3[0x37]; uVar8 != 0; uVar8 = *(uint *)(uVar8 + 0xdc)) {
              if ((*(byte *)((int)puVar3 + 0xcd) == 2) && (*(byte *)(uVar8 + 0xcd) < 2)) {
                if (((puVar3[0xb] == *(uint *)(uVar8 + 0x2c)) &&
                    ((float)puVar3[6] < *(float *)(uVar8 + 0x18))) &&
                   (iVar6 = FUN_00579b50((int)puVar3,(int)uVar8,param_6), iVar6 != 0)) {
                  *(undefined4 *)(uVar8 + 0xac) = 0;
                }
              }
              else if (((*(byte *)(uVar8 + 0xcd) == 2) &&
                       (((*(byte *)((int)puVar3 + 0xcd) < 2 &&
                         (puVar3[0xb] == *(uint *)(uVar8 + 0x2c))) &&
                        (*(float *)(uVar8 + 0x18) < (float)puVar3[6])))) &&
                      (iVar6 = FUN_00579b50((int)uVar8,(int)puVar3,param_6), iVar6 != 0)) {
                puVar3[0x2b] = 0;
                break;
              }
            }
          }
          uVar5 = uVar5 + 1;
        } while (uVar5 < local_bc);
      }
      if (local_bc == 1) {
        ppuVar4 = local_40;
        while (puVar12 != (uint *)0x0) {
          puVar12 = *ppuVar4;
          if (puVar12[0x2b] == 0) {
            *ppuVar4 = (uint *)puVar12[0x37];
          }
          else {
            ppuVar4 = (uint **)(puVar12 + 0x37);
          }
          puVar12 = *ppuVar4;
        }
      }
      else {
        ppuVar4 = local_40;
        for (iVar6 = 0x10; iVar6 != 0; iVar6 = iVar6 + -1) {
          *ppuVar4 = (uint *)0x0;
          ppuVar4 = ppuVar4 + 1;
        }
        if (param_2 != 0) {
          puVar12 = param_1 + 0xc;
          uVar5 = param_2;
          do {
            if (puVar12[0x1f] != 0) {
              puVar12[0x2b] = (uint)local_40[*puVar12 & 0xf];
              local_40[*puVar12 & 0xf] = puVar12 + -0xc;
            }
            puVar12 = puVar12 + 0x38;
            uVar5 = uVar5 - 1;
          } while (uVar5 != 0);
        }
      }
      uVar5 = 0;
      if (local_bc != 0) {
        do {
          for (puVar12 = local_40[uVar5]; puVar12 != (uint *)0x0; puVar12 = (uint *)puVar12[0x37]) {
            for (uVar8 = puVar12[0x37]; uVar8 != 0; uVar8 = *(uint *)(uVar8 + 0xdc)) {
              if (((byte)puVar12[0x33] == 2) && (*(byte *)(uVar8 + 0xcc) < 2)) {
                if (((puVar12[0xc] == *(uint *)(uVar8 + 0x30)) &&
                    ((float)puVar12[6] < *(float *)(uVar8 + 0x18))) &&
                   (iVar6 = FUN_00579b50((int)puVar12,(int)uVar8,param_6), iVar6 != 0)) {
                  *(undefined4 *)(uVar8 + 0xac) = 0;
                }
              }
              else if (((*(byte *)(uVar8 + 0xcc) == 2) &&
                       ((((byte)puVar12[0x33] < 2 && (puVar12[0xc] == *(uint *)(uVar8 + 0x30))) &&
                        (*(float *)(uVar8 + 0x18) < (float)puVar12[6])))) &&
                      (iVar6 = FUN_00579b50((int)uVar8,(int)puVar12,param_6), iVar6 != 0)) {
                puVar12[0x2b] = 0;
                break;
              }
            }
          }
          uVar5 = uVar5 + 1;
        } while (uVar5 < local_bc);
      }
    }
  }
  iVar6 = 0;
  uVar5 = 0;
  uVar8 = 0;
  if (param_2 != 0) {
    piVar9 = (int *)(param_1 + 0x2b);
    do {
      if (*piVar9 != 0) {
        iVar6 = iVar6 + *piVar9;
        uVar5 = uVar8;
      }
      uVar8 = uVar8 + 1;
      piVar9 = piVar9 + 0x38;
    } while (uVar8 < param_2);
    if (iVar6 != 0) {
      puVar13 = param_1 + uVar5 * 0x38;
      if (iVar6 != param_1[uVar5 * 0x38 + 0x2b]) {
        local_a0 = 0;
        pfVar10 = (float *)(param_1 + 1);
        local_b4 = 0.0;
        do {
          if (pfVar10[0x2a] != 0.0) {
            uVar5 = 0;
            if (local_a0 != 0) {
              pfVar11 = pfVar10 + -1;
              piVar9 = (int *)(param_4 + 0x36);
              do {
                pfVar14 = (float *)(param_1 + *piVar9 * 0x38);
                if (_DAT_005cc9b4 <
                    pfVar10[1] * pfVar14[2] + *pfVar11 * *pfVar14 + pfVar14[1] * *pfVar10) {
                  local_94 = (uint)(fabs(pfVar10[1] * (pfVar10[4] - pfVar14[5]) +
                                        *pfVar11 * (pfVar10[2] - pfVar14[3]) +
                                        *pfVar10 * (pfVar10[3] - pfVar14[4])) <
                                   param_6 * _DAT_005cc328);
                  if ((local_94 != 0) ||
                     (fabs(pfVar14[2] * (pfVar10[4] - pfVar14[5]) +
                          (pfVar10[2] - pfVar14[3]) * *pfVar14 +
                          pfVar14[1] * (pfVar10[3] - pfVar14[4])) < param_6 * _DAT_005cc328)) {
                    pfVar10[0x36] = pfVar14[0x37];
                    *(void **)&pfVar14[0x37] = (void *)pfVar11;
                    break;
                  }
                }
                uVar5 = uVar5 + 1;
                piVar9 = piVar9 + 0x38;
              } while (uVar5 < local_a0);
            }
            if ((uVar5 == local_a0) && (uVar5 < param_5)) {
              local_a0 = local_a0 + 1;
              param_4[uVar5 * 0x38 + 0x36] = local_b4;
              pfVar10[0x36] = 0.0;
            }
          }
          local_b4 = (float)((int)local_b4 + 1);
          pfVar10 = pfVar10 + 0x38;
        } while ((uint)local_b4 < param_2);
        if (local_a0 != 0) {
          param_4 = param_4 + 0x2c;
          local_6c = local_a0;
          do {
            pfVar10 = (float *)(param_1 + param_4[10] * 0x38);
            if (pfVar10[0x37] == 0.0) {
              uVar1 = *param_4;
              pfVar11 = (float *)(param_4 + -0x2c);
              for (iVar6 = 0x38; iVar6 != 0; iVar6 = iVar6 + -1) {
                *pfVar11 = *pfVar10;
                pfVar10 = pfVar10 + 1;
                pfVar11 = pfVar11 + 1;
              }
              *param_4 = uVar1;
            }
            else {
              uVar5 = 0;
              for (puVar13 = param_1 + param_4[10] * 0x38; puVar13 != (undefined4 *)0x0;
                  puVar13 = (undefined4 *)puVar13[0x37]) {
                uVar5 = uVar5 + puVar13[0x2b];
              }
              local_84 = DAT_005d757c;
              local_88 = DAT_005d757c;
              local_a4 = 0.0;
              local_8c = DAT_005d757c;
              local_a8 = 0.0;
              local_ac = 0.0;
              if (pfVar10 != (float *)0x0) {
                fVar17 = 3.4028235e+38;
                pfVar11 = pfVar10;
                do {
                  local_50 = pfVar11[0x2b];
                  uStack_4c = 0;
                  pfVar14 = pfVar11 + 3;
                  fVar16 = (float)(uint)local_50;
                  local_ac = fVar16 * *pfVar11 + local_ac;
                  local_a8 = pfVar11[1] * fVar16 + local_a8;
                  local_a4 = pfVar11[2] * fVar16 + local_a4;
                  for (fVar16 = local_50; fVar16 != 0.0; fVar16 = (float)((int)fVar16 + -1)) {
                    local_8c = local_8c + *pfVar14;
                    local_88 = pfVar14[1] + local_88;
                    local_84 = pfVar14[2] + local_84;
                    if (pfVar14[3] < fVar17) {
                      fVar17 = pfVar14[3];
                      local_b0 = pfVar14;
                    }
                    pfVar14 = pfVar14 + 10;
                  }
                  pfVar11 = *(float **)&pfVar11[0x37];
                } while (pfVar11 != (float *)0x0);
              }
              local_60 = param_4;
              FUN_005667c0(&local_ac,&local_ac);
              uStack_44 = 0;
              fVar17 = _DAT_005cc320 / (float)uVar5;
              local_48 = uVar5;
              if (uVar5 < 5) {
                uVar1 = *param_4;
                pfVar11 = pfVar10;
                pfVar14 = (float *)(param_4 + -0x2c);
                for (iVar6 = 0x38; iVar6 != 0; iVar6 = iVar6 + -1) {
                  *pfVar14 = *pfVar11;
                  pfVar11 = pfVar11 + 1;
                  pfVar14 = pfVar14 + 1;
                }
                *param_4 = uVar1;
                for (fVar17 = pfVar10[0x37]; fVar17 != 0.0; fVar17 = *(float *)((int)fVar17 + 0xdc))
                {
                  pfVar10 = (float *)((int)fVar17 + 0xc);
                  iVar6 = *(int *)((int)fVar17 + 0xac);
                  while (iVar6 != 0) {
                    uVar5 = 0;
                    pfVar11 = (float *)(param_4 + -0x29);
                    if (param_4[-1] != 0) {
                      do {
                        if (fabs(*pfVar10 - *pfVar11) +
                            fabs(pfVar10[1] - pfVar11[1]) + fabs(pfVar10[2] - pfVar11[2]) <
                            param_6 * _DAT_005cc558) goto LAB_005797c4;
                        uVar5 = uVar5 + 1;
                        pfVar11 = pfVar11 + 10;
                      } while (uVar5 < (uint)param_4[-1]);
                    }
                    pfVar14 = pfVar10;
                    for (iVar7 = 10; iVar7 != 0; iVar7 = iVar7 + -1) {
                      *pfVar11 = *pfVar14;
                      pfVar14 = pfVar14 + 1;
                      pfVar11 = pfVar11 + 1;
                    }
                    param_4[-1] = param_4[-1] + 1;
LAB_005797c4:
                    pfVar10 = pfVar10 + 10;
                    iVar6 = iVar6 + -1;
                    local_70 = iVar6;
                  }
                }
                param_4[-0x2c] = local_ac;
                param_4[-0x2b] = local_a8;
                param_4[-0x2a] = local_a4;
                local_94 = 0;
                param_4 = local_60;
              }
              else {
                fVar16 = local_8c * fVar17 - *local_b0;
                local_7c = local_88 * fVar17 - local_b0[1];
                local_78 = fVar17 * local_84 - local_b0[2];
                if (fabs(fVar16) + fabs(local_78) + fabs(local_7c) < DAT_005ceabc) {
                  fVar16 = _DAT_005cc320;
                }
                local_74 = -3.4028235e+38;
                local_b4 = -3.4028235e+38;
                local_68 = 3.4028235e+38;
                local_5c = local_78 * local_a8 - local_7c * local_a4;
                local_64 = 3.4028235e+38;
                local_58 = local_a4 * fVar16 - local_78 * local_ac;
                local_54 = local_7c * local_ac - fVar16 * local_a8;
                for (pfVar11 = pfVar10; pfVar11 != (float *)0x0; pfVar11 = *(float **)&pfVar11[0x37]) {
                  pfVar14 = pfVar11 + 3;
                  for (fVar17 = pfVar11[0x2b]; fVar17 != 0.0; fVar17 = (float)((int)fVar17 + -1)) {
                    fVar2 = pfVar14[2] * local_78 + fVar16 * *pfVar14 + pfVar14[1] * local_7c;
                    if (local_b4 < fVar2) {
                      local_b4 = fVar2;
                      local_b0 = pfVar14;
                    }
                    if (fVar2 < local_64) {
                      local_90 = pfVar14;
                      local_64 = fVar2;
                    }
                    fVar2 = pfVar14[2] * local_54 + local_5c * *pfVar14 + pfVar14[1] * local_58;
                    if (local_74 < fVar2) {
                      local_98 = pfVar14;
                      local_74 = fVar2;
                    }
                    if (fVar2 < local_68) {
                      local_9c = pfVar14;
                      local_68 = fVar2;
                    }
                    pfVar14 = pfVar14 + 10;
                  }
                }
                uVar1 = *param_4;
                pfVar11 = (float *)(param_4 + -0x29);
                pfVar14 = (float *)(param_4 + -0x2c);
                for (iVar6 = 0x38; iVar6 != 0; iVar6 = iVar6 + -1) {
                  *pfVar14 = *pfVar10;
                  pfVar10 = pfVar10 + 1;
                  pfVar14 = pfVar14 + 1;
                }
                *param_4 = uVar1;
                param_4[-0x2c] = local_ac;
                param_4[-0x2b] = local_a8;
                param_4[-0x2a] = local_a4;
                pfVar10 = local_b0;
                pfVar14 = pfVar11;
                for (iVar6 = 10; iVar6 != 0; iVar6 = iVar6 + -1) {
                  *pfVar14 = *pfVar10;
                  pfVar10 = pfVar10 + 1;
                  pfVar14 = pfVar14 + 1;
                }
                param_4[-1] = 1;
                if (local_98 != local_b0) {
                  uVar5 = 0;
                  pfVar10 = pfVar11;
                  if (param_4[-1] != 0) {
                    do {
                      if (fabs(*local_98 - *pfVar10) +
                          fabs(local_98[2] - pfVar10[2]) + fabs(local_98[1] - pfVar10[1]) <
                          param_6 * _DAT_005cc558) goto LAB_00579a1b;
                      uVar5 = uVar5 + 1;
                      pfVar10 = pfVar10 + 10;
                    } while (uVar5 < (uint)param_4[-1]);
                  }
                  pfVar14 = local_98;
                  for (iVar6 = 10; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *pfVar10 = *pfVar14;
                    pfVar14 = pfVar14 + 1;
                    pfVar10 = pfVar10 + 1;
                  }
                  param_4[-1] = param_4[-1] + 1;
                }
LAB_00579a1b:
                if ((local_90 != local_b0) && (local_90 != local_98)) {
                  uVar5 = 0;
                  pfVar10 = pfVar11;
                  if (param_4[-1] != 0) {
                    do {
                      if (fabs(*local_90 - *pfVar10) +
                          fabs(local_90[2] - pfVar10[2]) + fabs(local_90[1] - pfVar10[1]) <
                          param_6 * _DAT_005cc558) goto LAB_00579a93;
                      uVar5 = uVar5 + 1;
                      pfVar10 = pfVar10 + 10;
                    } while (uVar5 < (uint)param_4[-1]);
                  }
                  pfVar14 = local_90;
                  for (iVar6 = 10; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *pfVar10 = *pfVar14;
                    pfVar14 = pfVar14 + 1;
                    pfVar10 = pfVar10 + 1;
                  }
                  param_4[-1] = param_4[-1] + 1;
                }
LAB_00579a93:
                if (((local_9c != local_b0) && (local_9c != local_98)) && (local_9c != local_90)) {
                  uVar5 = 0;
                  if (param_4[-1] != 0) {
                    do {
                      if (fabs(*local_9c - *pfVar11) +
                          fabs(local_9c[2] - pfVar11[2]) + fabs(local_9c[1] - pfVar11[1]) <
                          param_6 * _DAT_005cc558) goto LAB_00579b1b;
                      uVar5 = uVar5 + 1;
                      pfVar11 = pfVar11 + 10;
                    } while (uVar5 < (uint)param_4[-1]);
                  }
                  pfVar10 = local_9c;
                  for (iVar6 = 10; iVar6 != 0; iVar6 = iVar6 + -1) {
                    *pfVar11 = *pfVar10;
                    pfVar10 = pfVar10 + 1;
                    pfVar11 = pfVar11 + 1;
                  }
                  param_4[-1] = param_4[-1] + 1;
                }
              }
            }
LAB_00579b1b:
            param_4 = param_4 + 0x38;
            local_6c = local_6c - 1;
          } while (local_6c != 0);
        }
        return local_a0;
      }
LAB_005793b0:
      uVar1 = param_4[0x2c];
      puVar15 = param_4;
      for (iVar6 = 0x38; iVar6 != 0; iVar6 = iVar6 + -1) {
        *puVar15 = *puVar13;
        puVar13 = puVar13 + 1;
        puVar15 = puVar15 + 1;
      }
      param_4[0x2c] = uVar1;
      return 1;
    }
  }
  return 0;
}
