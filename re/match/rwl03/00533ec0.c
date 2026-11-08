// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00533ec0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, FUN_00532a60, 0x00532a60);
byte FUN_004c0b10(int p0);
void FUN_004c0e50(int p0);
int FUN_004c0ed0(int p0);
undefined4 * FUN_004c4600(undefined4 * p0, undefined4 * p1, undefined4 * p2);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x00533ec0 FUN_00533ec0
/* [C1 2026-05-20] Per-frame matrix-palette assembly for the skin atomic. */

undefined4 FUN_00533ec0(uint *param_1)

{
  byte *pbVar1;
  float fVar2;
  float fVar3;
  uint uVar4;
  code *pcVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  int iVar9;
  uint *puVar10;
  uint uVar11;
  int iVar12;
  int iVar13;
  uint uVar14;
  float *pfVar15;
  uint *puVar16;
  float *pfVar17;
  uint *puVar18;
  uint *local_a14;
  uint *local_a10;
  uint *local_a0c;
  uint *local_a04;
  int local_a00;
  int local_9d8;
  float local_9d0 [5];
  float local_9bc;
  float local_9b8;
  float local_9b0;
  float local_9ac;
  float local_9a8;
  undefined4 local_9a0;
  undefined4 local_99c;
  undefined4 local_998;
  int local_990;
  float local_98c;
  uint local_980 [4];
  undefined4 local_970;
  undefined4 local_96c;
  undefined4 local_968;
  undefined4 local_960;
  undefined4 local_95c;
  undefined4 local_958;
  undefined4 local_950;
  undefined4 local_94c;
  undefined4 local_948;
  uint local_940 [4];
  undefined4 local_930;
  undefined4 local_92c;
  undefined4 local_928;
  undefined4 local_920;
  undefined4 local_91c;
  undefined4 local_918;
  undefined4 local_910;
  undefined4 local_90c;
  undefined4 local_908;
  uint local_900;
  undefined4 local_8fc;
  undefined4 local_8f8;
  uint local_8f4;
  undefined4 local_8f0;
  undefined4 local_8ec;
  undefined4 local_8e8;
  undefined4 local_8e0;
  undefined4 local_8dc;
  undefined4 local_8d8;
  undefined4 local_8d0;
  undefined4 local_8cc;
  undefined4 local_8c8;
  uint local_8c0 [16];
  uint local_880 [48];
  uint local_7c0 [496];
  
  uVar4 = *param_1;
  if ((uVar4 & 2) != 0) {
    iVar12 = *(int *)(param_1[5] + 4);
    if (iVar12 == 0) {
      local_958 = 0x3f800000;
      local_980[3] = local_980[3] | 0x20003;
      local_96c = 0x3f800000;
      local_980[0] = 0x3f800000;
      local_970 = 0;
      local_980[2] = 0;
      local_980[1] = 0;
      local_95c = 0;
      local_960 = 0;
      local_968 = 0;
      local_948 = 0;
      local_94c = 0;
      local_950 = 0;
    }
    else {
      iVar9 = FUN_004c0b10(iVar12);
      if (iVar9 == 0) {
        puVar10 = (uint *)FUN_004c0ed0(iVar12);
        puVar16 = local_980;
        for (iVar12 = 0x10; iVar12 != 0; iVar12 = iVar12 + -1) {
          *puVar16 = *puVar10;
          puVar10 = puVar10 + 1;
          puVar16 = puVar16 + 1;
        }
      }
      else {
        puVar10 = (uint *)(iVar12 + 0x10);
        puVar16 = local_980;
        for (iVar9 = 0x10; iVar9 != 0; iVar9 = iVar9 + -1) {
          *puVar16 = *puVar10;
          puVar10 = puVar10 + 1;
          puVar16 = puVar16 + 1;
        }
        for (iVar12 = *(int *)(iVar12 + 4); iVar12 != 0; iVar12 = *(int *)(iVar12 + 4)) {
          puVar10 = local_980;
          puVar16 = local_880;
          for (iVar9 = 0x10; iVar9 != 0; iVar9 = iVar9 + -1) {
            *puVar16 = *puVar10;
            puVar10 = puVar10 + 1;
            puVar16 = puVar16 + 1;
          }
          FUN_004c4600(local_980,local_880,iVar12 + 0x10);
        }
      }
    }
    uVar14 = uVar4 & 0x2000;
    if ((uVar14 != 0) && ((*(byte *)(*(int *)(param_1[5] + 0xa0) + 3) & 3) == 0)) {
      *(undefined4 *)(*(int *)(param_1[5] + 0xa0) + 8) = *(undefined4 *)(DAT_007d3ff8 + 0xbc);
      *(int *)(*(int *)(param_1[5] + 0xa0) + 0xc) = DAT_007d3ff8 + 0xbc;
      *(int *)(*(int *)(DAT_007d3ff8 + 0xbc) + 4) = *(int *)(param_1[5] + 0xa0) + 8;
      *(int *)(DAT_007d3ff8 + 0xbc) = *(int *)(param_1[5] + 0xa0) + 8;
      pbVar1 = (byte *)(*(int *)(param_1[5] + 0xa0) + 3);
      *pbVar1 = *pbVar1 | 2;
    }
    uVar11 = param_1[8];
    local_a00 = 0;
    iVar9 = *(int *)(uVar11 + 0x24);
    pcVar5 = *(code **)(uVar11 + 0x3c);
    iVar12 = uVar11 + 0x4c;
    local_a10 = local_7c0;
    if ((int)param_1[1] < 1) {
      return 1;
    }
    local_a0c = (uint *)(param_1[4] + 8);
    do {
      if (pcVar5 == FUN_00532a60) {
        fVar2 = *(float *)(iVar12 + 8);
        fVar3 = *(float *)(iVar12 + 0xc);
        fVar6 = *(float *)(iVar12 + 0x10);
        fVar7 = *(float *)(iVar12 + 0x14);
        local_98c = fVar7 * fVar2;
        fVar8 = fVar6 * fVar6 + fVar3 * fVar3;
        local_9d0[0] = _DAT_005cc320 - (fVar8 + fVar8);
        local_9d0[1] = fVar3 * fVar2 + fVar7 * fVar6;
        local_9d0[1] = local_9d0[1] + local_9d0[1];
        local_9d0[2] = fVar6 * fVar2 - fVar7 * fVar3;
        local_9d0[2] = local_9d0[2] + local_9d0[2];
        local_9d0[4] = fVar3 * fVar2 - fVar7 * fVar6;
        local_9d0[4] = local_9d0[4] + local_9d0[4];
        fVar8 = fVar6 * fVar6 + fVar2 * fVar2;
        local_9bc = _DAT_005cc320 - (fVar8 + fVar8);
        local_9b8 = local_98c + fVar6 * fVar3;
        local_9b8 = local_9b8 + local_9b8;
        local_9b0 = fVar7 * fVar3 + fVar6 * fVar2;
        local_9b0 = local_9b0 + local_9b0;
        local_9ac = fVar6 * fVar3 - local_98c;
        local_9ac = local_9ac + local_9ac;
        fVar2 = fVar3 * fVar3 + fVar2 * fVar2;
        local_9a8 = _DAT_005cc320 - (fVar2 + fVar2);
        local_9a0 = *(undefined4 *)(iVar12 + 0x18);
        local_99c = *(undefined4 *)(iVar12 + 0x1c);
        local_998 = *(undefined4 *)(iVar12 + 0x20);
        local_9d0[3] = 4.2039e-45;
      }
      else {
        (*pcVar5)(local_9d0,iVar12);
      }
      FUN_004c4600(local_8c0,local_9d0,local_980);
      uVar11 = local_a0c[1];
      if (uVar11 != 0) {
        if ((uVar4 & 0x1000) == 0) {
          if (uVar14 != 0) goto LAB_0053426d;
        }
        else {
          pfVar15 = local_9d0;
          pfVar17 = (float *)(uVar11 + 0x10);
          for (iVar13 = 0x10; iVar13 != 0; iVar13 = iVar13 + -1) {
            *pfVar17 = *pfVar15;
            pfVar15 = pfVar15 + 1;
            pfVar17 = pfVar17 + 1;
          }
          if (uVar14 == 0) {
            FUN_004c0e50(uVar11);
          }
          else {
LAB_0053426d:
            puVar10 = local_8c0;
            puVar16 = (uint *)(uVar11 + 0x50);
            for (iVar13 = 0x10; iVar13 != 0; iVar13 = iVar13 + -1) {
              *puVar16 = *puVar10;
              puVar10 = puVar10 + 1;
              puVar16 = puVar16 + 1;
            }
            *(byte *)(uVar11 + 3) = *(byte *)(uVar11 + 3) & 0xfb | 8;
          }
        }
      }
      uVar11 = *local_a0c & 3;
      if (uVar11 == 0) {
LAB_005342bb:
        puVar10 = local_8c0;
LAB_005342c2:
        puVar16 = local_980;
        for (iVar13 = 0x10; iVar13 != 0; iVar13 = iVar13 + -1) {
          *puVar16 = *puVar10;
          puVar10 = puVar10 + 1;
          puVar16 = puVar16 + 1;
        }
      }
      else {
        if (uVar11 == 1) {
          puVar10 = local_a10 + -0x10;
          local_a10 = puVar10;
          goto LAB_005342c2;
        }
        if (uVar11 == 2) {
          puVar18 = local_a10 + 0x10;
          puVar10 = local_980;
          puVar16 = local_a10;
          for (iVar13 = 0x10; local_a10 = puVar18, iVar13 != 0; iVar13 = iVar13 + -1) {
            *puVar16 = *puVar10;
            puVar10 = puVar10 + 1;
            puVar16 = puVar16 + 1;
          }
          goto LAB_005342bb;
        }
      }
      iVar12 = iVar12 + iVar9;
      local_a0c = local_a0c + 4;
      local_a00 = local_a00 + 1;
      if ((int)param_1[1] <= local_a00) {
        return 1;
      }
    } while( true );
  }
  if (((uVar4 & 1) == 0) || (param_1[7] == 0xffffffff)) {
    if ((uVar4 & 0x4000) == 0) {
      local_a14 = local_940;
    }
    else {
      local_a14 = &local_900;
      local_8f4 = local_8f4 | 0x20003;
      local_8d8 = 0x3f800000;
      local_8ec = 0x3f800000;
      local_900 = 0x3f800000;
      local_8f0 = 0;
      local_8f8 = 0;
      local_8fc = 0;
      local_8dc = 0;
      local_8e0 = 0;
      local_8e8 = 0;
      local_8c8 = 0;
      local_8cc = 0;
      local_8d0 = 0;
      if ((uVar4 & 0x2000) == 0) goto LAB_00534449;
    }
    uVar14 = param_1[5];
  }
  else {
    local_a14 = (uint *)(param_1[7] * 0x40 + *(int *)(param_1[6] + 8));
    if (((uVar4 & 0x2000) == 0) || ((uVar4 & 0x4000) == 0)) goto LAB_00534449;
    uVar14 = *(uint *)(param_1[6] + 0x14);
  }
  if ((uVar14 == 0) || (iVar12 = *(int *)(uVar14 + 4), iVar12 == 0)) {
    local_918 = 0x3f800000;
    local_940[3] = local_940[3] | 0x20003;
    local_92c = 0x3f800000;
    local_940[0] = 0x3f800000;
    local_930 = 0;
    local_940[2] = 0;
    local_940[1] = 0;
    local_91c = 0;
    local_920 = 0;
    local_928 = 0;
    local_908 = 0;
    local_90c = 0;
    local_910 = 0;
  }
  else {
    iVar9 = FUN_004c0b10(iVar12);
    if (iVar9 == 0) {
      puVar10 = (uint *)FUN_004c0ed0(iVar12);
      puVar16 = local_940;
      for (iVar12 = 0x10; iVar12 != 0; iVar12 = iVar12 + -1) {
        *puVar16 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar16 = puVar16 + 1;
      }
    }
    else {
      puVar10 = (uint *)(iVar12 + 0x10);
      puVar16 = local_940;
      for (iVar9 = 0x10; iVar9 != 0; iVar9 = iVar9 + -1) {
        *puVar16 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar16 = puVar16 + 1;
      }
      for (iVar12 = *(int *)(iVar12 + 4); iVar12 != 0; iVar12 = *(int *)(iVar12 + 4)) {
        puVar10 = local_940;
        puVar16 = local_880;
        for (iVar9 = 0x10; iVar9 != 0; iVar9 = iVar9 + -1) {
          *puVar16 = *puVar10;
          puVar10 = puVar10 + 1;
          puVar16 = puVar16 + 1;
        }
        FUN_004c4600(local_940,local_880,iVar12 + 0x10);
      }
    }
  }
LAB_00534449:
  uVar14 = uVar4 & 0x2000;
  if ((uVar14 != 0) && ((*(byte *)(*(int *)(param_1[5] + 0xa0) + 3) & 3) == 0)) {
    *(undefined4 *)(*(int *)(param_1[5] + 0xa0) + 8) = *(undefined4 *)(DAT_007d3ff8 + 0xbc);
    *(int *)(*(int *)(param_1[5] + 0xa0) + 0xc) = DAT_007d3ff8 + 0xbc;
    *(int *)(*(int *)(DAT_007d3ff8 + 0xbc) + 4) = *(int *)(param_1[5] + 0xa0) + 8;
    *(int *)(DAT_007d3ff8 + 0xbc) = *(int *)(param_1[5] + 0xa0) + 8;
    pbVar1 = (byte *)(*(int *)(param_1[5] + 0xa0) + 3);
    *pbVar1 = *pbVar1 | 2;
  }
  uVar11 = param_1[8];
  puVar10 = (uint *)param_1[2];
  local_9d8 = 0;
  local_990 = *(int *)(uVar11 + 0x24);
  pcVar5 = *(code **)(uVar11 + 0x3c);
  iVar12 = uVar11 + 0x4c;
  local_a0c = local_880 + 1;
  if (0 < (int)param_1[1]) {
    local_a04 = (uint *)(param_1[4] + 8);
    do {
      if (pcVar5 == FUN_00532a60) {
        fVar2 = *(float *)(iVar12 + 8);
        fVar3 = *(float *)(iVar12 + 0xc);
        fVar6 = *(float *)(iVar12 + 0x10);
        fVar7 = *(float *)(iVar12 + 0x14);
        local_98c = fVar7 * fVar2;
        fVar8 = fVar6 * fVar6 + fVar3 * fVar3;
        local_9d0[0] = _DAT_005cc320 - (fVar8 + fVar8);
        local_9d0[1] = fVar3 * fVar2 + fVar7 * fVar6;
        local_9d0[1] = local_9d0[1] + local_9d0[1];
        local_9d0[2] = fVar6 * fVar2 - fVar7 * fVar3;
        local_9d0[2] = local_9d0[2] + local_9d0[2];
        local_9d0[4] = fVar3 * fVar2 - fVar7 * fVar6;
        local_9d0[4] = local_9d0[4] + local_9d0[4];
        fVar8 = fVar6 * fVar6 + fVar2 * fVar2;
        local_9bc = _DAT_005cc320 - (fVar8 + fVar8);
        local_9b8 = local_98c + fVar6 * fVar3;
        local_9b8 = local_9b8 + local_9b8;
        local_9b0 = fVar7 * fVar3 + fVar6 * fVar2;
        local_9b0 = local_9b0 + local_9b0;
        local_9ac = fVar6 * fVar3 - local_98c;
        local_9ac = local_9ac + local_9ac;
        fVar2 = fVar3 * fVar3 + fVar2 * fVar2;
        local_9a8 = _DAT_005cc320 - (fVar2 + fVar2);
        local_9a0 = *(undefined4 *)(iVar12 + 0x18);
        local_99c = *(undefined4 *)(iVar12 + 0x1c);
        local_998 = *(undefined4 *)(iVar12 + 0x20);
        local_9d0[3] = 4.2039e-45;
      }
      else {
        (*pcVar5)(local_9d0,iVar12);
      }
      FUN_004c4600(puVar10,local_9d0,local_a14);
      uVar11 = local_a04[1];
      if (uVar11 != 0) {
        if ((uVar4 & 0x1000) == 0) {
          if (uVar14 != 0) goto LAB_005347aa;
        }
        else {
          pfVar15 = local_9d0;
          pfVar17 = (float *)(uVar11 + 0x10);
          for (iVar9 = 0x10; iVar9 != 0; iVar9 = iVar9 + -1) {
            *pfVar17 = *pfVar15;
            pfVar15 = pfVar15 + 1;
            pfVar17 = pfVar17 + 1;
          }
          if (uVar14 == 0) {
            FUN_004c0e50(uVar11);
          }
          else {
LAB_005347aa:
            if ((uVar4 & 0x4000) == 0) {
              puVar16 = puVar10;
              puVar18 = (uint *)(uVar11 + 0x50);
              for (iVar9 = 0x10; iVar9 != 0; iVar9 = iVar9 + -1) {
                *puVar18 = *puVar16;
                puVar16 = puVar16 + 1;
                puVar18 = puVar18 + 1;
              }
            }
            else {
              FUN_004c4600(uVar11 + 0x50,puVar10,local_940);
            }
            *(byte *)(uVar11 + 3) = *(byte *)(uVar11 + 3) & 0xfb | 8;
          }
        }
      }
      uVar11 = *local_a04 & 3;
      puVar16 = puVar10;
      if (uVar11 != 0) {
        if (uVar11 == 1) {
          local_a0c = local_a0c + -1;
          puVar16 = (uint *)*local_a0c;
        }
        else {
          puVar16 = local_a14;
          if (uVar11 == 2) {
            *local_a0c = (uint)local_a14;
            local_a0c = local_a0c + 1;
            puVar16 = puVar10;
          }
        }
      }
      local_a14 = puVar16;
      iVar12 = iVar12 + local_990;
      puVar10 = puVar10 + 0x10;
      local_a04 = local_a04 + 4;
      local_9d8 = local_9d8 + 1;
    } while (local_9d8 < (int)param_1[1]);
  }
  return 1;
}
