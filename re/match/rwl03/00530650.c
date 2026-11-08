// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00530650: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned short ushort;
typedef unsigned char byte;
typedef int code();
MGLOBAL(int, DAT_007dc508, 0x007dc508);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(uint, DAT_007dc548, 0x007dc548);
undefined4 FUN_004f5030(int p0, float * p1, float * p2, int p3, int p4);
undefined4 FUN_004f57e0(int p0, float * p1, float * p2, int p3, int p4);
undefined4 FUN_004f71f0(int p0, float * p1, float * p2, int p3, int p4);
bool FUN_004f77f0(undefined4 * p0, int p1, int p2, int p3);
undefined4 FUN_004f7850(int p0, ushort * p1, byte * p2, int p3, int p4, int p5);
undefined4 FUN_004f79f0(void);
undefined4 FUN_004f8660(int p0);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x00530650 FUN_00530650
/* [C1 2026-05-20] Vertex stream rebuild for the CSL skin atomic pipeline. */

void FUN_00530650(int param_1,int param_2,int param_3,uint param_4)

{
  ushort *puVar1;
  byte *pbVar2;
  char cVar3;
  ushort uVar4;
  undefined4 *puVar5;
  int iVar6;
  int iVar7;
  ushort *puVar8;
  uint uVar9;
  int unaff_EBX;
  int iVar10;
  int unaff_ESI;
  int *piVar11;
  uint uVar12;
  char *pcVar13;
  uint uVar14;
  int local_1c4 [4];
  int *piStack_1b4;
  uint uStack_1b0;
  uint local_1ac;
  uint local_1a8;
  ushort auStack_1a4 [2];
  char acStack_1a0 [8];
  undefined1 local_198 [132];
  char acStack_114 [272];
  
  iVar10 = *(int *)(param_1 + 0x18);
  local_1c4[2] = iVar10;
  local_1a8 = FUN_004f8660(iVar10);
  local_1ac = *(uint *)(iVar10 + 0x1c);
  local_1c4[1] = *(undefined4 *)(DAT_007dc508 + iVar10);
  (**(code **)(**(int **)(param_2 + 0x34) + 0x10))(*(int **)(param_2 + 0x34),local_198,local_1c4);
  uVar12 = 0;
  uStack_1b0 = 0;
  if (acStack_1a0[0] != '\x11') {
    puVar8 = auStack_1a4;
    do {
      if (uVar12 <= *puVar8) {
        uVar12 = *puVar8 + 1;
      }
      puVar1 = puVar8 + 6;
      puVar8 = puVar8 + 4;
    } while ((char)*puVar1 != '\x11');
    uStack_1b0 = uVar12;
  }
  uVar12 = uStack_1b0;
  local_1c4[0] = 0;
  uVar14 = 0;
  local_1c4[1] = 0;
  if (uStack_1b0 != 0) {
    do {
      puVar5 = (undefined4 *)((uVar14 + 1) * 0x10 + param_2);
      if ((param_4 == 0xfff) || ((param_4 & *(ushort *)(puVar5 + 3)) != 0)) {
        if ((*(char *)((int)puVar5 + 0xe) == '\0') && (puVar5[1] == 0)) {
          param_4 = param_4 | *(ushort *)(puVar5 + 3);
          (**(code **)(*(int *)*puVar5 + 0x2c))((int *)*puVar5,0,0,local_1c4 + uVar14,0x2800);
        }
        else {
          (**(code **)(*(int *)*puVar5 + 0x2c))
                    ((int *)*puVar5,puVar5[1],puVar5[2] * *(int *)(param_2 + 0x3c),
                     local_1c4 + uVar14,0x800);
        }
      }
      uVar14 = uVar14 + 1;
      iVar10 = unaff_EBX;
    } while (uVar14 < uVar12);
  }
  if ((param_4 & 2) != 0) {
    iVar6 = 0;
    while ((acStack_1a0[iVar6 * 8 + 2] != '\0' || (acStack_1a0[iVar6 * 8 + 3] != '\0'))) {
      iVar6 = iVar6 + 1;
    }
    FUN_004f57e0(acStack_1a0[iVar6 * 8],
                 (uint)*(ushort *)(acStack_1a0 + iVar6 * 8 + -2) + local_1c4[auStack_1a4[iVar6 * 4]]
                 ,*(undefined4 *)(*(int *)(iVar10 + 0x5c) + 0x14),*(undefined4 *)(param_2 + 0x3c),
                 *(undefined4 *)((uint)auStack_1a4[iVar6 * 4] * 0x10 + 0x18 + param_2));
  }
  if (((unaff_ESI != 0) && (*(int *)(unaff_ESI + 0x20) != 0)) && (param_4 == 0xfff)) {
    local_1a8 = *(uint *)(unaff_ESI + 4);
    if (1 < *(uint *)(unaff_ESI + 0x10)) {
      iVar10 = 0;
      while ((acStack_1a0[iVar10 * 8 + 2] != '\x01' || (acStack_1a0[iVar10 * 8 + 3] != '\0'))) {
        iVar10 = iVar10 + 1;
      }
      iVar6 = *(int *)(unaff_ESI + 0x18);
      uVar4 = auStack_1a4[iVar10 * 4];
      uVar12 = 0;
      if (*(int *)(param_2 + 0x3c) != 0) {
        do {
          if (_DAT_005cc320 < *(float *)(uVar12 * 0x10 + iVar6)) {
            *(undefined4 *)(uVar12 * 0x10 + iVar6) = 0x3f800000;
            *(undefined4 *)(uVar12 * 0x10 + 4 + iVar6) = 0;
            *(undefined4 *)(uVar12 * 0x10 + 8 + iVar6) = 0;
            *(undefined4 *)(uVar12 * 0x10 + 0xc + iVar6) = 0;
          }
          uVar12 = uVar12 + 1;
        } while (uVar12 < *(uint *)(param_2 + 0x3c));
      }
      FUN_004f5030(acStack_1a0[iVar10 * 8],
                   (uint)*(ushort *)(acStack_1a0 + iVar10 * 8 + -2) + local_1c4[uVar4],iVar6,
                   *(undefined4 *)(param_2 + 0x3c),
                   *(undefined4 *)((uint)uVar4 * 0x10 + 0x18 + param_2));
    }
    iVar10 = 0;
    local_1c4[2] = 0;
    while ((acStack_1a0[iVar10 * 8 + 2] != '\x02' || (acStack_1a0[iVar10 * 8 + 3] != '\0'))) {
      iVar10 = iVar10 + 1;
    }
    pcVar13 = acStack_114;
    for (iVar6 = 0x3f; iVar6 != 0; iVar6 = iVar6 + -1) {
      pcVar13[0] = '\0';
      pcVar13[1] = '\0';
      pcVar13[2] = '\0';
      pcVar13[3] = '\0';
      pcVar13 = pcVar13 + 4;
    }
    pcVar13[0] = '\0';
    pcVar13[1] = '\0';
    uVar12 = (uint)auStack_1a4[iVar10 * 4];
    local_1c4[2] = iVar10;
    pcVar13[2] = '\0';
    local_1ac = uVar12;
    if (*(int *)(unaff_ESI + 0x34) == 0) {
      uVar14 = *(uint *)(param_3 + 4);
      uVar9 = 0;
      if (DAT_007dc548 < uVar14) {
        iVar6 = *(int *)(unaff_ESI + 8);
        acStack_114[0] = '\0';
        iVar10 = local_1c4[2];
        if (local_1a8 != 0) {
          do {
            cVar3 = (char)uVar9;
            pbVar2 = (byte *)(iVar6 + uVar9);
            uVar9 = uVar9 + 1;
            acStack_114[*pbVar2] = cVar3 * '\x03';
            uVar12 = local_1ac;
          } while (uVar9 < local_1a8);
        }
      }
      else if (uVar14 != 0) {
        do {
          acStack_114[uVar9] = (char)uVar9 * '\x03';
          uVar9 = uVar9 + 1;
          iVar10 = local_1c4[2];
        } while (uVar9 < uVar14);
      }
    }
    else {
      if (*(int *)(param_3 + 4) <= (int)(uint)**(byte **)(unaff_ESI + 0x30)) {
        **(byte **)(unaff_ESI + 0x30) = 0;
      }
      uVar14 = *(uint *)(param_3 + 4);
      iVar6 = *(int *)(unaff_ESI + 0x30);
      uVar9 = 0;
      if (uVar14 != 0) {
        do {
          cVar3 = *(char *)(iVar6 + uVar9);
          if (cVar3 != -1) {
            acStack_114[uVar9] = cVar3 * '\x03';
            iVar10 = local_1c4[2];
          }
          uVar9 = uVar9 + 1;
        } while (uVar9 < uVar14);
      }
    }
    FUN_004f7850(acStack_1a0[iVar10 * 8],
                 (uint)*(ushort *)(acStack_1a0 + iVar10 * 8 + -2) + local_1c4[uVar12],
                 *(undefined4 *)(unaff_ESI + 0x14),acStack_114,*(undefined4 *)(param_2 + 0x3c),
                 *(undefined4 *)(uVar12 * 0x10 + 0x18 + param_2));
    iVar10 = unaff_EBX;
  }
  if (((*(byte *)(iVar10 + 8) & 0x10) != 0) && ((param_4 & 4) != 0)) {
    iVar6 = 0;
    while ((acStack_1a0[iVar6 * 8 + 2] != '\x03' || (acStack_1a0[iVar6 * 8 + 3] != '\0'))) {
      iVar6 = iVar6 + 1;
    }
    FUN_004f57e0(acStack_1a0[iVar6 * 8],
                 (uint)*(ushort *)(acStack_1a0 + iVar6 * 8 + -2) + local_1c4[auStack_1a4[iVar6 * 4]]
                 ,*(undefined4 *)(*(int *)(iVar10 + 0x5c) + 0x18),*(undefined4 *)(param_2 + 0x3c),
                 *(undefined4 *)((uint)auStack_1a4[iVar6 * 4] * 0x10 + 0x18 + param_2));
  }
  if ((((uint)piStack_1b4 & 0x10000) != 0) && ((param_4 & 0xff2) != 0)) {
    iVar6 = 0;
    while ((acStack_1a0[iVar6 * 8 + 2] != '\x06' || (acStack_1a0[iVar6 * 8 + 3] != '\0'))) {
      iVar6 = iVar6 + 1;
    }
    FUN_004f79f0(acStack_1a0[iVar6 * 8],
                 (uint)*(ushort *)(acStack_1a0 + iVar6 * 8 + -2) + local_1c4[auStack_1a4[iVar6 * 4]]
                 ,*(undefined4 *)(*(int *)(iVar10 + 0x5c) + 0x14),
                 *(undefined4 *)(iVar10 + 0x30 + local_1c4[3] * 4),param_2,
                 *(undefined4 *)((uint)auStack_1a4[iVar6 * 4] * 0x10 + 0x18 + param_2));
  }
  if (((local_1c4[3] != 0) && ((param_4 & 0xff0) != 0)) && (uVar12 = 0, local_1c4[3] != 0)) {
    puVar5 = (undefined4 *)(unaff_EBX + 0x34);
    do {
      if ((param_4 & 0x10 << ((byte)uVar12 & 0x1f)) != 0) {
        iVar10 = 0;
        while ((acStack_1a0[iVar10 * 8 + 2] != '\x05' ||
               ((byte)acStack_1a0[iVar10 * 8 + 3] != uVar12))) {
          iVar10 = iVar10 + 1;
        }
        FUN_004f71f0(acStack_1a0[iVar10 * 8],
                     (uint)*(ushort *)(acStack_1a0 + iVar10 * 8 + -2) +
                     local_1c4[auStack_1a4[iVar10 * 4]],*puVar5,*(undefined4 *)(param_2 + 0x3c),
                     *(undefined4 *)((uint)auStack_1a4[iVar10 * 4] * 0x10 + 0x18 + param_2));
      }
      uVar12 = uVar12 + 1;
      puVar5 = puVar5 + 1;
    } while (uVar12 < (uint)local_1c4[3]);
  }
  if (((*(byte *)(unaff_EBX + 8) & 8) != 0) && ((param_4 & 8) != 0)) {
    iVar10 = 0;
    while ((acStack_1a0[iVar10 * 8 + 2] != '\n' || (acStack_1a0[iVar10 * 8 + 3] != '\0'))) {
      iVar10 = iVar10 + 1;
    }
    uVar4 = *(ushort *)(acStack_1a0 + iVar10 * 8 + -2);
    iVar6 = *(int *)(param_2 + 4);
    piStack_1b4 = local_1c4 + auStack_1a4[iVar10 * 4];
    iVar10 = *(int *)((uint)auStack_1a4[iVar10 * 4] * 0x10 + 0x18 + param_2);
    piVar11 = (int *)(param_2 + 0x44);
    do {
      iVar7 = FUN_004f77f0(*piVar11 * iVar10 + *piStack_1b4 + (uint)uVar4,
                           *(int *)(unaff_EBX + 0x30) + *piVar11 * 4,piVar11[5],iVar10);
      piVar11[2] = iVar7;
      piVar11 = piVar11 + 9;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
  }
  uVar12 = uStack_1b0;
  uVar14 = 0;
  if (uStack_1b0 != 0) {
    do {
      if (local_1c4[uVar14] != 0) {
        piVar11 = *(int **)((uVar14 + 1) * 0x10 + param_2);
        (**(code **)(*piVar11 + 0x30))(piVar11);
      }
      uVar14 = uVar14 + 1;
    } while (uVar14 < uVar12);
  }
  return;
}
