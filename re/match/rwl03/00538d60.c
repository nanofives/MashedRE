// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00538d60: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned short ushort;
typedef unsigned char byte;
typedef long double float10;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005ce54c, 0x005ce54c);
MGLOBAL(int, DAT_007dc5b4, 0x007dc5b4);
MGLOBAL(undefined1, DAT_005e4574, 0x005e4574);
MGLOBAL(undefined1, DAT_005cea1c, 0x005cea1c);
float10 FUN_004c3b90(float p0);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005ce54c, 0x005ce54c);
MGLOBAL(int, _DAT_005e4574, 0x005e4574);
MGLOBAL(int, _DAT_005cea1c, 0x005cea1c);
// MATCH: 0x00538d60 FUN_00538d60
/* [C1 2026-05-20] Ray-vs-BVH-vs-triangle intersection over a RpWorld using the game's collision
   BVH. */

void FUN_00538d60(int param_1,float *param_2,code *param_3,undefined4 param_4)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  char cVar5;
  ushort uVar6;
  int *piVar7;
  int iVar8;
  undefined1 *puVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float fVar20;
  int iVar21;
  byte *pbVar22;
  uint uVar23;
  int *piVar24;
  undefined1 **ppuVar25;
  float10 fVar26;
  uint local_564;
  float local_550;
  float local_54c;
  float local_548;
  float local_544;
  float local_540;
  float local_53c;
  uint local_538;
  float *local_534;
  float *local_530;
  float *local_52c;
  int local_528;
  float local_524;
  float local_520;
  int local_518;
  int local_514;
  uint local_510;
  int local_50c;
  float local_508 [12];
  undefined1 local_4d8;
  undefined1 local_4d7;
  undefined2 local_4d6;
  undefined1 *local_4d0;
  int local_4cc;
  int local_4c8;
  undefined1 *local_4c4 [96];
  undefined1 **local_344;
  float local_340 [12];
  int local_310;
  undefined4 local_30c;
  undefined4 local_308;
  int local_304 [192];
  int *local_4;
  
  local_340[3] = param_2[3] - *param_2;
  local_340[0] = *param_2;
  local_340[4] = param_2[4] - param_2[1];
  local_340[1] = param_2[1];
  local_340[5] = param_2[5] - param_2[2];
  local_340[2] = param_2[2];
  if (local_340[3] == DAT_005d757c) {
    local_340[6] = 0.0;
  }
  else {
    local_340[6] = _DAT_005cc320 / local_340[3];
  }
  if (local_340[4] == DAT_005d757c) {
    local_340[7] = 0.0;
  }
  else {
    local_340[7] = _DAT_005cc320 / local_340[4];
  }
  if (local_340[5] == DAT_005d757c) {
    local_340[8] = 0.0;
  }
  else {
    local_340[8] = _DAT_005cc320 / local_340[5];
  }
  fVar1 = *param_2;
  if (*param_2 < DAT_005d757c) {
    fVar1 = -fVar1;
  }
  fVar2 = param_2[3];
  if (param_2[3] < DAT_005d757c) {
    fVar2 = -fVar2;
  }
  if (fVar1 < fVar2) {
    fVar1 = param_2[3];
    fVar2 = param_2[3];
  }
  else {
    fVar1 = *param_2;
    fVar2 = *param_2;
  }
  if (fVar1 < DAT_005d757c) {
    fVar2 = -fVar2;
  }
  fVar1 = param_2[1];
  if (param_2[1] < DAT_005d757c) {
    fVar1 = -fVar1;
  }
  fVar3 = param_2[4];
  if (param_2[4] < DAT_005d757c) {
    fVar3 = -fVar3;
  }
  if (fVar1 < fVar3) {
    fVar3 = param_2[4];
    fVar1 = param_2[4];
  }
  else {
    fVar3 = param_2[1];
    fVar1 = param_2[1];
  }
  if (fVar3 < DAT_005d757c) {
    fVar1 = -fVar1;
  }
  fVar3 = param_2[2];
  if (param_2[2] < DAT_005d757c) {
    fVar3 = -fVar3;
  }
  fVar4 = param_2[5];
  if (param_2[5] < DAT_005d757c) {
    fVar4 = -fVar4;
  }
  if (fVar3 < fVar4) {
    fVar4 = param_2[5];
    fVar3 = param_2[5];
  }
  else {
    fVar4 = param_2[2];
    fVar3 = param_2[2];
  }
  if (fVar4 < DAT_005d757c) {
    fVar3 = -fVar3;
  }
  local_340[9] = fVar2 * _DAT_005ce54c;
  local_310 = *(int *)(param_1 + 0x1c);
  local_30c = 0;
  local_308 = 0x3f800000;
  piVar24 = local_304;
  local_340[10] = fVar1 * _DAT_005ce54c;
  local_340[0xb] = fVar3 * _DAT_005ce54c;
  do {
    local_4 = piVar24 + -3;
    piVar7 = (int *)piVar24[-3];
    iVar8 = *piVar7;
    piVar24 = local_4;
    while (local_4 = piVar24, -1 < iVar8) {
      iVar8 = *piVar7;
      fVar1 = *(float *)((int)local_340 + iVar8 + 0xc);
      fVar2 = *(float *)((int)local_340 + iVar8 + 0x24);
      fVar10 = fVar1 * (float)piVar24[2] + *(float *)((int)local_340 + iVar8);
      fVar4 = fVar1 * (float)piVar24[1] + *(float *)((int)local_340 + iVar8);
      fVar3 = (float)piVar7[5] - fVar2;
      fVar2 = (float)piVar7[4] + fVar2;
      if (fVar1 <= DAT_005d757c) {
        if (fVar10 <= fVar2) {
          if (fVar3 <= fVar4) {
            local_4 = piVar24 + 3;
            fVar1 = *(float *)((int)local_340 + *piVar7 + 0x18);
            piVar24[3] = piVar7[3];
            local_4[1] = local_4[-2];
            local_4[2] = local_4[-1];
            if (fVar10 < fVar3) {
              local_4[2] = (int)((fVar3 - fVar10) * fVar1 + (float)local_4[2]);
            }
            local_4[-3] = piVar7[2];
            fVar3 = fVar2;
            if (fVar2 < fVar4) goto LAB_00539198;
          }
          else {
            *piVar24 = piVar7[2];
          }
        }
        else {
          *piVar24 = piVar7[3];
        }
      }
      else if (fVar3 <= fVar10) {
        if (fVar4 <= fVar2) {
          local_4 = piVar24 + 3;
          fVar1 = *(float *)((int)local_340 + *piVar7 + 0x18);
          piVar24[3] = piVar7[2];
          local_4[1] = local_4[-2];
          local_4[2] = local_4[-1];
          if (fVar2 < fVar10) {
            local_4[2] = (int)((fVar2 - fVar10) * fVar1 + (float)local_4[2]);
          }
          local_4[-3] = piVar7[3];
          if (fVar4 < fVar3) {
LAB_00539198:
            local_4[-2] = (int)((fVar3 - fVar4) * fVar1 + (float)local_4[-2]);
          }
        }
        else {
          *piVar24 = piVar7[3];
        }
      }
      else {
        *piVar24 = piVar7[2];
      }
      piVar7 = (int *)*local_4;
      piVar24 = local_4;
      iVar8 = *piVar7;
    }
    local_528 = *piVar24;
    local_518 = *(int *)(local_528 + DAT_007dc5b4);
    if (local_518 != 0) {
      local_514 = *(int *)(local_528 + 4);
      iVar8 = *(int *)(local_528 + 8);
      local_4d6 = 0;
      local_4d7 = 0xff;
      if (*(short *)(local_518 + 0x1e) == 0) {
        local_4d7 = *(undefined1 *)(local_518 + 0x1c);
      }
      local_508[0] = local_340[0];
      local_508[1] = local_340[1];
      local_508[2] = local_340[2];
      local_508[3] = local_340[3];
      local_508[4] = local_340[4];
      local_508[5] = local_340[5];
      local_508[6] = local_340[6];
      local_508[7] = local_340[7];
      local_508[8] = local_340[8];
      local_508[9] = local_340[9];
      local_508[10] = local_340[10];
      local_508[0xb] = local_340[0xb];
      local_4d0 = &local_4d8;
      ppuVar25 = local_4c4;
      local_4cc = piVar24[1];
      local_4c8 = piVar24[2];
      local_50c = local_518;
      do {
        local_344 = ppuVar25 + -3;
        puVar9 = ppuVar25[-3];
        cVar5 = puVar9[1];
        ppuVar25 = local_344;
        while (local_344 = ppuVar25, cVar5 == -1) {
          pbVar22 = (byte *)((uint)*(ushort *)(puVar9 + 2) * 0x10 + *(int *)(local_50c + 0x20));
          uVar23 = *pbVar22 & 0xc;
          fVar1 = *(float *)((int)local_508 + uVar23 + 0xc);
          fVar2 = *(float *)((int)local_508 + uVar23 + 0x24);
          fVar10 = fVar1 * (float)ppuVar25[2] + *(float *)((int)local_508 + uVar23);
          fVar4 = fVar1 * (float)ppuVar25[1] + *(float *)((int)local_508 + uVar23);
          fVar3 = *(float *)(pbVar22 + 0xc) - fVar2;
          fVar2 = *(float *)(pbVar22 + 4) + fVar2;
          if (fVar1 <= DAT_005d757c) {
            if (fVar10 <= fVar2) {
              if (fVar3 <= fVar4) {
                local_344 = ppuVar25 + 3;
                fVar1 = *(float *)((int)local_508 + uVar23 + 0x18);
                ppuVar25[3] = pbVar22 + 8;
                local_344[1] = local_344[-2];
                local_344[2] = local_344[-1];
                if (fVar10 < fVar3) {
                  local_344[2] = (undefined1 *)((fVar3 - fVar10) * fVar1 + (float)local_344[2]);
                }
                local_344[-3] = pbVar22;
                fVar3 = fVar2;
                if (fVar2 < fVar4) goto LAB_005394ca;
              }
              else {
                *ppuVar25 = pbVar22;
              }
            }
            else {
              *ppuVar25 = pbVar22 + 8;
            }
          }
          else if (fVar3 <= fVar10) {
            if (fVar4 <= fVar2) {
              local_344 = ppuVar25 + 3;
              fVar1 = *(float *)((int)local_508 + uVar23 + 0x18);
              ppuVar25[3] = pbVar22;
              local_344[1] = local_344[-2];
              local_344[2] = local_344[-1];
              if (fVar2 < fVar10) {
                local_344[2] = (undefined1 *)((fVar2 - fVar10) * fVar1 + (float)local_344[2]);
              }
              local_344[-3] = pbVar22 + 8;
              if (fVar4 < fVar3) {
LAB_005394ca:
                local_344[-2] = (undefined1 *)((fVar3 - fVar4) * fVar1 + (float)local_344[-2]);
              }
            }
            else {
              *ppuVar25 = pbVar22 + 8;
            }
          }
          else {
            *ppuVar25 = pbVar22;
          }
          puVar9 = *local_344;
          piVar24 = local_4;
          ppuVar25 = local_344;
          cVar5 = puVar9[1];
        }
        uVar6 = *(ushort *)(*ppuVar25 + 2);
        local_564 = (uint)uVar6;
        local_538 = (uint)uVar6;
        local_510 = (byte)(*ppuVar25)[1] + local_538;
        if (local_538 < local_510) {
          do {
            if (*(int *)(local_518 + 0x24) != 0) {
              local_538 = (uint)*(ushort *)(*(int *)(local_518 + 0x24) + local_538 * 2);
            }
            local_534 = (float *)(iVar8 + (uint)*(ushort *)(local_514 + local_538 * 8) * 0xc);
            uVar23 = (uint)*(ushort *)(local_514 + 2 + local_538 * 8);
            fVar1 = *(float *)(iVar8 + uVar23 * 0xc) - *local_534;
            local_530 = (float *)(iVar8 + uVar23 * 0xc);
            fVar2 = local_530[1] - local_534[1];
            fVar3 = local_530[2] - local_534[2];
            local_52c = (float *)(iVar8 + (uint)*(ushort *)(local_514 + 4 + local_538 * 8) * 0xc);
            fVar4 = *local_52c - *local_534;
            fVar10 = local_52c[1] - local_534[1];
            fVar17 = local_52c[2] - local_534[2];
            fVar16 = fVar17 * local_340[4] - fVar10 * local_340[5];
            fVar19 = fVar4 * local_340[5] - fVar17 * local_340[3];
            fVar15 = fVar10 * local_340[3] - fVar4 * local_340[4];
            fVar18 = fVar15 * fVar3 + fVar16 * fVar1 + fVar19 * fVar2;
            if (_DAT_005e4574 < fVar18) {
              fVar20 = fVar18 * _DAT_005cea1c;
              fVar11 = fVar18 - fVar20;
              fVar12 = local_340[0] - *local_534;
              fVar13 = local_340[1] - local_534[1];
              fVar14 = local_340[2] - local_534[2];
              fVar16 = fVar14 * fVar15 + fVar12 * fVar16 + fVar13 * fVar19;
              if ((fVar20 <= fVar16) && (fVar16 < fVar11 != (fVar16 == fVar11))) {
                fVar15 = fVar13 * fVar3 - fVar14 * fVar2;
                fVar3 = fVar14 * fVar1 - fVar12 * fVar3;
                fVar1 = fVar12 * fVar2 - fVar13 * fVar1;
                fVar2 = fVar1 * local_340[5] + fVar15 * local_340[3] + fVar3 * local_340[4];
                if ((fVar20 <= fVar2) &&
                   (((fVar16 = fVar16 + fVar2, fVar16 < fVar11 != (fVar16 == fVar11) &&
                     (fVar1 = fVar1 * fVar17 + fVar15 * fVar4 + fVar3 * fVar10, fVar20 <= fVar1)) &&
                    (fVar1 < fVar11 != (fVar1 == fVar11))))) {
                  local_544 = *local_534;
                  local_540 = local_534[1];
                  local_53c = local_534[2];
                  local_524 = *local_52c - *local_534;
                  local_520 = local_52c[1] - local_534[1];
                  local_550 = (local_52c[2] - local_534[2]) * (local_530[1] - local_534[1]) -
                              local_520 * (local_530[2] - local_534[2]);
                  local_54c = local_524 * (local_530[2] - local_534[2]) -
                              (local_52c[2] - local_534[2]) * (*local_530 - *local_534);
                  local_548 = local_520 * (*local_530 - *local_534) -
                              local_524 * (local_530[1] - local_534[1]);
                  fVar26 = (float10)FUN_004c3b90(local_548 * local_548 +
                                                 local_550 * local_550 + local_54c * local_54c);
                  local_550 = (float)((float10)local_550 * fVar26);
                  local_54c = (float)((float10)local_54c * fVar26);
                  local_548 = (float)(fVar26 * (float10)local_548);
                  iVar21 = (*param_3)(param_2,local_528,&local_550,fVar1 / fVar18,param_4);
                  piVar24 = local_4;
                  ppuVar25 = local_344;
                  if (iVar21 == 0) {
                    return;
                  }
                }
              }
            }
            local_564 = local_564 + 1;
            local_538 = local_564 & 0xffff;
          } while ((int)local_538 < (int)local_510);
        }
      } while (&local_4d0 < ppuVar25);
    }
    if (piVar24 <= &local_310) {
      return;
    }
  } while( true );
}
