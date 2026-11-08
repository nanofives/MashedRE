// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004de390: best 45.0% (regs ~50%), class (b). Raster-format converter (per-format pixel loops, 1087 B). Done: (flags & 0xf) << 8 as the format word (shl eax,8 in the original) and the compares against 0x100/0x200/0x300/0xa00, error pair as an array. Not done: loop shapes and register assignment of the per-format inner loops; budget (45 min cap) spent on the smaller rows. */
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
typedef unsigned short ushort;
typedef unsigned char byte;
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
void FUN_004de800(int p0, int * p1);
// MATCH: 0x004de390 FUN_004de390
void FUN_004de390(int param_1,int *param_2)

{
  byte bVar1;
  ushort uVar2;
  undefined4 uVar3;
  uint uVar4;
  byte *pbVar5;
  undefined1 *puVar6;
  undefined4 *puVar7;
  int iVar8;
  int iVar9;
  ushort *puVar10;
  int iVar11;
  uint *puVar12;
  undefined4 loc[2];
  
  uVar4 = (*(byte *)((int)param_2 + 0x23) & 0xf) << 8;
  if (uVar4 != 0) {
    switch(param_2[5]) {
    case 4:
    case 8:
      iVar8 = param_2[2];
      iVar11 = 0;
      if (iVar8 == 0) {
        if (0 < param_2[4]) {
          do {
            pbVar5 = (byte *)(param_2[6] * iVar11 + param_2[1]);
            iVar8 = param_2[3];
            puVar12 = (uint *)(*(int *)(param_1 + 0x10) * iVar11 + *(int *)(param_1 + 0x14));
            do {
              uVar4 = (uint)*pbVar5;
              pbVar5 = pbVar5 + 1;
              *puVar12 = ((uVar4 | 0xffffff00) << 8 | uVar4) << 8 | uVar4;
              iVar8 = iVar8 + -1;
              puVar12 = puVar12 + 1;
            } while (iVar8 != 0);
            iVar11 = iVar11 + 1;
          } while (iVar11 < param_2[4]);
          return;
        }
      }
      else if (0 < param_2[4]) {
        do {
          pbVar5 = (byte *)(param_2[6] * iVar11 + param_2[1]);
          iVar9 = param_2[3];
          puVar7 = (undefined4 *)(*(int *)(param_1 + 0x10) * iVar11 + *(int *)(param_1 + 0x14));
          do {
            bVar1 = *pbVar5;
            pbVar5 = pbVar5 + 1;
            iVar9 = iVar9 + -1;
            *puVar7 = *(undefined4 *)(iVar8 + (uint)bVar1 * 4);
            puVar7 = puVar7 + 1;
          } while (iVar9 != 0);
          iVar11 = iVar11 + 1;
        } while (iVar11 < param_2[4]);
        return;
      }
      break;
    default:
      loc[0] = 1;
      loc[1] = FUN_004d7ff0(0x8000000c);
      FUN_004d8480(loc);
      break;
    case 0x10:
      if (uVar4 == 0x300 || uVar4 < 0x300) {
        if (uVar4 == 0x300) {
          iVar8 = 0;
          if (0 < param_2[4]) {
            do {
              puVar10 = (ushort *)(param_2[6] * iVar8 + param_2[1]);
              iVar11 = param_2[3];
              pbVar5 = (byte *)(*(int *)(param_1 + 0x10) * iVar8 + *(int *)(param_1 + 0x14));
              do {
                uVar2 = *puVar10;
                *pbVar5 = (byte)(uVar2 >> 4) & 0xf0;
                pbVar5[1] = (byte)uVar2 & 0xf0;
                pbVar5[2] = (byte)uVar2 << 4;
                if ((uVar2 & 0xf000) == 0xf000) {
                  pbVar5[3] = 0xff;
                }
                else {
                  pbVar5[3] = (byte)(uVar2 >> 8) & 0xf0;
                }
                puVar10 = puVar10 + 1;
                pbVar5 = pbVar5 + 4;
                iVar11 = iVar11 + -1;
              } while (iVar11 != 0);
              iVar8 = iVar8 + 1;
            } while (iVar8 < param_2[4]);
            return;
          }
        }
        else if (uVar4 == 0x100) {
          iVar8 = 0;
          if (0 < param_2[4]) {
            do {
              puVar10 = (ushort *)(param_2[6] * iVar8 + param_2[1]);
              iVar11 = param_2[3];
              pbVar5 = (byte *)(*(int *)(param_1 + 0x10) * iVar8 + *(int *)(param_1 + 0x14));
              do {
                uVar2 = *puVar10;
                *pbVar5 = (byte)(uVar2 >> 7) & 0xf8;
                pbVar5[1] = (byte)(uVar2 >> 2) & 0xf8;
                pbVar5[2] = (char)uVar2 << 3;
                if ((short)uVar2 < 0) {
                  pbVar5[3] = 0xff;
                }
                else {
                  pbVar5[3] = 0;
                }
                puVar10 = puVar10 + 1;
                pbVar5 = pbVar5 + 4;
                iVar11 = iVar11 + -1;
              } while (iVar11 != 0);
              iVar8 = iVar8 + 1;
            } while (iVar8 < param_2[4]);
            return;
          }
        }
        else if ((uVar4 == 0x200) && (iVar8 = 0, 0 < param_2[4])) {
          do {
            puVar10 = (ushort *)(param_2[6] * iVar8 + param_2[1]);
            iVar11 = param_2[3];
            pbVar5 = (byte *)(*(int *)(param_1 + 0x10) * iVar8 + *(int *)(param_1 + 0x14));
            do {
              uVar2 = *puVar10;
              pbVar5[3] = 0xff;
              puVar10 = puVar10 + 1;
              *pbVar5 = (byte)(uVar2 >> 8) & 0xf8;
              pbVar5[1] = (byte)(uVar2 >> 3) & 0xfc;
              pbVar5[2] = (char)uVar2 << 3;
              pbVar5 = pbVar5 + 4;
              iVar11 = iVar11 + -1;
            } while (iVar11 != 0);
            iVar8 = iVar8 + 1;
          } while (iVar8 < param_2[4]);
          return;
        }
      }
      else if ((uVar4 == 0xa00) && (iVar8 = 0, 0 < param_2[4])) {
        do {
          puVar10 = (ushort *)(param_2[6] * iVar8 + param_2[1]);
          iVar11 = param_2[3];
          pbVar5 = (byte *)(*(int *)(param_1 + 0x10) * iVar8 + *(int *)(param_1 + 0x14));
          do {
            uVar2 = *puVar10;
            pbVar5[3] = 0xff;
            puVar10 = puVar10 + 1;
            *pbVar5 = (byte)(uVar2 >> 7) & 0xf8;
            pbVar5[1] = (byte)(uVar2 >> 2) & 0xf8;
            pbVar5[2] = (char)uVar2 << 3;
            pbVar5 = pbVar5 + 4;
            iVar11 = iVar11 + -1;
          } while (iVar11 != 0);
          iVar8 = iVar8 + 1;
        } while (iVar8 < param_2[4]);
        return;
      }
      break;
    case 0x18:
    case 0x20:
      iVar8 = *(int *)(*param_2 + DAT_00911ae4 + 0x18);
      if (iVar8 == 0x16) {
        iVar8 = 0;
        if (0 < param_2[4]) {
          do {
            puVar7 = (undefined4 *)(param_2[6] * iVar8 + param_2[1]);
            puVar6 = (undefined1 *)(*(int *)(param_1 + 0x10) * iVar8 + *(int *)(param_1 + 0x14));
            iVar11 = param_2[3];
            do {
              uVar3 = *puVar7;
              puVar6[3] = 0xff;
              puVar6[2] = (char)uVar3;
              *puVar6 = (char)((uint)uVar3 >> 0x10);
              puVar6[1] = (char)((uint)uVar3 >> 8);
              puVar7 = puVar7 + 1;
              puVar6 = puVar6 + 4;
              iVar11 = iVar11 + -1;
            } while (iVar11 != 0);
            iVar8 = iVar8 + 1;
          } while (iVar8 < param_2[4]);
          return;
        }
      }
      else if (iVar8 == 0x15) {
        iVar8 = 0;
        if (0 < param_2[4]) {
          do {
            puVar7 = (undefined4 *)(param_2[6] * iVar8 + param_2[1]);
            iVar11 = param_2[3];
            puVar6 = (undefined1 *)(*(int *)(param_1 + 0x10) * iVar8 + *(int *)(param_1 + 0x14));
            do {
              uVar3 = *puVar7;
              puVar7 = puVar7 + 1;
              puVar6[2] = (char)uVar3;
              *puVar6 = (char)((uint)uVar3 >> 0x10);
              puVar6[1] = (char)((uint)uVar3 >> 8);
              puVar6[3] = (char)((uint)uVar3 >> 0x18);
              puVar6 = puVar6 + 4;
              iVar11 = iVar11 + -1;
            } while (iVar11 != 0);
            iVar8 = iVar8 + 1;
          } while (iVar8 < param_2[4]);
          return;
        }
      }
      else {
        if (iVar8 != 0x23) goto LAB_004de3a6;
        iVar8 = 0;
        if (0 < param_2[4]) {
          do {
            puVar12 = (uint *)(param_2[6] * iVar8 + param_2[1]);
            iVar11 = param_2[3];
            puVar6 = (undefined1 *)(*(int *)(param_1 + 0x10) * iVar8 + *(int *)(param_1 + 0x14));
            do {
              uVar4 = *puVar12;
              puVar12 = puVar12 + 1;
              *puVar6 = (char)(uVar4 >> 0x16);
              puVar6[1] = (char)(uVar4 >> 0xc);
              puVar6[2] = (char)(uVar4 >> 2);
              puVar6[3] = (byte)(uVar4 >> 0x1e) * 'U';
              iVar11 = iVar11 + -1;
              puVar6 = puVar6 + 4;
            } while (iVar11 != 0);
            iVar8 = iVar8 + 1;
          } while (iVar8 < param_2[4]);
          return;
        }
      }
    }
    return;
  }
LAB_004de3a6:
  FUN_004de800(param_1,param_2);
  return;
}
