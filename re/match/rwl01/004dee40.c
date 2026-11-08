// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004dee40: 22.5% regs 51.0%.  class (b) Raster format classifier; raw Ghidra form kept with goto-merged tests. Original uses a different loop/register structure (ebp/ebx/esi/edi all live). Not reached in budget.. */
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
MGLOBAL(int, DAT_00911f94, 0x00911f94);
bool FUN_004d38b0(undefined4 p0);
// MATCH: 0x004dee40 FUN_004dee40
int FUN_004dee40(int param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  byte *pbVar4;
  byte *pbVar5;
  char *pcVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  
  iVar3 = param_1;
  iVar9 = *(int *)(param_1 + 0xc);
  if ((iVar9 != 4) && (iVar9 != 8)) {
    piVar1 = (int *)(param_1 + 8);
    piVar2 = (int *)(param_1 + 4);
    iVar9 = *(int *)(param_1 + 0x14);
    uVar8 = 0;
    param_1 = 0;
    if (0 < *piVar1) {
      do {
        iVar7 = 0;
        if (0 < *piVar2) {
          pbVar5 = (byte *)(iVar9 + 3);
          do {
            if (*pbVar5 != 0xff) {
              if (0xf < *pbVar5) goto LAB_004deede;
              uVar8 = 1;
            }
            pbVar5 = pbVar5 + 4;
            iVar7 = iVar7 + 1;
          } while (iVar7 < *piVar2);
        }
        if (7 < uVar8) {
LAB_004deede:
          if ((0x10 < DAT_00911f94) && (iVar9 = FUN_004d38b0(0x15), iVar9 != 0)) {
            return 0x500;
          }
          iVar9 = FUN_004d38b0(0x1a);
          return (-(uint)(iVar9 != 0) & 0x200) + 0x100;
        }
        iVar9 = iVar9 + *(int *)(iVar3 + 0x10);
        param_1 = param_1 + 1;
      } while (param_1 < *(int *)(iVar3 + 8));
      if (uVar8 != 0) {
        if (DAT_00911f94 < 0x11) {
          return 0x100;
        }
        iVar9 = FUN_004d38b0(0x15);
        return (-(uint)(iVar9 != 0) & 0x400) + 0x100;
      }
    }
    if (DAT_00911f94 < 0x11) {
      return 0x200;
    }
    iVar9 = FUN_004d38b0(0x16);
    return (-(uint)(iVar9 != 0) & 0x400) + 0x200;
  }
  pbVar5 = *(byte **)(param_1 + 0x14);
  iVar3 = 0;
  pcVar6 = (char *)(*(int *)(param_1 + 0x18) + 3);
  if (iVar9 == 4) {
    while (*pcVar6 == -1) {
      iVar3 = iVar3 + 1;
      pcVar6 = pcVar6 + 4;
      if (0xf < iVar3) {
        return 0x4600;
      }
    }
LAB_004defc4:
    iVar3 = 0;
    if (0 < *(int *)(param_1 + 8)) {
      do {
        iVar7 = 0;
        pbVar4 = pbVar5;
        if (0 < *(int *)(param_1 + 4)) {
          do {
            if (*(char *)(*(int *)(param_1 + 0x18) + 3 + (uint)*pbVar4 * 4) != -1) {
              return (-(uint)(iVar9 != 4) & 0xffffe000) + 0x4500;
            }
            pbVar4 = pbVar4 + 1;
            iVar7 = iVar7 + 1;
          } while (iVar7 < *(int *)(param_1 + 4));
        }
        pbVar5 = pbVar5 + *(int *)(param_1 + 0x10);
        iVar3 = iVar3 + 1;
      } while (iVar3 < *(int *)(param_1 + 8));
    }
  }
  else {
    do {
      if (*pcVar6 != -1) goto LAB_004defc4;
      iVar3 = iVar3 + 1;
      pcVar6 = pcVar6 + 4;
    } while (iVar3 < 0x100);
  }
  if (iVar9 != 4) {
    return 0x2600;
  }
  return 0x4600;
}
