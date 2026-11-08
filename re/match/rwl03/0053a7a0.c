// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053a7a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(int, DAT_007dc5b0, 0x007dc5b0);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005ce54c, 0x005ce54c);
MGLOBAL(undefined1, DAT_005e4574, 0x005e4574);
MGLOBAL(undefined1, DAT_005cea1c, 0x005cea1c);
float10 FUN_004c3b90(float p0);
MGLOBAL(int, _DAT_005e4574, 0x005e4574);
MGLOBAL(int, _DAT_005cea1c, 0x005cea1c);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005ce54c, 0x005ce54c);
// MATCH: 0x0053a7a0 FUN_0053a7a0
/* [C1 2026-05-20] Ray-vs-atomic (per-RpAtomic mesh BVH). */

void FUN_0053a7a0(int param_1,float *param_2,undefined4 param_3,code *param_4,undefined4 param_5)

{
  float fVar1;
  float fVar2;
  char cVar3;
  ushort uVar4;
  int iVar5;
  undefined1 *puVar6;
  float fVar7;
  float fVar8;
  float fVar9;
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
  float fVar21;
  float fVar22;
  uint uVar23;
  int iVar24;
  uint uVar25;
  byte *pbVar26;
  uint uVar27;
  undefined1 **ppuVar28;
  ushort *puVar29;
  float10 fVar30;
  float local_280;
  float local_27c;
  int local_240;
  float local_228;
  float local_224;
  float local_220;
  float local_21c;
  float local_218;
  float local_214;
  uint local_210;
  float *local_20c;
  float *local_208;
  float *local_204;
  float local_200;
  float local_1fc;
  float local_1f8;
  float local_1f0;
  float local_1ec;
  float local_1e8;
  float local_1e4;
  float local_1e0;
  float local_1dc;
  float local_1d8;
  float local_1d4;
  int local_1d0;
  int local_1cc;
  float local_1c8 [12];
  undefined1 local_198;
  undefined1 local_197;
  undefined2 local_196;
  undefined1 *local_190;
  float local_18c;
  float local_188;
  undefined1 *local_184 [96];
  undefined1 **local_4;
  
  local_1d0 = *(int *)(DAT_007dc5b0 + param_1);
  if (local_1d0 == 0) {
    puVar29 = *(ushort **)(param_1 + 0x2c);
    local_240 = 0;
    fVar7 = param_2[3] - *param_2;
    iVar5 = *(int *)(*(int *)(param_1 + 0x5c) + 0x14);
    fVar8 = param_2[4] - param_2[1];
    fVar9 = param_2[5] - param_2[2];
    local_1d0 = 0;
    if (0 < *(int *)(param_1 + 0x10)) {
      do {
        local_20c = (float *)(iVar5 + (uint)*puVar29 * 0xc);
        fVar1 = *(float *)(iVar5 + (uint)puVar29[1] * 0xc) - *local_20c;
        local_208 = (float *)(iVar5 + (uint)puVar29[1] * 0xc);
        fVar2 = local_208[1] - local_20c[1];
        local_204 = (float *)(iVar5 + (uint)puVar29[2] * 0xc);
        fVar10 = local_208[2] - local_20c[2];
        fVar16 = *local_204 - *local_20c;
        fVar15 = local_204[1] - local_20c[1];
        fVar11 = local_204[2] - local_20c[2];
        fVar12 = fVar11 * fVar8 - fVar15 * fVar9;
        fVar21 = fVar16 * fVar9 - fVar11 * fVar7;
        fVar13 = fVar15 * fVar7 - fVar16 * fVar8;
        fVar14 = fVar13 * fVar10 + fVar12 * fVar1 + fVar21 * fVar2;
        if (_DAT_005e4574 < fVar14) {
          fVar22 = fVar14 * _DAT_005cea1c;
          fVar17 = fVar14 - fVar22;
          fVar18 = *param_2 - *local_20c;
          fVar19 = param_2[1] - local_20c[1];
          fVar20 = param_2[2] - local_20c[2];
          fVar12 = fVar20 * fVar13 + fVar18 * fVar12 + fVar19 * fVar21;
          if ((fVar22 <= fVar12) && (fVar12 < fVar17 != (fVar12 == fVar17))) {
            fVar13 = fVar19 * fVar10 - fVar20 * fVar2;
            fVar10 = fVar20 * fVar1 - fVar18 * fVar10;
            fVar1 = fVar18 * fVar2 - fVar19 * fVar1;
            fVar2 = fVar1 * fVar9 + fVar13 * fVar7 + fVar10 * fVar8;
            if ((fVar22 <= fVar2) &&
               (((fVar12 = fVar12 + fVar2, fVar12 < fVar17 != (fVar12 == fVar17) &&
                 (fVar1 = fVar1 * fVar11 + fVar13 * fVar16 + fVar10 * fVar15, fVar22 <= fVar1)) &&
                (fVar1 < fVar17 != (fVar1 == fVar17))))) {
              local_21c = *local_20c;
              local_218 = local_20c[1];
              local_214 = local_20c[2];
              local_210 = local_240;
              local_200 = *local_204 - *local_20c;
              local_1fc = local_204[1] - local_20c[1];
              local_228 = (local_204[2] - local_20c[2]) * (local_208[1] - local_20c[1]) -
                          local_1fc * (local_208[2] - local_20c[2]);
              local_224 = local_200 * (local_208[2] - local_20c[2]) -
                          (local_204[2] - local_20c[2]) * (*local_208 - *local_20c);
              local_220 = local_1fc * (*local_208 - *local_20c) -
                          local_200 * (local_208[1] - local_20c[1]);
              local_1e8 = local_220 * local_220 + local_228 * local_228 + local_224 * local_224;
              fVar30 = (float10)FUN_004c3b90(local_1e8);
              local_228 = (float)((float10)local_228 * fVar30);
              local_224 = (float)((float10)local_224 * fVar30);
              local_220 = (float)(fVar30 * (float10)local_220);
              iVar24 = (*param_4)(param_3,&local_228,fVar1 / fVar14,param_5);
              if (iVar24 == 0) {
                return;
              }
            }
          }
        }
        local_240 = local_240 + 1;
        puVar29 = puVar29 + 4;
      } while (local_240 < *(int *)(param_1 + 0x10));
    }
  }
  else {
    local_1e8 = *(float *)(param_1 + 0x2c);
    fVar7 = param_2[3] - *param_2;
    iVar5 = *(int *)(*(int *)(param_1 + 0x5c) + 0x14);
    local_196 = 0;
    local_197 = 0xff;
    fVar8 = param_2[4] - param_2[1];
    fVar9 = param_2[5] - param_2[2];
    if (*(short *)(local_1d0 + 0x1e) == 0) {
      local_197 = *(undefined1 *)(local_1d0 + 0x1c);
    }
    local_1c8[3] = fVar7;
    local_1c8[4] = fVar8;
    local_1c8[0] = *param_2;
    local_1c8[1] = param_2[1];
    local_1c8[2] = param_2[2];
    local_1c8[5] = fVar9;
    if (fVar7 == DAT_005d757c) {
      local_1c8[6] = 0.0;
    }
    else {
      local_1c8[6] = _DAT_005cc320 / fVar7;
    }
    if (fVar8 == DAT_005d757c) {
      local_1c8[7] = 0.0;
    }
    else {
      local_1c8[7] = _DAT_005cc320 / fVar8;
    }
    if (fVar9 == DAT_005d757c) {
      local_1c8[8] = 0.0;
    }
    else {
      local_1c8[8] = _DAT_005cc320 / fVar9;
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
      local_1c8[9] = param_2[3];
    }
    else {
      fVar1 = *param_2;
      local_1c8[9] = *param_2;
    }
    if (fVar1 < DAT_005d757c) {
      local_1c8[9] = -local_1c8[9];
    }
    fVar1 = param_2[1];
    if (param_2[1] < DAT_005d757c) {
      fVar1 = -fVar1;
    }
    fVar2 = param_2[4];
    if (param_2[4] < DAT_005d757c) {
      fVar2 = -fVar2;
    }
    if (fVar1 < fVar2) {
      fVar1 = param_2[4];
      local_1c8[10] = param_2[4];
    }
    else {
      fVar1 = param_2[1];
      local_1c8[10] = param_2[1];
    }
    if (fVar1 < DAT_005d757c) {
      local_1c8[10] = -local_1c8[10];
    }
    fVar1 = param_2[2];
    if (param_2[2] < DAT_005d757c) {
      fVar1 = -fVar1;
    }
    fVar2 = param_2[5];
    if (param_2[5] < DAT_005d757c) {
      fVar2 = -fVar2;
    }
    if (fVar1 < fVar2) {
      fVar1 = param_2[5];
      local_1c8[0xb] = param_2[5];
    }
    else {
      fVar1 = param_2[2];
      local_1c8[0xb] = param_2[2];
    }
    if (fVar1 < DAT_005d757c) {
      local_1c8[0xb] = -local_1c8[0xb];
    }
    local_1c8[9] = local_1c8[9] * _DAT_005ce54c;
    local_18c = 0.0;
    local_190 = &local_198;
    local_188 = 1.0;
    ppuVar28 = local_184;
    local_1c8[10] = local_1c8[10] * _DAT_005ce54c;
    local_1c8[0xb] = local_1c8[0xb] * _DAT_005ce54c;
    fVar1 = *(float *)(local_1d0 + 0x10) - local_1c8[9];
    local_1f0 = *(float *)(local_1d0 + 0x14) - local_1c8[10];
    local_1ec = *(float *)(local_1d0 + 0x18) - local_1c8[0xb];
    fVar2 = *(float *)(local_1d0 + 4) + local_1c8[9];
    fVar10 = *(float *)(local_1d0 + 8) + local_1c8[10];
    local_1f8 = *(float *)(local_1d0 + 0xc) + local_1c8[0xb];
    if (local_1c8[6] == DAT_005d757c) {
      if (local_1c8[0] < fVar1) {
        return;
      }
      if (local_1c8[0] < fVar2 == (local_1c8[0] == fVar2)) {
        return;
      }
    }
    else {
      local_27c = (fVar1 - local_1c8[0]) * local_1c8[6];
      fVar1 = (fVar2 - local_1c8[0]) * local_1c8[6];
      local_280 = fVar1;
      if (fVar1 < local_27c) {
        local_280 = local_27c;
        local_27c = fVar1;
      }
      if (DAT_005d757c < local_280 == (DAT_005d757c == local_280)) {
        return;
      }
      if (_DAT_005cc320 < local_27c) {
        return;
      }
      if (DAT_005d757c < local_27c) {
        local_18c = local_27c;
      }
      if (_DAT_005cc320 < local_280 == (_DAT_005cc320 == local_280)) {
        local_188 = local_280;
      }
    }
    if (local_1c8[7] == DAT_005d757c) {
      if (local_1c8[1] < local_1f0) {
        return;
      }
      if (local_1c8[1] < fVar10 == (local_1c8[1] == fVar10)) {
        return;
      }
    }
    else {
      fVar1 = (local_1f0 - local_1c8[1]) * local_1c8[7];
      local_27c = (fVar10 - local_1c8[1]) * local_1c8[7];
      local_280 = fVar1;
      if (local_27c < fVar1) {
        local_280 = local_27c;
        local_27c = fVar1;
      }
      if (local_18c < local_27c == (local_18c == local_27c)) {
        return;
      }
      if (local_188 < local_280) {
        return;
      }
      if (local_18c < local_280) {
        local_18c = local_280;
      }
      if (local_188 < local_27c == (local_188 == local_27c)) {
        local_188 = local_27c;
      }
    }
    local_1cc = local_1d0;
    if (local_1c8[8] == DAT_005d757c) {
      if (local_1ec <= local_1c8[2]) {
        if (local_1c8[2] < local_1f8 == (local_1c8[2] == local_1f8)) {
          return;
        }
LAB_0053ade4:
        do {
          local_4 = ppuVar28 + -3;
          puVar6 = ppuVar28[-3];
          cVar3 = puVar6[1];
          while (cVar3 == -1) {
            pbVar26 = (byte *)((uint)*(ushort *)(puVar6 + 2) * 0x10 + *(int *)(local_1cc + 0x20));
            uVar27 = *pbVar26 & 0xc;
            fVar1 = *(float *)((int)local_1c8 + uVar27 + 0xc);
            fVar2 = *(float *)((int)local_1c8 + uVar27 + 0x24);
            fVar15 = fVar1 * (float)local_4[2] + *(float *)((int)local_1c8 + uVar27);
            fVar16 = fVar1 * (float)local_4[1] + *(float *)((int)local_1c8 + uVar27);
            fVar10 = *(float *)(pbVar26 + 0xc) - fVar2;
            fVar2 = *(float *)(pbVar26 + 4) + fVar2;
            if (fVar1 <= DAT_005d757c) {
              if (fVar15 <= fVar2) {
                if (fVar10 <= fVar16) {
                  local_4 = local_4 + 3;
                  fVar1 = *(float *)((int)local_1c8 + uVar27 + 0x18);
                  *local_4 = pbVar26 + 8;
                  local_4[1] = local_4[-2];
                  local_4[2] = local_4[-1];
                  if (fVar15 < fVar10) {
                    local_4[2] = (undefined1 *)((fVar10 - fVar15) * fVar1 + (float)local_4[2]);
                  }
                  local_4[-3] = pbVar26;
                  fVar10 = fVar2;
                  if (fVar2 < fVar16) goto LAB_0053afbb;
                }
                else {
                  *local_4 = pbVar26;
                }
              }
              else {
                *local_4 = pbVar26 + 8;
              }
            }
            else if (fVar10 <= fVar15) {
              if (fVar16 <= fVar2) {
                local_4 = local_4 + 3;
                fVar1 = *(float *)((int)local_1c8 + uVar27 + 0x18);
                *local_4 = pbVar26;
                local_4[1] = local_4[-2];
                local_4[2] = local_4[-1];
                if (fVar2 < fVar15) {
                  local_4[2] = (undefined1 *)((fVar2 - fVar15) * fVar1 + (float)local_4[2]);
                }
                local_4[-3] = pbVar26 + 8;
                if (fVar16 < fVar10) {
LAB_0053afbb:
                  local_4[-2] = (undefined1 *)((fVar10 - fVar16) * fVar1 + (float)local_4[-2]);
                }
              }
              else {
                *local_4 = pbVar26 + 8;
              }
            }
            else {
              *local_4 = pbVar26;
            }
            puVar6 = *local_4;
            cVar3 = puVar6[1];
          }
          uVar4 = *(ushort *)(*local_4 + 2);
          uVar25 = (uint)uVar4;
          local_210 = (uint)uVar4;
          uVar27 = (byte)(*local_4)[1] + local_210;
          ppuVar28 = local_4;
          while (local_210 < uVar27) {
            if (*(int *)(local_1d0 + 0x24) != 0) {
              local_210 = (uint)*(ushort *)(*(int *)(local_1d0 + 0x24) + local_210 * 2);
            }
            local_20c = (float *)(iVar5 + (uint)*(ushort *)((int)local_1e8 + local_210 * 8) * 0xc);
            uVar23 = (uint)*(ushort *)((int)local_1e8 + 2 + local_210 * 8);
            local_1e0 = *(float *)(iVar5 + uVar23 * 0xc) - *local_20c;
            local_208 = (float *)(iVar5 + uVar23 * 0xc);
            local_1dc = *(float *)((int)local_208 + 4) - local_20c[1];
            local_204 = (float *)(iVar5 + (uint)*(ushort *)((int)local_1e8 + 4 + local_210 * 8) *
                                          0xc);
            local_1d4 = *(float *)((int)local_208 + 8) - local_20c[2];
            local_200 = *local_204 - *local_20c;
            local_1fc = local_204[1] - local_20c[1];
            fVar1 = local_204[2] - local_20c[2];
            fVar2 = fVar1 * fVar8 - local_1fc * fVar9;
            fVar15 = local_200 * fVar9 - fVar1 * fVar7;
            fVar10 = local_1fc * fVar7 - local_200 * fVar8;
            fVar16 = fVar10 * local_1d4 + fVar2 * local_1e0 + fVar15 * local_1dc;
            if (_DAT_005e4574 < fVar16) {
              fVar21 = fVar16 * _DAT_005cea1c;
              fVar11 = fVar16 - fVar21;
              fVar12 = *param_2 - *local_20c;
              fVar13 = param_2[1] - local_20c[1];
              fVar14 = param_2[2] - local_20c[2];
              fVar2 = fVar14 * fVar10 + fVar12 * fVar2 + fVar13 * fVar15;
              if ((fVar21 <= fVar2) && (fVar2 < fVar11 != (fVar2 == fVar11))) {
                fVar10 = fVar13 * local_1d4 - fVar14 * local_1dc;
                fVar15 = fVar14 * local_1e0 - fVar12 * local_1d4;
                fVar12 = fVar12 * local_1dc - fVar13 * local_1e0;
                fVar13 = fVar12 * fVar9 + fVar10 * fVar7 + fVar15 * fVar8;
                if ((fVar21 <= fVar13) &&
                   (((fVar2 = fVar2 + fVar13, fVar2 < fVar11 != (fVar2 == fVar11) &&
                     (fVar2 = fVar12 * fVar1 + fVar10 * local_200 + fVar15 * local_1fc,
                     fVar21 <= fVar2)) && (fVar2 < fVar11 != (fVar2 == fVar11))))) {
                  local_21c = *local_20c;
                  local_218 = local_20c[1];
                  local_214 = local_20c[2];
                  local_228 = fVar1 * local_1dc - local_1fc * local_1d4;
                  local_224 = local_200 * local_1d4 - fVar1 * local_1e0;
                  local_220 = local_1fc * local_1e0 - local_200 * local_1dc;
                  local_1e4 = local_200;
                  local_1d8 = local_1fc;
                  fVar30 = (float10)FUN_004c3b90(local_220 * local_220 +
                                                 local_228 * local_228 + local_224 * local_224);
                  local_228 = (float)((float10)local_228 * fVar30);
                  local_224 = (float)((float10)local_224 * fVar30);
                  local_220 = (float)(fVar30 * (float10)local_220);
                  iVar24 = (*param_4)(param_3,&local_228,fVar2 / fVar16,param_5);
                  ppuVar28 = local_4;
                  if (iVar24 == 0) {
                    return;
                  }
                }
              }
            }
            uVar25 = uVar25 + 1;
            local_210 = uVar25 & 0xffff;
          }
          if (ppuVar28 <= &local_190) {
            return;
          }
        } while( true );
      }
    }
    else {
      fVar1 = (local_1ec - local_1c8[2]) * local_1c8[8];
      local_27c = (local_1f8 - local_1c8[2]) * local_1c8[8];
      local_280 = fVar1;
      if (local_27c < fVar1) {
        local_280 = local_27c;
        local_27c = fVar1;
      }
      if ((local_18c < local_27c != (local_18c == local_27c)) && (local_280 <= local_188)) {
        if (local_18c < local_280) {
          local_18c = local_280;
        }
        if (local_188 < local_27c == (local_188 == local_27c)) {
          local_188 = local_27c;
        }
        goto LAB_0053ade4;
      }
    }
  }
  return;
}
