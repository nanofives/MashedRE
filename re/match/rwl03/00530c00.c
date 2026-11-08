// FLAGS: /O2 /GS /Ob1
/* MATCH-RWL03 0x00530c00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
MGLOBAL(int, DAT_007dc508, 0x007dc508);
bool FUN_004cb8a0();
undefined * FUN_004cbb60(void);
undefined4 FUN_004dc5b0(uint p0, uint p1, int * p2, int * p3);
void FUN_004dc750(int p0, int p1, int p2, int p3);
undefined4 FUN_004dcaa0(uint * p0, uint * p1);
void FUN_004dcb80(int p0);
void FUN_004f4200(int p0);
undefined4 FUN_004f5020(int p0);
char FUN_004f8490(undefined4 p0, undefined4 p1);
undefined4 FUN_004f8660(int p0);
undefined4 FUN_00530010(int p0);
// MATCH: 0x00530c00 FUN_00530c00
undefined4 FUN_00530c00(int param_1,int param_2,int param_3)

{
  undefined2 uVar1;
  undefined4 uVar2;
  byte bVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int *piVar9;
  undefined4 *puVar10;
  byte bVar11;
  uint uVar12;
  uint uVar13;
  int iVar14;
  uint local_a4;
  undefined4 *local_a0;
  undefined4 local_9c;
  undefined4 local_98;
  undefined2 local_94;
  undefined2 local_92;
  byte local_90 [4];
  undefined2 local_8c [2];
  undefined1 local_88 [132];
  int local_4;
  
  iVar4 = FUN_004cbb60();
  iVar7 = *(int *)(param_1 + 0x18);
  local_98 = *(int *)(DAT_007dc508 + iVar7);
  uVar13 = *(uint *)(iVar7 + 8);
  local_4 = iVar7;
  uVar5 = FUN_004f8660(iVar7);
  local_9c = *(uint *)(iVar7 + 0x1c);
  piVar9 = (int *)(param_2 + 0x18);
  iVar14 = 2;
  do {
    iVar6 = piVar9[-2];
    if (iVar6 != 0) {
      if (*(char *)((int)piVar9 + 6) == '\0') {
        if ((piVar9[-1] == 0) && (*(char *)((int)piVar9 + 7) == '\0')) {
          FUN_004dcb80(iVar6);
        }
      }
      else {
        FUN_004dc750(*piVar9,*(int *)(param_2 + 0x3c) * *piVar9,iVar6,piVar9[-1]);
      }
      piVar9[-2] = 0;
    }
    piVar9[-1] = 0;
    *piVar9 = 0;
    *(undefined2 *)(piVar9 + 1) = 0;
    *(undefined1 *)((int)piVar9 + 6) = 0;
    *(undefined1 *)((int)piVar9 + 7) = 0;
    piVar9 = piVar9 + 4;
    iVar14 = iVar14 + -1;
  } while (iVar14 != 0);
  iVar14 = param_2 + 0x10;
  if (((uVar5 & 0xfff) == 0) &&
     ((((local_98 == 0 || (*(int *)(local_98 + 0x20) != 0)) || (param_3 == 0)) ||
      (iVar6 = FUN_00530010(param_1,param_2), iVar6 != 0)))) {
    uVar12 = 1;
    local_a4 = 1;
    *(undefined1 *)(param_2 + 0x1e) = 1;
  }
  else {
    local_a4 = 2;
    *(undefined1 *)(param_2 + 0x1e) = 0;
    *(undefined1 *)(param_2 + 0x2e) = 1;
    FUN_004f4200(iVar7);
    uVar12 = 2;
  }
  if ((local_98 == 0) || (*(int *)(local_98 + 0x20) == 0)) {
    local_92 = *(undefined2 *)(param_2 + 0x18);
    local_94 = 0;
    local_90[0] = 2;
    local_90[1] = 0;
    local_90[2] = 0;
    local_90[3] = 0;
    iVar7 = 1;
    *(undefined4 *)(param_2 + 0x18) = 0xc;
    *(undefined2 *)(param_2 + 0x1c) = 2;
    if ((uVar13 & 0x10) != 0) {
      *(int *)(param_2 + 0x18) = *(int *)(param_2 + 0x18) + 0xc;
      *(ushort *)(param_2 + 0x1c) = *(ushort *)(param_2 + 0x1c) | 4;
      local_8c[0] = 0;
      local_8c[1] = 0xc;
      local_88[0] = 2;
      local_88[1] = 0;
      local_88[2] = 3;
      local_88[3] = 0;
      iVar7 = 2;
    }
    if ((uVar13 & 8) != 0) {
      if ((uVar5 & 8) == 0) {
        iVar6 = uVar12 - 1;
      }
      else {
        iVar6 = 0;
      }
      (&local_94)[iVar7 * 4] = (short)iVar6;
      iVar6 = iVar6 * 0x10 + iVar14;
      iVar7 = iVar7 + 1;
      uVar1 = *(undefined2 *)(iVar6 + 8);
      *(undefined1 *)(&local_98 + iVar7 * 2) = 4;
      *(undefined2 *)((int)&local_9c + iVar7 * 8 + 2) = uVar1;
      *(undefined1 *)((int)&local_98 + iVar7 * 8 + 1) = 0;
      *(undefined1 *)((int)&local_98 + iVar7 * 8 + 2) = 10;
      *(undefined1 *)((int)&local_98 + iVar7 * 8 + 3) = 0;
      *(byte *)(iVar6 + 0xc) = *(byte *)(iVar6 + 0xc) | 8;
      *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + 4;
    }
    uVar13 = 0;
    if (local_9c != 0) {
      do {
        bVar11 = (byte)uVar13;
        if ((uVar5 & 0x10 << (bVar11 & 0x1f)) == 0) {
          iVar6 = local_a4 - 1;
        }
        else {
          iVar6 = 0;
        }
        (&local_94)[iVar7 * 4] = (short)iVar6;
        iVar6 = iVar6 * 0x10 + iVar14;
        iVar7 = iVar7 + 1;
        uVar1 = *(undefined2 *)(iVar6 + 8);
        *(undefined1 *)(&local_98 + iVar7 * 2) = 1;
        *(undefined2 *)((int)&local_9c + iVar7 * 8 + 2) = uVar1;
        *(undefined1 *)((int)&local_98 + iVar7 * 8 + 1) = 0;
        *(undefined1 *)((int)&local_98 + iVar7 * 8 + 2) = 5;
        *(byte *)((int)&local_98 + iVar7 * 8 + 3) = bVar11;
        *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + 8;
        *(ushort *)(iVar6 + 0xc) = *(ushort *)(iVar6 + 0xc) | (ushort)(0x10 << (bVar11 & 0x1f));
        uVar13 = uVar13 + 1;
      } while (uVar13 < local_9c);
    }
    goto LAB_00531195;
  }
  if ((uVar5 & 2) == 0) {
    iVar7 = uVar12 - 1;
  }
  else {
    iVar7 = 0;
  }
  local_94 = (undefined2)iVar7;
  iVar6 = iVar7 * 0x10 + iVar14;
  iVar7 = 1;
  local_92 = *(undefined2 *)(iVar6 + 8);
  local_90[0] = 2;
  local_90[1] = 0;
  local_90[2] = 0;
  local_90[3] = 0;
  *(undefined4 *)(iVar6 + 8) = 0xc;
  *(undefined2 *)(iVar6 + 0xc) = 2;
  if ((uVar13 & 0x10) != 0) {
    if ((uVar5 & 4) == 0) {
      iVar7 = uVar12 - 1;
    }
    else {
      iVar7 = 0;
    }
    local_8c[0] = (undefined2)iVar7;
    iVar7 = iVar7 * 0x10 + iVar14;
    local_8c[1] = *(undefined2 *)(iVar7 + 8);
    if ((*(byte *)(iVar4 + 0xec) & 0x80) == 0) {
      local_88[0] = 2;
      iVar6 = *(int *)(iVar7 + 8) + 0xc;
    }
    else {
      local_88[0] = 0xe;
      iVar6 = *(int *)(iVar7 + 8) + 4;
    }
    *(int *)(iVar7 + 8) = iVar6;
    local_88[1] = 0;
    local_88[2] = 3;
    local_88[3] = 0;
    *(ushort *)(iVar7 + 0xc) = *(ushort *)(iVar7 + 0xc) | 4;
    iVar7 = 2;
  }
  if ((uVar5 & 0x10000) != 0) {
    if ((uVar5 & 0xff2) == 0) {
      iVar6 = uVar12 - 1;
    }
    else {
      iVar6 = 0;
    }
    (&local_94)[iVar7 * 4] = (short)iVar6;
    piVar9 = (int *)(iVar6 * 0x10 + 8 + iVar14);
    *(undefined2 *)(local_90 + iVar7 * 8 + -2) = *(undefined2 *)(iVar6 * 0x10 + 8 + iVar14);
    if ((*(byte *)(iVar4 + 0xec) & 0x80) == 0) {
      local_90[iVar7 * 8] = 2;
      *piVar9 = *piVar9 + 0xc;
    }
    else {
      local_90[iVar7 * 8] = 0xe;
      *piVar9 = *piVar9 + 4;
    }
    local_90[iVar7 * 8 + 1] = 0;
    local_90[iVar7 * 8 + 2] = 6;
    *(undefined1 *)((int)local_8c + iVar7 * 8 + -1) = 0;
    iVar7 = iVar7 + 1;
    uVar12 = local_a4;
  }
  if ((uVar13 & 8) != 0) {
    if ((uVar5 & 8) == 0) {
      iVar6 = uVar12 - 1;
    }
    else {
      iVar6 = 0;
    }
    (&local_94)[iVar7 * 4] = (short)iVar6;
    iVar6 = iVar6 * 0x10 + iVar14;
    iVar7 = iVar7 + 1;
    uVar1 = *(undefined2 *)(iVar6 + 8);
    *(undefined1 *)(&local_98 + iVar7 * 2) = 4;
    *(undefined2 *)((int)&local_9c + iVar7 * 8 + 2) = uVar1;
    *(undefined1 *)((int)&local_98 + iVar7 * 8 + 1) = 0;
    *(undefined1 *)((int)&local_98 + iVar7 * 8 + 2) = 10;
    *(undefined1 *)((int)&local_98 + iVar7 * 8 + 3) = 0;
    *(byte *)(iVar6 + 0xc) = *(byte *)(iVar6 + 0xc) | 8;
    *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + 4;
  }
  uVar13 = 0;
  if (local_9c != 0) {
    local_a0 = (undefined4 *)(local_4 + 0x34);
    do {
      bVar11 = (byte)uVar13;
      if ((uVar5 & 0x10 << (bVar11 & 0x1f)) == 0) {
        iVar6 = local_a4 - 1;
      }
      else {
        iVar6 = 0;
      }
      (&local_94)[iVar7 * 4] = (short)iVar6;
      uVar2 = *(undefined4 *)(param_2 + 0x3c);
      iVar6 = iVar6 * 0x10 + iVar14;
      *(undefined2 *)(local_90 + iVar7 * 8 + -2) = *(undefined2 *)(iVar6 + 8);
      bVar3 = FUN_004f8490(*local_a0,uVar2);
      local_90[iVar7 * 8] = bVar3;
      local_90[iVar7 * 8 + 1] = 0;
      local_90[iVar7 * 8 + 2] = 5;
      *(byte *)((int)local_8c + iVar7 * 8 + -1) = bVar11;
      iVar8 = FUN_004f5020(bVar3);
      *(int *)(iVar6 + 8) = *(int *)(iVar6 + 8) + iVar8;
      local_a0 = local_a0 + 1;
      *(ushort *)(iVar6 + 0xc) = *(ushort *)(iVar6 + 0xc) | (ushort)(0x10 << (bVar11 & 0x1f));
      iVar7 = iVar7 + 1;
      uVar13 = uVar13 + 1;
    } while (uVar13 < local_9c);
  }
  iVar6 = local_a4 - 1;
  if (1 < *(uint *)(local_98 + 0x10)) {
    (&local_94)[iVar7 * 4] = (short)iVar6;
    piVar9 = (int *)(iVar6 * 0x10 + 8 + iVar14);
    *(undefined2 *)(local_90 + iVar7 * 8 + -2) = *(undefined2 *)(iVar6 * 0x10 + 8 + iVar14);
    if (*(uint *)(local_98 + 0x10) < 4) {
      uVar13 = *(uint *)(iVar4 + 0xec);
      if (*(uint *)(local_98 + 0x10) < 3) {
        if ((uVar13 & 0x10) == 0) {
          if ((uVar13 & 4) == 0) {
            if (-1 < (char)uVar13) goto LAB_00530fe8;
            local_90[iVar7 * 8] = 0xe;
          }
          else {
            local_90[iVar7 * 8] = 9;
          }
        }
        else {
          local_90[iVar7 * 8] = 0xb;
        }
      }
      else {
        if (-1 < (char)uVar13) goto LAB_00530f89;
        local_90[iVar7 * 8] = 0xe;
      }
LAB_00530ff9:
      iVar8 = *piVar9 + 4;
    }
    else {
      uVar13 = *(uint *)(iVar4 + 0xec);
LAB_00530f89:
      if ((uVar13 & 0x20) == 0) {
        if ((uVar13 & 8) == 0) {
LAB_00530fe8:
          if ((uVar13 & 2) == 0) {
            local_90[iVar7 * 8] = 4;
          }
          else {
            local_90[iVar7 * 8] = 8;
          }
          goto LAB_00530ff9;
        }
        local_90[iVar7 * 8] = 10;
        iVar8 = *piVar9 + 8;
      }
      else {
        local_90[iVar7 * 8] = 0xc;
        iVar8 = *piVar9 + 8;
      }
    }
    *piVar9 = iVar8;
    local_90[iVar7 * 8 + 1] = 0;
    local_90[iVar7 * 8 + 2] = 1;
    *(undefined1 *)((int)local_8c + iVar7 * 8 + -1) = 0;
    iVar7 = iVar7 + 1;
  }
  (&local_94)[iVar7 * 4] = (short)iVar6;
  piVar9 = (int *)(iVar6 * 0x10 + 8 + iVar14);
  *(undefined2 *)(local_90 + iVar7 * 8 + -2) = *(undefined2 *)(iVar6 * 0x10 + 8 + iVar14);
  if (*(uint *)(local_98 + 0x10) < 3) {
    local_90[iVar7 * 8] = 6;
LAB_00531053:
    iVar6 = *piVar9 + 4;
  }
  else {
    if ((*(byte *)(iVar4 + 0xec) & 1) != 0) {
      local_90[iVar7 * 8] = 5;
      goto LAB_00531053;
    }
    local_90[iVar7 * 8] = 7;
    iVar6 = *piVar9 + 8;
  }
  *piVar9 = iVar6;
  local_90[iVar7 * 8 + 1] = 0;
  local_90[iVar7 * 8 + 2] = 2;
  *(undefined1 *)((int)local_8c + iVar7 * 8 + -1) = 0;
  iVar7 = iVar7 + 1;
LAB_00531195:
  (&local_94)[iVar7 * 4] = 0xff;
  (local_90 + iVar7 * 8 + -2)[0] = 0;
  (local_90 + iVar7 * 8 + -2)[1] = 0;
  local_90[iVar7 * 8] = 0x11;
  local_90[iVar7 * 8 + 1] = 0;
  local_90[iVar7 * 8 + 2] = 0;
  *(undefined1 *)((int)local_8c + iVar7 * 8 + -1) = 0;
  FUN_004cb8a0();
  if (1 < local_a4) {
    piVar9 = (int *)(local_a4 * 0x10 + -8 + iVar14);
    do {
      if (*piVar9 != 0) break;
      local_a4 = local_a4 - 1;
      piVar9 = piVar9 + -4;
    } while (1 < local_a4);
  }
  if ((local_a4 == 1) && (*(char *)(param_2 + 0x1e) != '\0')) {
    iVar7 = FUN_004dc5b0(*(int *)(param_2 + 0x18),
                         *(int *)(param_2 + 0x3c) * *(int *)(param_2 + 0x18),iVar14,param_2 + 0x14);
    if (iVar7 == 0) {
      return 0;
    }
  }
  else {
    uVar13 = 0;
    if (local_a4 != 0) {
      piVar9 = (int *)(param_2 + 0x18);
      do {
        iVar7 = *(int *)(param_2 + 0x3c) * *piVar9;
        if ((*(char *)((int)piVar9 + 6) == '\0') || ((*(byte *)(iVar4 + 0xd4) & 1) == 0)) {
          if ((*(int *)(local_98 + 0x20) == 0) &&
             (((*(byte *)(iVar4 + 0xd4) & 1) != 0 || (local_a4 == 1)))) {
            *(undefined1 *)((int)piVar9 + 7) = 1;
          }
          else {
            iVar7 = FUN_004dcaa0(iVar7,piVar9 + -2);
            if (iVar7 == 0) {
              return 0;
            }
          }
          *(undefined1 *)((int)piVar9 + 6) = 0;
          piVar9[-1] = 0;
        }
        else {
          iVar7 = FUN_004dc5b0(*piVar9,iVar7,piVar9 + -2,piVar9 + -1);
          if (iVar7 == 0) {
            return 0;
          }
        }
        uVar13 = uVar13 + 1;
        piVar9 = piVar9 + 4;
      } while (uVar13 < local_a4);
    }
    if (1 < local_a4) {
      if ((*(byte *)(iVar4 + 0xd4) & 1) != 0) {
        *(undefined4 *)(param_2 + 0x30) = 1;
      }
      iVar7 = *(int *)(param_2 + 4);
      puVar10 = (undefined4 *)(param_2 + 0x54);
      do {
        *puVar10 = puVar10[-4];
        puVar10 = puVar10 + 9;
        iVar7 = iVar7 + -1;
      } while (iVar7 != 0);
      return 1;
    }
  }
  iVar7 = *(int *)(param_2 + 4);
  *(undefined4 *)(param_2 + 0x30) = 0;
  piVar9 = (int *)(param_2 + 0x54);
  do {
    iVar7 = iVar7 + -1;
    *piVar9 = *(uint *)(param_2 + 0x14) / *(uint *)(param_2 + 0x18) + piVar9[-4];
    piVar9 = piVar9 + 9;
  } while (iVar7 != 0);
  return 1;
}
