// FLAGS: /O2 /Ob1 /G5
// h2: 30.1% (permuter, 2693 compiles; Ghidra form 26.5%). 2430 B state-dump routine on an 8-byte on-stack record array, only compile-fixed (float/pointer locals split, FUN_004f6870 prototype). Structure not rebuilt: budget. Class (b).
/* MATCH-RWL02 0x004ea9a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl02_20261107/results.tsv. */
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
MGLOBAL(uint, DAT_00911fbc, 0x00911fbc);
MGLOBAL(byte, DAT_0091208c, 0x0091208c);
MGLOBAL(undefined1, DAT_00912074, 0x00912074);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, FUN_004eb320, 0x004eb320);
bool FUN_004cb8a0();
undefined4 FUN_004dc5b0(uint p0, uint p1, int * p2, int * p3);
void FUN_004dc750(int p0, int p1, int p2, int p3);
undefined4 FUN_004dcaa0(uint * p0, uint * p1);
void FUN_004dcb80(int p0);
void FUN_004f4200(int p0);
undefined4 FUN_004f57e0(int p0, float * p1, float * p2, int p3, int p4);
undefined4 FUN_004f6870(int p0, uint p1, undefined4 p2, undefined4 p3, float p4, undefined4 p5, undefined4 p6);
undefined4 FUN_004f71f0(int p0, float * p1, float * p2, int p3, int p4);
bool FUN_004f77f0(undefined4 * p0, int p1, int p2, int p3);
undefined4 FUN_004f79f0(void);
undefined4 FUN_004f8660(int p0);
void FUN_005c24e0(undefined1 * p0, uint p1, uint p2, undefined * p3);
// MATCH: 0x004ea9a0 FUN_004ea9a0
undefined4 FUN_004ea9a0(int param_1,int param_2,int param_3)

{
  ushort *puVar1;
  int iVar2;
  undefined4 *puVar3;
  byte bVar4;
  ushort *puVar5;
  uint uVar6;
  int *piVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  undefined4 uVar11;
  uint local_dc;
  undefined4 *local_d8;
  float local_d8f;
  int local_d4;
  uint local_d0;
  uint local_cc;
  uint local_c8;
  ushort *local_c4;
  int local_c0 [4];
  undefined4 uStack_b0;
  undefined4 uStack_ac;
  ushort local_a8;
  ushort local_a6;
  char local_a4 [4];
  undefined2 local_a0;
  undefined2 local_9e;
  undefined1 local_9c [152];
  
  iVar2 = *(int *)(param_1 + 0x18);
  local_c4 = *(ushort **)(iVar2 + 8);
  local_d4 = iVar2;
  local_c8 = FUN_004f8660(iVar2);
  local_c0[3] = *(uint *)(iVar2 + 0x1c);
  if (param_3 == 0) {
    *(undefined4 *)(param_2 + 0x3c) = *(undefined4 *)(iVar2 + 0x14);
    local_dc = 0;
    do {
      piVar7 = (int *)((local_dc + 1) * 0x10 + param_2);
      iVar8 = *piVar7;
      if (iVar8 != 0) {
        if (*(char *)(local_dc * 0x10 + 0x1e + param_2) == '\0') {
          if ((piVar7[1] == 0) && (*(char *)((int)piVar7 + 0xf) == '\0')) {
            FUN_004dcb80(iVar8);
          }
        }
        else {
          FUN_004dc750(piVar7[2],*(int *)(param_2 + 0x3c) * piVar7[2],iVar8,piVar7[1]);
        }
        *piVar7 = 0;
      }
      piVar7[1] = 0;
      piVar7[2] = 0;
      *(undefined2 *)(piVar7 + 3) = 0;
      *(undefined1 *)((int)piVar7 + 0xe) = 0;
      *(undefined1 *)((int)piVar7 + 0xf) = 0;
      local_dc = local_dc + 1;
    } while (local_dc < 2);
    if ((((*(int *)(iVar2 + 0x18) < 2) && ((local_c8 & 0xfff) == 0)) ||
        ((DAT_00911fbc & 0x10000) == 0)) ||
       ((*(int *)(param_1 + 0x6c) != 0 && (*(int *)(*(int *)(param_1 + 0x6c) + 0x2c) == 0x12e)))) {
      uVar6 = 1;
      local_d8 = (undefined4 *)0x0;
      local_d0 = 1;
    }
    else {
      if (1 < *(int *)(iVar2 + 0x18)) {
        local_c8 = local_c8 | 6;
      }
      uVar6 = 2;
      local_d8 = (undefined4 *)0x1;
      local_d0 = 2;
      FUN_004f4200(local_d4);
    }
    if ((local_c8 & 2) == 0) {
      iVar2 = uVar6 - 1;
    }
    else {
      iVar2 = 0;
    }
    local_a8 = (ushort)iVar2;
    iVar8 = 1;
    local_a6 = *(undefined2 *)(iVar2 * 0x10 + 0x18 + param_2);
    iVar2 = iVar2 * 0x10 + 0x10 + param_2;
    local_a4[0] = '\x02';
    local_a4[1] = 0;
    local_a4[2] = 0;
    local_a4[3] = 0;
    *(undefined4 *)(iVar2 + 8) = 0xc;
    *(undefined2 *)(iVar2 + 0xc) = 2;
    if (((uint)local_c4 & 0x10) != 0) {
      if ((local_c8 & 4) == 0) {
        iVar2 = uVar6 - 1;
      }
      else {
        iVar2 = 0;
      }
      local_a0 = (undefined2)iVar2;
      local_9e = *(undefined2 *)(iVar2 * 0x10 + 0x18 + param_2);
      iVar2 = iVar2 * 0x10 + 0x10 + param_2;
      if ((DAT_0091208c & 0x80) == 0) {
        local_9c[0] = 2;
        iVar8 = *(int *)(iVar2 + 8) + 0xc;
      }
      else {
        local_9c[0] = 0xe;
        iVar8 = *(int *)(iVar2 + 8) + 4;
      }
      *(int *)(iVar2 + 8) = iVar8;
      local_9c[1] = 0;
      local_9c[2] = 3;
      local_9c[3] = 0;
      *(byte *)(iVar2 + 0xc) = *(byte *)(iVar2 + 0xc) | 4;
      iVar8 = 2;
    }
    if (((uint)local_c4 & 8) != 0) {
      if ((local_c8 & 8) == 0) {
        iVar2 = uVar6 - 1;
      }
      else {
        iVar2 = 0;
      }
      (&local_a8)[iVar8 * 4] = (ushort)iVar2;
      iVar8 = iVar8 + 1;
      iVar9 = iVar2 * 0x10 + 0x10 + param_2;
      *(undefined2 *)((int)&uStack_b0 + iVar8 * 8 + 2) =
           *(undefined2 *)(iVar2 * 0x10 + 0x18 + param_2);
      *(undefined1 *)(&uStack_ac + iVar8 * 2) = 4;
      *(undefined1 *)((int)&uStack_ac + iVar8 * 8 + 1) = 0;
      *(undefined1 *)((int)&uStack_ac + iVar8 * 8 + 2) = 10;
      *(undefined1 *)((int)&uStack_ac + iVar8 * 8 + 3) = 0;
      *(byte *)(iVar9 + 0xc) = *(byte *)8 | (iVar9 + 0xc);
      *(int *)(iVar9 + 8) = *(int *)(iVar9 + 8) + 4;
    }
    local_dc = 0;
    if (local_c0[3] != 0) {
      do {
        bVar4 = (byte)local_dc;
        if ((local_c8 & 0x10 << (bVar4 & 0x1f)) == 0) {
          iVar2 = uVar6 - 1;
        }
        else {
          iVar2 = 0;
        }
        (&local_a8)[iVar8 * 4] = (ushort)iVar2;
        iVar8 = iVar8 + 1;
        iVar9 = iVar2 * 0x10 + 0x10 + param_2;
        *(undefined2 *)((int)&uStack_b0 + iVar8 * 8 + 2) =
             *(undefined2 *)(iVar2 * 0x10 + 0x18 + param_2);
        *(undefined1 *)(&uStack_ac + iVar8 * 2) = 1;
        *(undefined1 *)((int)&uStack_ac + iVar8 * 8 + 1) = 0;
        *(undefined1 *)((int)&uStack_ac + iVar8 * 8 + 2) = 5;
        *(byte *)((int)&uStack_ac + iVar8 * 8 + 3) = bVar4;
        *(int *)(iVar9 + 8) = *(int *)(iVar9 + 8) + 8;
        *(ushort *)(iVar9 + 0xc) = *(ushort *)(iVar9 + 0xc) | (ushort)(0x10 << (bVar4 & 0x1f));
        local_dc = local_dc + 1;
      } while (local_dc < (uint)local_c0[3]);
    }
    if (((local_c8 & 0x10000) != 0) && (local_c0[3] != 0)) {
      if ((local_c8 & 0xff2) == 0) {
        iVar2 = uVar6 - 1;
      }
      else {
        iVar2 = 0;
      }
      (&local_a8)[iVar8 * 4] = (ushort)iVar2;
      piVar7 = (int *)(iVar2 * 0x10 + 0x18 + param_2);
      *(undefined2 *)(local_a4 + iVar8 * 8 + -2) = *(undefined2 *)(iVar2 * 0x10 + 0x18 + param_2);
      if ((char)DAT_0091208c < '\0') {
        local_a4[iVar8 * 8] = '\x0e';
        iVar2 = *piVar7 + 4;
      }
      else {
        local_a4[iVar8 * 8] = '\x02';
        iVar2 = *piVar7 + 0xc;
      }
      *piVar7 = iVar2;
      local_a4[iVar8 * 8 + 1] = '\0';
      local_a4[iVar8 * 8 + 2] = '\x06';
      local_a4[iVar8 * 8 + 3] = '\0';
      iVar8 = iVar8 + 1;
    }
    if (1 < uVar6) {
      piVar7 = (int *)(uVar6 * 0x10 + 8 + param_2);
      do {
        local_d0 = uVar6;
        if (*piVar7 != 0) break;
        uVar6 = uVar6 - 1;
        piVar7 = piVar7 + -4;
        local_d0 = uVar6;
      } while (1 < uVar6);
    }
    FUN_005c24e0(&local_a8,iVar8,8,FUN_004eb320);
    (&local_a8)[iVar8 * 4] = 0xff;
    (local_a4 + iVar8 * 8 + -2)[0] = '\0';
    (local_a4 + iVar8 * 8 + -2)[1] = '\0';
    local_a4[iVar8 * 8] = '\x11';
    local_a4[iVar8 * 8 + 1] = '\0';
    local_a4[iVar8 * 8 + 2] = '\0';
    local_a4[iVar8 * 8 + 3] = '\0';
    FUN_004cb8a0();
    local_dc = 0;
    if (uVar6 != 0) {
      do {
        iVar2 = (local_dc + 1) * 0x10 + param_2;
        iVar8 = *(int *)(param_2 + 0x3c) * *(int *)(iVar2 + 8);
        if ((local_d8 == (undefined4 *)0x0) || ((local_dc != 0 && (((byte)DAT_00912074 & 1) != 0))))
        {
          *(undefined1 *)(iVar2 + 0xe) = 1;
          iVar2 = FUN_004dc5b0(*(int *)(iVar2 + 8),iVar8,iVar2,iVar2 + 4);
        }
        else {
          *(undefined1 *)(iVar2 + 0xe) = 0;
          iVar2 = FUN_004dcaa0(iVar8,iVar2);
        }
        if (iVar2 == 0) {
          return 0;
        }
        local_dc = local_dc + 1;
      } while (local_dc < uVar6);
    }
    if (!(uVar6 < 2)) {
      if (((byte)DAT_00912074 & 1) != 0) {
        *(undefined4 *)(param_2 + 0x30) = 1;
      }
      iVar2 = *(int *)(param_2 + 4);
      puVar3 = (undefined4 *)(param_2 + 0x54);
      do {
        *puVar3 = puVar3[-4];
        puVar3 = puVar3 + 9;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
    } else {
      iVar2 = *(int *)(param_2 + 4);
      *(undefined4 *)(param_2 + 0x30) = 0;
      piVar7 = (int *)(param_2 + 0x54);
      do {
        iVar2 = iVar2 + -1;
        *piVar7 = *(uint *)(param_2 + 0x14) / *(uint *)(param_2 + 0x18) + piVar7[-4];
        piVar7 = piVar7 + 9;
      } while (iVar2 != 0);
    }
  }
  else {
    (**(code **)(**(int **)(param_2 + 0x34) + 0x10))(*(int **)(param_2 + 0x34),&local_a8,&local_dc);
    uVar6 = 0;
    local_d0 = 0;
    if (local_a4[0] != '\x11') {
      puVar5 = &local_a8;
      do {
        if (uVar6 <= *puVar5) {
          uVar6 = *puVar5 + 1;
        }
        puVar1 = puVar5 + 6;
        puVar5 = puVar5 + 4;
        local_d0 = uVar6;
      } while ((char)*puVar1 != '\x11');
    }
  }
  local_c0[0] = 0;
  local_c0[1] = 0;
  if (param_3 == 0) {
    local_cc = 0xfff;
    local_dc = 0;
    if (uVar6 != 0) {
      do {
        puVar3 = (undefined4 *)((local_dc + 1) * 0x10 + param_2);
        (**(code **)(*(int *)*puVar3 + 0x2c))
                  ((int *)*puVar3,puVar3[1],puVar3[2] * *(int *)(param_2 + 0x3c),local_c0 + local_dc
                   ,0x800);
        local_dc = local_dc + 1;
      } while (local_dc < uVar6);
    }
  }
  else {
    local_cc = (uint)*(ushort *)(local_d4 + 0xc);
    local_dc = 0;
    if (uVar6 != 0) {
      do {
        puVar3 = (undefined4 *)((local_dc + 1) * 0x10 + param_2);
        if ((local_cc == 0xfff) || ((local_cc & *(ushort *)(puVar3 + 3)) != 0)) {
          if (*(char *)((int)puVar3 + 0xe) == '\0') {
            uVar11 = 0x2800;
            local_cc = local_cc | *(ushort *)(puVar3 + 3);
          }
          else {
            uVar11 = 0x800;
          }
          (**(code **)(*(int *)*puVar3 + 0x2c))
                    ((int *)*puVar3,puVar3[1],puVar3[2] * *(int *)(param_2 + 0x3c),
                     local_c0 + local_dc,uVar11);
          uVar6 = local_d0;
        }
        local_dc = local_dc + 1;
      } while (local_dc < uVar6);
    }
  }
  iVar2 = *(int *)(local_d4 + 0x18);
  if (iVar2 == 1) {
    local_c0[2] = 0;
  }
  else {
    iVar8 = (int)*(short *)(param_1 + 0x52);
    local_c0[2] = (int)*(short *)(param_1 + 0x50);
    if ((iVar2 <= *(short *)(param_1 + 0x50)) || (iVar2 <= iVar8)) {
      iVar8 = 0;
      local_c0[2] = 0;
    }
    local_d8f = *(float *)(param_1 + 0x5c) * *(float *)(param_1 + 0x58);
    if ((local_c0[2] != iVar8) && (local_d8f != DAT_005d757c)) {
      iVar9 = local_c0[2] * 0x1c;
      uStack_b0 = *(int *)(*(int *)(local_d4 + 0x5c) + 0x14 + iVar9);
      uStack_ac = *(undefined4 *)(iVar8 * 0x1c + 0x14 + *(int *)(local_d4 + 0x5c));
      iVar2 = 0;
      while ((local_a4[iVar2 * 8 + 2] != '\0' || (local_a4[iVar2 * 8 + 3] != '\0'))) {
        iVar2 = iVar2 + 1;
      }
      FUN_004f6870(local_a4[iVar2 * 8],
                   (uint)*(ushort *)(local_a4 + iVar2 * 8 + -2) + local_c0[(&local_a8)[iVar2 * 4]],
                   uStack_b0,uStack_ac,local_d8f,*(undefined4 *)(param_2 + 0x3c),
                   *(undefined4 *)((uint)(&local_a8)[iVar2 * 4] * 0x10 + 0x18 + param_2));
      if (((uint)local_c4 & 0x10) != 0) {
        iVar2 = 0;
        while ((local_a4[iVar2 * 8 + 2] != '\x03' || (local_a4[iVar2 * 8 + 3] != '\0'))) {
          iVar2 = iVar2 + 1;
        }
        FUN_004f6870(local_a4[iVar2 * 8],
                     (uint)*(ushort *)(local_a4 + iVar2 * 8 + -2) + local_c0[(&local_a8)[iVar2 * 4]]
                     ,*(undefined4 *)(*(int *)(local_d4 + 0x5c) + 0x18 + iVar9),
                     *(undefined4 *)(iVar8 * 0x1c + 0x18 + *(int *)(local_d4 + 0x5c)),local_d8f,
                     *(undefined4 *)(param_2 + 0x3c),
                     *(undefined4 *)((uint)(&local_a8)[iVar2 * 4] * 0x10 + 0x18 + param_2));
      }
      goto LAB_004eb104;
    }
  }
  if ((local_cc & 2) != 0) {
    iVar2 = 0;
    while ((local_a4[iVar2 * 8 + 2] != '\0' || (local_a4[iVar2 * 8 + 3] != '\0'))) {
      iVar2 = iVar2 + 1;
    }
    FUN_004f57e0(local_a4[iVar2 * 8],
                 (uint)*(ushort *)(local_a4 + iVar2 * 8 + -2) + local_c0[(&local_a8)[iVar2 * 4]],
                 *(undefined4 *)(*(int *)(local_d4 + 0x5c) + 0x14 + local_c0[2] * 0x1c),
                 *(undefined4 *)(param_2 + 0x3c),
                 *(undefined4 *)((uint)(&local_a8)[iVar2 * 4] * 0x10 + 0x18 + param_2));
  }
  if ((((uint)local_c4 & 0x10) != 0) && ((local_cc & 4) != 0)) {
    iVar2 = 0;
    while ((local_a4[iVar2 * 8 + 2] != '\x03' || (local_a4[iVar2 * 8 + 3] != '\0'))) {
      iVar2 = iVar2 + 1;
    }
    FUN_004f57e0(local_a4[iVar2 * 8],
                 (uint)*(ushort *)(local_a4 + iVar2 * 8 + -2) + local_c0[(&local_a8)[iVar2 * 4]],
                 *(undefined4 *)(*(int *)(local_d4 + 0x5c) + 0x18 + local_c0[2] * 0x1c),
                 *(undefined4 *)(param_2 + 0x3c),
                 *(undefined4 *)((uint)(&local_a8)[iVar2 * 4] * 0x10 + 0x18 + param_2));
  }
LAB_004eb104:
  if (((uint)local_c4 & 8) == 0) {
    if (param_3 == 0) {
      iVar2 = *(int *)(param_2 + 4);
      puVar3 = (undefined4 *)(param_2 + 0x4c);
      do {
        *puVar3 = 0;
        puVar3 = puVar3 + 9;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
    }
  }
  else if ((local_cc & 8) != 0) {
    iVar2 = 0;
    while ((local_a4[iVar2 * 8 + 2] != '\n' || (local_a4[iVar2 * 8 + 3] != '\0'))) {
      iVar2 = iVar2 + 1;
    }
    piVar7 = (int *)(param_2 + 0x44);
    local_d8 = *(undefined4 **)(param_2 + 4);
    uStack_b0 = *(int *)(local_d4 + 0x30);
    uStack_ac = (undefined4)(&local_a8 + iVar2 * 4);
    iVar8 = *(int *)((uint)(&local_a8)[iVar2 * 4] * 0x10 + 0x18 + param_2);
    local_c4 = (ushort *)(local_a4 + iVar2 * 8 + -2);
    do {
      iVar2 = FUN_004f77f0(local_c0[*(ushort *)uStack_ac] + *piVar7 * iVar8 + (uint)*local_c4,
                           uStack_b0 + *piVar7 * 4,piVar7[5],iVar8);
      piVar7[2] = iVar2;
      piVar7 = piVar7 + 9;
      local_d8 = (undefined4 *)((int)local_d8 + -1);
    } while (local_d8 != (undefined4 *)0x0);
    local_d8 = (undefined4 *)0x0;
  }
  uVar6 = local_c0[3];
  if (((local_c0[3] != 0) && ((local_cc & 0xff0) != 0)) && (uVar10 = 0, local_c0[3] != 0)) {
    local_d8 = (undefined4 *)(local_d4 + 0x34);
    do {
      if ((local_cc & 0x10 << ((byte)uVar10 & 0x1f)) != 0) {
        iVar2 = 0;
        while ((local_a4[iVar2 * 8 + 2] != '\x05' || ((byte)local_a4[iVar2 * 8 + 3] != uVar10))) {
          iVar2 = iVar2 + 1;
        }
        FUN_004f71f0(local_a4[iVar2 * 8],
                     (uint)*(ushort *)(local_a4 + iVar2 * 8 + -2) + local_c0[(&local_a8)[iVar2 * 4]]
                     ,*local_d8,*(undefined4 *)(param_2 + 0x3c),
                     *(undefined4 *)((uint)(&local_a8)[iVar2 * 4] * 0x10 + 0x18 + param_2));
        uVar6 = local_c0[3];
      }
      uVar10 = uVar10 + 1;
      local_d8 = local_d8 + 1;
    } while (uVar10 < uVar6);
  }
  if ((((local_c8 & 0x10000) != 0) && (uVar6 != 0)) && ((local_cc & 0xff2) != 0)) {
    iVar2 = 0;
    while ((local_a4[iVar2 * 8 + 2] != '\x06' || (local_a4[iVar2 * 8 + 3] != '\0'))) {
      iVar2 = iVar2 + 1;
    }
    FUN_004f79f0(local_a4[iVar2 * 8],
                 (uint)*(ushort *)(local_a4 + iVar2 * 8 + -2) + local_c0[(&local_a8)[iVar2 * 4]],
                 *(undefined4 *)(*(int *)(local_d4 + 0x5c) + 0x14 + local_c0[2] * 0x1c),
                 *(undefined4 *)(local_d4 + 0x30 + local_c0[3] * 4),param_2,
                 *(undefined4 *)((uint)(&local_a8)[iVar2 * 4] * 0x10 + 0x18 + param_2));
  }
  local_dc = 0;
  if (local_d0 != 0) {
    do {
      if (local_c0[local_dc] != 0) {
        piVar7 = *(int **)((local_dc + 1) * 0x10 + param_2);
        (**(code **)(*piVar7 + 0x30))(piVar7);
      }
      local_dc = local_dc + 1;
    } while (local_dc < local_d0);
  }
  return 1;
}
