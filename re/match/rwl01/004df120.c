// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 46.3% (regs 56.6%), class b. Raw converter output with prototype/cast fixes for the C++ check only. Frame 0x21c vs 0x220 (4-byte error-record store eliminated, same fix as 0x004e14a0: 2-dword record as an array); not rewritten, above budget. */
/* MATCH-RWL01 0x004df120: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(undefined1, DAT_005d8bf5, 0x005d8bf5);
MGLOBALA(char, s_Conversion_to_compressed_rasters_00618510, 0x34, 0x00618510);
MGLOBAL(undefined4, DAT_00911b00, 0x00911b00);
undefined4 FUN_004c7600(int * p0);
int FUN_004c7620(int * p0);
uint FUN_004c76c0(int * p0, uint p1);
uint FUN_004c7860(int * p0, uint p1, int p2);
undefined4 FUN_004cdd00(int p0);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
undefined4 * FUN_004d8480(undefined4 * p0);
int FUN_004db2e0(int p0, undefined4 p1, undefined4 p2);
int FUN_004dee40(int p0);
undefined * FUN_004df750(undefined4 p0);
void FUN_004dfbc0(int * p0, int p1);
void FUN_004e02d0(int * p0, int p1);
// MATCH: 0x004df120 FUN_004df120
undefined4 FUN_004df120(int *param_1,int param_2)

{
  byte *pbVar1;
  byte *pbVar2;
  byte *pbVar3;
  byte bVar4;
  undefined4 uVar5;
  bool bVar6;
  bool bVar7;
  undefined2 uVar8;
  int iVar9;
  int iVar10;
  code *pcVar11;
  byte *pbVar12;
  ushort *puVar13;
  uint uVar14;
  undefined2 *puVar15;
  int iVar16;
  int iVar17;
  int local_220;
  int local_21c;
  code *local_210;
  undefined4 local_20c;
  uint local_208;
  undefined4 local_204;
  undefined2 local_200 [256];
  
  iVar10 = DAT_00911ae4;
  bVar7 = false;
  bVar6 = false;
  if ((*(byte *)(DAT_00911ae4 + 10 + (int)param_1) & 0xf0) != 0) {
    local_208 = 1;
    local_204 = FUN_004d7ff0(2,s_Conversion_to_compressed_rasters_00618510);
    FUN_004d8480(&local_208);
    return 0;
  }
  uVar14 = (uint)*(byte *)((int)param_1 + 0x23) << 8;
  if ((char)param_1[8] == '\0') {
    iVar9 = FUN_004dee40(param_2);
    *(undefined *)((int)param_1 + iVar10 + 8) = (&DAT_005d8bf5)[(iVar9 >> 8 & 0xfU) * 8];
  }
  else {
    iVar9 = *(int *)((int)param_1 + DAT_00911ae4 + 0x18);
    if (iVar9 == 0x29) {
      if ((uVar14 & 0xf00) == 0x500) {
        *(undefined1 *)((int)param_1 + DAT_00911ae4 + 8) = 1;
      }
      else {
        *(undefined1 *)((int)param_1 + DAT_00911ae4 + 8) = 0;
      }
    }
    else {
      *(undefined1 *)((int)param_1 + DAT_00911ae4 + 8) = *(undefined1 *)(&DAT_00911b00 + iVar9);
    }
  }
  if ((*(char *)((int)param_1 + iVar10 + 8) != '\0') && (param_1 != (int *)*param_1)) {
    *(undefined1 *)(*param_1 + DAT_00911ae4 + 8) = 1;
  }
  if ((param_1[3] != *(int *)(param_2 + 4)) ||
     (iVar10 = param_2, param_1[4] != *(int *)(param_2 + 8))) {
    iVar10 = FUN_004db2e0(param_2,param_1[3],param_1[4]);
  }
  if ((*(byte *)((int)param_1 + 0x22) & 4) == 0) {
    iVar9 = FUN_004c7860(param_1,0,5);
    if (iVar9 == 0) {
      return 0;
    }
  }
  else {
    bVar7 = true;
  }
  local_208 = uVar14 & 0x6000;
  if (local_208 != 0) {
    if ((*(byte *)((int)param_1 + 0x22) & 0x10) == 0) {
      iVar9 = FUN_004c76c0(param_1,5);
      if (iVar9 == 0) {
        return 0;
      }
    }
    else {
      bVar6 = true;
    }
  }
  switch(param_1[5]) {
  case 4:
  case 8:
    FUN_004dfbc0(param_1,iVar10);
    break;
  default:
    local_210 = (code *)0x1;
    local_20c = FUN_004d7ff0(0x8000000c);
    FUN_004d8480((undefined4 *)&local_210);
    break;
  case 0x10:
    iVar9 = *(int *)(iVar10 + 0xc);
    if ((iVar9 == 4) || (iVar9 == 8)) {
      pcVar11 = (code *)FUN_004df750(*(undefined4 *)(DAT_00911ae4 + 0x18 + (int)param_1));
      iVar9 = 0;
      puVar15 = local_200;
      if (*(int *)(iVar10 + 0xc) == 4) {
        do {
          uVar8 = (*pcVar11)(*(int *)(iVar10 + 0x18) + iVar9);
          *puVar15 = uVar8;
          iVar9 = iVar9 + 4;
          puVar15 = puVar15 + 1;
        } while (iVar9 < 0x40);
      }
      else {
        do {
          uVar8 = (*pcVar11)(*(int *)(iVar10 + 0x18) + iVar9);
          *puVar15 = uVar8;
          iVar9 = iVar9 + 4;
          puVar15 = puVar15 + 1;
        } while (iVar9 < 0x400);
      }
      iVar9 = 0;
      if (0 < param_1[4]) {
        iVar16 = param_1[3];
        do {
          pbVar12 = (byte *)(*(int *)(iVar10 + 0x10) * iVar9 + *(int *)(iVar10 + 0x14));
          iVar17 = 0;
          puVar15 = (undefined2 *)(param_1[6] * iVar9 + param_1[1]);
          if (0 < iVar16) {
            do {
              bVar4 = *pbVar12;
              pbVar12 = pbVar12 + 1;
              iVar17 = iVar17 + 1;
              *puVar15 = local_200[bVar4];
              iVar16 = param_1[3];
              puVar15 = puVar15 + 1;
            } while (iVar17 < iVar16);
          }
          iVar9 = iVar9 + 1;
        } while (iVar9 < param_1[4]);
      }
    }
    else if (iVar9 == 0x20) {
      uVar5 = *(undefined4 *)(DAT_00911ae4 + 0x18 + (int)param_1);
      switch(uVar5) {
      case 0x17:
        local_220 = 0;
        if (0 < param_1[4]) {
          iVar9 = param_1[3];
          do {
            pbVar12 = (byte *)(*(int *)(iVar10 + 0x10) * local_220 + *(int *)(iVar10 + 0x14));
            iVar16 = 0;
            puVar13 = (ushort *)(param_1[6] * local_220 + param_1[1]);
            if (0 < iVar9) {
              do {
                bVar4 = *pbVar12;
                pbVar1 = pbVar12 + 1;
                pbVar2 = pbVar12 + 2;
                pbVar12 = pbVar12 + 4;
                *puVar13 = ((bVar4 & 0xfff8) << 5 | (ushort)(*pbVar1 & 0xfc)) << 3 |
                           (ushort)(*pbVar2 >> 3);
                iVar9 = param_1[3];
                iVar16 = iVar16 + 1;
                puVar13 = puVar13 + 1;
              } while (iVar16 < iVar9);
            }
            local_220 = local_220 + 1;
          } while (local_220 < param_1[4]);
        }
        break;
      case 0x18:
        local_220 = 0;
        if (0 < param_1[4]) {
          iVar9 = param_1[3];
          do {
            iVar16 = 0;
            pbVar12 = (byte *)(*(int *)(iVar10 + 0x10) * local_220 + *(int *)(iVar10 + 0x14));
            puVar13 = (ushort *)(param_1[6] * local_220 + param_1[1]);
            if (0 < iVar9) {
              do {
                *puVar13 = ((ushort)(*pbVar12 & 0xf8) << 5 | (ushort)(pbVar12[1] & 0xf8)) << 2 |
                           (ushort)(pbVar12[2] >> 3);
                iVar9 = param_1[3];
                iVar16 = iVar16 + 1;
                pbVar12 = pbVar12 + 4;
                puVar13 = puVar13 + 1;
              } while (iVar16 < iVar9);
            }
            local_220 = local_220 + 1;
          } while (local_220 < param_1[4]);
        }
        break;
      case 0x19:
        local_220 = 0;
        if (0 < param_1[4]) {
          iVar9 = param_1[3];
          do {
            pbVar12 = (byte *)(*(int *)(iVar10 + 0x10) * local_220 + *(int *)(iVar10 + 0x14));
            iVar16 = 0;
            puVar13 = (ushort *)(param_1[6] * local_220 + param_1[1]);
            if (0 < iVar9) {
              do {
                pbVar1 = pbVar12 + 3;
                bVar4 = *pbVar12;
                pbVar2 = pbVar12 + 1;
                pbVar3 = pbVar12 + 2;
                pbVar12 = pbVar12 + 4;
                *puVar13 = (((*pbVar1 & 0xff80) * 2 | (ushort)(bVar4 & 0xf8)) << 5 |
                           (ushort)(*pbVar2 & 0xf8)) << 2 | (ushort)(*pbVar3 >> 3);
                iVar9 = param_1[3];
                iVar16 = iVar16 + 1;
                puVar13 = puVar13 + 1;
              } while (iVar16 < iVar9);
            }
            local_220 = local_220 + 1;
          } while (local_220 < param_1[4]);
        }
        break;
      case 0x1a:
        local_220 = 0;
        if (0 < param_1[4]) {
          iVar9 = param_1[3];
          do {
            pbVar12 = (byte *)(*(int *)(iVar10 + 0x10) * local_220 + *(int *)(iVar10 + 0x14));
            iVar16 = 0;
            puVar13 = (ushort *)(param_1[6] * local_220 + param_1[1]);
            if (0 < iVar9) {
              do {
                pbVar1 = pbVar12 + 3;
                bVar4 = *pbVar12;
                pbVar2 = pbVar12 + 2;
                pbVar3 = pbVar12 + 1;
                pbVar12 = pbVar12 + 4;
                iVar16 = iVar16 + 1;
                *puVar13 = ((*pbVar1 & 0xfff0) << 4 | (ushort)(bVar4 & 0xf0)) << 4 |
                           (ushort)(byte)(*pbVar3 & 0xf0 | *pbVar2 >> 4);
                iVar9 = param_1[3];
                puVar13 = puVar13 + 1;
              } while (iVar16 < iVar9);
            }
            local_220 = local_220 + 1;
          } while (local_220 < param_1[4]);
        }
        break;
      default:
        local_210 = (code *)FUN_004df750(uVar5);
        local_220 = 0;
        if (0 < param_1[4]) {
          iVar9 = param_1[3];
          do {
            iVar16 = *(int *)(iVar10 + 0x10) * local_220 + *(int *)(iVar10 + 0x14);
            puVar15 = (undefined2 *)(param_1[6] * local_220 + param_1[1]);
            local_21c = 0;
            if (0 < iVar9) {
              do {
                uVar8 = (*local_210)(iVar16);
                *puVar15 = uVar8;
                iVar9 = param_1[3];
                iVar16 = iVar16 + 4;
                puVar15 = puVar15 + 1;
                local_21c = local_21c + 1;
              } while (local_21c < iVar9);
            }
            local_220 = local_220 + 1;
          } while (local_220 < param_1[4]);
        }
      }
    }
    break;
  case 0x18:
  case 0x20:
    FUN_004e02d0(param_1,iVar10);
    break;
  case 0x40:
  case 0x80:
    break;
  }
  if ((local_208 != 0) && (!bVar6)) {
    FUN_004c7620(param_1);
  }
  if (!bVar7) {
    FUN_004c7600(param_1);
  }
  if (iVar10 != param_2) {
    FUN_004cdd00(iVar10);
  }
  return 1;
}
