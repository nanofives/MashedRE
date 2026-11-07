// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004e02d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
#define CONCAT21(a,b) ((uint)(((uint)(ushort)(a)<<8)|(uchar)(b)))
#define CONCAT31(a,b) ((uint)(((uint)(a)<<8)|(uchar)(b)))
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(undefined4, DAT_00911b00, 0x00911b00);
MGLOBAL(undefined1, FUN_004dfb60, 0x004dfb60);
MGLOBAL(undefined1, FUN_004df870, 0x004df870);
MGLOBAL(undefined1, FUN_004df8a0, 0x004df8a0);
MGLOBAL(undefined1, FUN_004df8d0, 0x004df8d0);
MGLOBAL(undefined1, FUN_004df910, 0x004df910);
MGLOBAL(undefined1, FUN_004df950, 0x004df950);
MGLOBAL(undefined1, FUN_004df980, 0x004df980);
MGLOBAL(undefined1, FUN_004df9b0, 0x004df9b0);
MGLOBAL(undefined1, FUN_004df9e0, 0x004df9e0);
MGLOBAL(undefined1, FUN_004dfa10, 0x004dfa10);
MGLOBAL(undefined1, FUN_004dfa40, 0x004dfa40);
MGLOBAL(undefined1, FUN_004dfa70, 0x004dfa70);
MGLOBAL(undefined1, FUN_004dfaa0, 0x004dfaa0);
MGLOBAL(undefined1, FUN_004dfab0, 0x004dfab0);
MGLOBAL(undefined1, FUN_004dfb10, 0x004dfb10);
// MATCH: 0x004e02d0 FUN_004e02d0
void FUN_004e02d0(int *param_1,int param_2)

{
  int iVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  uint uVar4;
  byte *pbVar5;
  byte bVar6;
  uint *puVar7;
  byte bVar8;
  int iVar9;
  int iVar10;
  undefined4 *puVar11;
  code *pcVar12;
  int iVar13;
  char local_40d;
  code *local_40c;
  int local_408;
  uint local_400 [256];
  
  iVar1 = DAT_00911ae4;
  iVar10 = *(int *)(param_2 + 0xc);
  if ((iVar10 == 4) || (iVar10 == 8)) {
    bVar8 = *(byte *)((int)param_1 + 0x23) & 0xf;
    bVar6 = (byte)iVar10;
    if (bVar8 == 6) {
      iVar10 = 0;
      iVar9 = 1 << (bVar6 & 0x1f);
      local_40d = -1;
      if (0 < iVar9) {
        iVar13 = *(int *)(param_2 + 0x18);
        pbVar5 = (byte *)(iVar13 + 2);
        do {
          local_400[iVar10] =
               ((*(byte *)(iVar13 + iVar10 * 4) | 0xffffff00) << 8 | (uint)pbVar5[-1]) << 8 |
               (uint)*pbVar5;
          iVar10 = iVar10 + 1;
          pbVar5 = pbVar5 + 4;
        } while (iVar10 < iVar9);
      }
    }
    else if (bVar8 == 5) {
      iVar10 = 0;
      iVar9 = 1 << (bVar6 & 0x1f);
      local_40d = '\0';
      if (0 < iVar9) {
        iVar13 = *(int *)(param_2 + 0x18);
        puVar2 = (undefined1 *)(iVar13 + 1);
        do {
          local_400[iVar10] =
               CONCAT31(CONCAT21(CONCAT11(puVar2[2],*(undefined1 *)(iVar13 + iVar10 * 4)),*puVar2),
                        puVar2[1]);
          iVar10 = iVar10 + 1;
          puVar2 = puVar2 + 4;
        } while (iVar10 < iVar9);
      }
    }
    else {
      iVar10 = *(int *)((int)param_1 + DAT_00911ae4 + 0x18);
      pcVar12 = (code *)0x0;
      switch(iVar10) {
      case 0x15:
        pcVar12 = FUN_004dfa40;
        break;
      case 0x16:
        pcVar12 = FUN_004df9e0;
        break;
      case 0x17:
        pcVar12 = FUN_004df8a0;
        break;
      case 0x18:
        pcVar12 = FUN_004df870;
        break;
      case 0x19:
        pcVar12 = FUN_004df8d0;
        break;
      case 0x1a:
        pcVar12 = FUN_004df910;
        break;
      case 0x1d:
        pcVar12 = FUN_004df950;
        break;
      case 0x1e:
        pcVar12 = FUN_004df980;
        break;
      case 0x20:
      case 0x3f:
        pcVar12 = FUN_004dfa70;
        break;
      case 0x21:
      case 0x3e:
        pcVar12 = FUN_004dfa10;
        break;
      case 0x32:
        pcVar12 = FUN_004dfab0;
        break;
      case 0x33:
        pcVar12 = FUN_004dfb10;
        break;
      case 0x3c:
      case 0x75:
        pcVar12 = FUN_004dfaa0;
        break;
      case 0x3d:
        pcVar12 = FUN_004df9b0;
        break;
      case 0x51:
        pcVar12 = FUN_004dfb60;
      }
      local_40d = *(char *)(&DAT_00911b00 + iVar10);
      iVar10 = 0;
      if (0 < 1 << (bVar6 & 0x1f)) {
        do {
          uVar4 = (*pcVar12)(*(int *)(param_2 + 0x18) + iVar10 * 4);
          local_400[iVar10] = uVar4;
          iVar10 = iVar10 + 1;
        } while (iVar10 < 1 << ((byte)*(undefined4 *)(param_2 + 0xc) & 0x1f));
      }
    }
    iVar10 = 0;
    if (0 < param_1[4]) {
      iVar9 = param_1[3];
      do {
        pbVar5 = (byte *)(*(int *)(param_2 + 0x10) * iVar10 + *(int *)(param_2 + 0x14));
        iVar13 = 0;
        puVar7 = (uint *)(param_1[6] * iVar10 + param_1[1]);
        if (0 < iVar9) {
          do {
            bVar6 = *pbVar5;
            pbVar5 = pbVar5 + 1;
            iVar13 = iVar13 + 1;
            *puVar7 = local_400[bVar6];
            iVar9 = param_1[3];
            puVar7 = puVar7 + 1;
          } while (iVar13 < iVar9);
        }
        iVar10 = iVar10 + 1;
      } while (iVar10 < param_1[4]);
    }
  }
  else {
    if (iVar10 != 0x20) goto LAB_004e0737;
    bVar6 = *(byte *)((int)param_1 + 0x23) & 0xf;
    if (bVar6 == 6) {
      iVar10 = 0;
      if (0 < param_1[4]) {
        iVar9 = param_1[3];
        do {
          iVar13 = 0;
          pbVar5 = (byte *)(*(int *)(param_2 + 0x10) * iVar10 + *(int *)(param_2 + 0x14));
          puVar7 = (uint *)(param_1[6] * iVar10 + param_1[1]);
          if (0 < iVar9) {
            do {
              iVar13 = iVar13 + 1;
              *puVar7 = ((*pbVar5 | 0xffffff00) << 8 | (uint)pbVar5[1]) << 8 | (uint)pbVar5[2];
              iVar9 = param_1[3];
              pbVar5 = pbVar5 + 4;
              puVar7 = puVar7 + 1;
            } while (iVar13 < iVar9);
          }
          iVar10 = iVar10 + 1;
        } while (iVar10 < param_1[4]);
      }
      goto LAB_004e0388;
    }
    if (bVar6 == 5) {
      iVar10 = 0;
      if (0 < param_1[4]) {
        iVar9 = param_1[3];
        do {
          iVar13 = 0;
          puVar2 = (undefined1 *)(*(int *)(param_2 + 0x10) * iVar10 + *(int *)(param_2 + 0x14));
          puVar11 = (undefined4 *)(param_1[6] * iVar10 + param_1[1]);
          if (0 < iVar9) {
            do {
              iVar13 = iVar13 + 1;
              *puVar11 = CONCAT31(CONCAT21(CONCAT11(puVar2[3],*puVar2),puVar2[1]),puVar2[2]);
              iVar9 = param_1[3];
              puVar2 = puVar2 + 4;
              puVar11 = puVar11 + 1;
            } while (iVar13 < iVar9);
          }
          iVar10 = iVar10 + 1;
        } while (iVar10 < param_1[4]);
      }
      goto LAB_004e0737;
    }
    iVar10 = *(int *)((int)param_1 + DAT_00911ae4 + 0x18);
    local_40c = (code *)0x0;
    switch(iVar10) {
    case 0x15:
      local_40c = FUN_004dfa40;
      break;
    case 0x16:
      local_40c = FUN_004df9e0;
      break;
    case 0x17:
      local_40c = FUN_004df8a0;
      break;
    case 0x18:
      local_40c = FUN_004df870;
      break;
    case 0x19:
      local_40c = FUN_004df8d0;
      break;
    case 0x1a:
      local_40c = FUN_004df910;
      break;
    case 0x1d:
      local_40c = FUN_004df950;
      break;
    case 0x1e:
      local_40c = FUN_004df980;
      break;
    case 0x20:
    case 0x3f:
      local_40c = FUN_004dfa70;
      break;
    case 0x21:
    case 0x3e:
      local_40c = FUN_004dfa10;
      break;
    case 0x32:
      local_40c = FUN_004dfab0;
      break;
    case 0x33:
      local_40c = FUN_004dfb10;
      break;
    case 0x3c:
    case 0x75:
      local_40c = FUN_004dfaa0;
      break;
    case 0x3d:
      local_40c = FUN_004df9b0;
      break;
    case 0x51:
      local_40c = FUN_004dfb60;
    }
    local_40d = *(char *)(&DAT_00911b00 + iVar10);
    local_408 = 0;
    if (0 < param_1[4]) {
      iVar10 = param_1[3];
      do {
        iVar13 = *(int *)(param_2 + 0x10) * local_408 + *(int *)(param_2 + 0x14);
        puVar11 = (undefined4 *)(param_1[6] * local_408 + param_1[1]);
        iVar9 = 0;
        if (0 < iVar10) {
          do {
            uVar3 = (*local_40c)(iVar13);
            *puVar11 = uVar3;
            iVar10 = param_1[3];
            iVar13 = iVar13 + 4;
            puVar11 = puVar11 + 1;
            iVar9 = iVar9 + 1;
          } while (iVar9 < iVar10);
        }
        local_408 = local_408 + 1;
      } while (local_408 < param_1[4]);
    }
  }
  if (local_40d == -1) {
LAB_004e0388:
    *(undefined1 *)((int)param_1 + iVar1 + 8) = 0;
    return;
  }
LAB_004e0737:
  *(undefined1 *)((int)param_1 + iVar1 + 8) = 1;
  if (param_1 == (int *)*param_1) {
    return;
  }
  *(undefined1 *)(*param_1 + DAT_00911ae4 + 8) = 1;
  return;
}
