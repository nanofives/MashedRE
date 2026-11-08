// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x005595d0 (h2): raw form untouched, 57.5%, class (b). C++ check FAILS (uint* to undefined4, arg types): not fixed. */
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
MGLOBAL(undefined1, DAT_005cc328, 0x005cc328);
undefined4 FUN_00563df0(int p0, uint p1, uint p2);
void FUN_00564100(undefined4 * p0, undefined1 p1, uint p2);
int FUN_005645b0(int p0, undefined4 p1, uint p2, undefined4 * p3, undefined4 p4);
MGLOBAL(int, _DAT_005cc328, 0x005cc328);
// MATCH: 0x005595d0 FUN_005595d0
/* [C1 2026-05-19] Sub-allocator carving function: receives `param_1` (arena cursor wrapper) with
   mutable cursor at `*(param_1+8)`, and a
 */

undefined4 *
FUN_005595d0(int param_1,int param_2,undefined4 param_3,float param_4,undefined4 param_5)

{
  uint *puVar1;
  uint *puVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  undefined4 *puVar7;
  
  puVar7 = (undefined4 *)(*(int *)(param_1 + 8) + 0xfU & 0xfffffff0);
  *(undefined4 **)(param_1 + 8) = puVar7 + 0x24;
  puVar7[3] = param_4;
  iVar6 = *(int *)(param_2 + 8);
  puVar7[2] = param_5;
  puVar7[1] = iVar6 + 0x1fU >> 5;
  uVar4 = *(uint *)(param_2 + 0x14);
  puVar2 = (uint *)(*(int *)(param_1 + 8) + 0xfU & 0xfffffff0);
  *(uint **)(param_1 + 8) = puVar2 + 0x1c;
  puVar2[0xe] = uVar4;
  puVar2[0xf] = uVar4;
  uVar5 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar5;
  *puVar2 = uVar5;
  iVar6 = uVar4 * 0x20;
  *(uint *)(param_1 + 8) = uVar4 * 0x40 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  puVar1 = puVar2 + 0x16;
  puVar2[1] = (uint)puVar1;
  uVar5 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar5;
  puVar2[2] = uVar5;
  *(uint *)(param_1 + 8) = iVar6 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  puVar2[3] = (uint)puVar1;
  uVar5 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar5;
  puVar2[4] = uVar5;
  *(uint *)(param_1 + 8) = uVar4 * 0x10 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  puVar2[5] = (uint)puVar1;
  uVar5 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar5;
  puVar2[0x10] = uVar5;
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + 0xf + iVar6 & 0xfffffff0;
  puVar2[0x11] = (uint)puVar1;
  uVar5 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar5;
  puVar2[0x14] = uVar5;
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + 0xf + uVar4 * 0x10 & 0xfffffff0;
  puVar2[0x15] = (uint)puVar1;
  uVar5 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar5;
  puVar2[0xc] = uVar5;
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + 0xf + iVar6 & 0xfffffff0;
  puVar2[0xd] = (uint)puVar1;
  uVar5 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar5;
  puVar2[0x12] = uVar5;
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + 0xf + iVar6 & 0xfffffff0;
  puVar2[0x13] = (uint)puVar1;
  uVar5 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar5;
  puVar2[8] = uVar5;
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + 0xf + uVar4 * 0x30 & 0xfffffff0;
  puVar2[9] = (uint)puVar1;
  uVar5 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar5;
  puVar2[10] = uVar5;
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + 0xf + uVar4 * 0x30 & 0xfffffff0;
  puVar2[0xb] = (uint)puVar1;
  puVar2[6] = *(uint *)(param_1 + 8);
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + uVar4 * 4;
  puVar2[7] = (uint)puVar1;
  *puVar1 = *(uint *)(param_1 + 8);
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + uVar4 * 4;
  puVar2[0x17] = 0;
  puVar2[0x18] = uVar4;
  puVar7[4] = puVar2;
  uVar3 = FUN_00563df0(param_1,*(undefined4 *)(param_2 + 8),*(undefined4 *)(param_2 + 0x1c));
  param_4 = param_4 * _DAT_005cc328;
  puVar7[0x15] = uVar3;
  uVar3 = FUN_005645b0(param_1,uVar3,*(undefined4 *)(param_2 + 8),param_3,param_4);
  *puVar7 = uVar3;
  puVar7[0x16] = *(undefined4 *)(param_1 + 8);
  FUN_00564100(*(undefined4 *)(param_1 + 8),0,puVar7[1] << 2);
  iVar6 = *(int *)(param_1 + 8) + puVar7[1] * 4;
  *(int *)(param_1 + 8) = iVar6;
  puVar7[0x17] = iVar6;
  FUN_00564100(*(undefined4 *)(param_1 + 8),0,puVar7[1] << 2);
  iVar6 = *(int *)(param_1 + 8) + puVar7[1] * 4;
  *(int *)(param_1 + 8) = iVar6;
  puVar7[0x19] = iVar6;
  FUN_00564100(*(undefined4 *)(param_1 + 8),0,puVar7[1] << 2);
  iVar6 = *(int *)(param_1 + 8) + puVar7[1] * 4;
  *(int *)(param_1 + 8) = iVar6;
  puVar7[0x18] = iVar6;
  FUN_00564100(*(undefined4 *)(param_1 + 8),0,puVar7[1] << 2);
  iVar6 = *(int *)(param_1 + 8) + puVar7[1] * 4;
  *(int *)(param_1 + 8) = iVar6;
  puVar7[0x1a] = iVar6;
  FUN_00564100(*(undefined4 *)(param_1 + 8),0,puVar7[1] << 2);
  *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + puVar7[1] * 4;
  uVar4 = 0;
  if (puVar7[1] != 0) {
    do {
      uVar4 = uVar4 + 1;
      *(undefined4 *)(puVar7[0x16] + -4 + uVar4 * 4) = 0;
      *(undefined4 *)(puVar7[0x17] + -4 + uVar4 * 4) = 0;
      *(undefined4 *)(puVar7[0x19] + -4 + uVar4 * 4) = 0;
      *(undefined4 *)(puVar7[0x18] + -4 + uVar4 * 4) = 0;
      *(undefined4 *)(puVar7[0x1a] + -4 + uVar4 * 4) = 0;
    } while (uVar4 < (uint)puVar7[1]);
  }
  puVar7[0x1c] = *(undefined4 *)(param_1 + 8);
  iVar6 = *(int *)(param_1 + 8) + puVar7[1] * 4;
  *(int *)(param_1 + 8) = iVar6;
  puVar7[0x1d] = iVar6;
  iVar6 = *(int *)(param_1 + 8) + puVar7[1] * 4;
  *(int *)(param_1 + 8) = iVar6;
  puVar7[0x1e] = iVar6;
  iVar6 = *(int *)(param_1 + 8) + puVar7[1] * 4;
  *(int *)(param_1 + 8) = iVar6;
  puVar7[9] = iVar6;
  FUN_00564100(*(undefined4 *)(param_1 + 8),0,*(int *)(param_2 + 0x14) * 0x2c);
  iVar6 = *(int *)(param_1 + 8) + *(int *)(param_2 + 0x14) * 0x2c;
  *(int *)(param_1 + 8) = iVar6;
  puVar7[10] = iVar6;
  *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + *(int *)(param_2 + 0x14) * 4;
  uVar4 = *(uint *)(param_2 + 0x14);
  uVar5 = 0;
  if (uVar4 != 0) {
    do {
      iVar6 = uVar4 - uVar5;
      uVar5 = uVar5 + 1;
      *(int *)(puVar7[10] + -4 + uVar5 * 4) = puVar7[9] + (iVar6 + -1) * 0x2c;
      uVar4 = *(uint *)(param_2 + 0x14);
    } while (uVar5 < uVar4);
  }
  puVar7[0xb] = *(undefined4 *)(param_2 + 0x14);
  puVar7[0xc] = *(undefined4 *)(param_2 + 0x14);
  uVar4 = 0;
  if (*(int *)(param_2 + 0x14) != 0) {
    iVar6 = 0;
    do {
      iVar6 = iVar6 + 0x2c;
      *(uint *)(puVar7[9] + -0x28 + iVar6) = uVar4;
      uVar4 = uVar4 + 1;
    } while (uVar4 < *(uint *)(param_2 + 0x14));
  }
  if (*(int *)(param_2 + 0x18) == 0) {
    puVar7[0x10] = 0;
    puVar7[0xf] = 0;
    puVar7[0xd] = 0;
    puVar7[0xe] = 0;
  }
  else {
    puVar7[0xd] = *(undefined4 *)(param_1 + 8);
    FUN_00564100(*(undefined4 *)(param_1 + 8),0,*(int *)(param_2 + 0x18) * 0x9c);
    iVar6 = *(int *)(param_1 + 8) + *(int *)(param_2 + 0x18) * 0x9c;
    uVar5 = 0;
    *(int *)(param_1 + 8) = iVar6;
    puVar7[0xe] = iVar6;
    *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + *(int *)(param_2 + 0x18) * 4;
    uVar4 = *(uint *)(param_2 + 0x18);
    if (uVar4 != 0) {
      do {
        iVar6 = uVar4 - uVar5;
        uVar5 = uVar5 + 1;
        *(int *)(puVar7[0xe] + -4 + uVar5 * 4) = puVar7[0xd] + (iVar6 + -1) * 0x9c;
        uVar4 = *(uint *)(param_2 + 0x18);
      } while (uVar5 < uVar4);
    }
    puVar7[0xf] = *(undefined4 *)(param_2 + 0x18);
    puVar7[0x10] = *(undefined4 *)(param_2 + 0x18);
  }
  iVar6 = *(int *)(param_2 + 0xc);
  puVar7[5] = iVar6;
  if (iVar6 == 0) {
    puVar7[0x1b] = 0;
    puVar7[8] = 0;
    puVar7[6] = 0;
  }
  else {
    iVar6 = *(int *)(param_2 + 0xc);
    puVar7[0x1b] = *(undefined4 *)(param_1 + 8);
    iVar6 = (iVar6 + 0x1fU >> 5) * 4;
    FUN_00564100(*(undefined4 *)(param_1 + 8),0,iVar6);
    iVar6 = *(int *)(param_1 + 8) + iVar6;
    *(int *)(param_1 + 8) = iVar6;
    puVar7[6] = iVar6;
    uVar4 = 0;
    *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + *(int *)(param_2 + 0xc) * 0x10;
    if (*(int *)(param_2 + 0xc) * 2 != 1) {
      do {
        *(uint *)(puVar7[6] + 4 + uVar4 * 8) = puVar7[6] + 8 + uVar4 * 8;
        uVar4 = uVar4 + 1;
      } while (uVar4 < *(int *)(param_2 + 0xc) * 2 - 1U);
    }
    *(undefined4 *)(puVar7[6] + 4 + uVar4 * 8) = 0;
    puVar7[7] = puVar7[6];
    puVar7[8] = *(undefined4 *)(param_1 + 8);
    FUN_00564100(*(undefined4 *)(param_1 + 8),0,*(int *)(param_2 + 8) << 2);
    *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + *(int *)(param_2 + 8) * 4;
  }
  puVar7[0x11] = *(undefined4 *)(param_2 + 8);
  puVar7[0x12] = *(undefined4 *)(param_2 + 0x10);
  puVar7[0x13] = 0;
  puVar7[0x14] = *(undefined4 *)(param_1 + 8);
  FUN_00564100(*(undefined4 *)(param_1 + 8),0,*(int *)(param_2 + 8) << 2);
  iVar6 = *(int *)(param_1 + 8) + *(int *)(param_2 + 8) * 4;
  *(int *)(param_1 + 8) = iVar6;
  puVar7[0x1f] = iVar6;
  iVar6 = *(int *)(param_1 + 8) + *(int *)(param_2 + 8) * 2;
  *(int *)(param_1 + 8) = iVar6;
  puVar7[0x20] = iVar6;
  *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + *(int *)(param_2 + 8) * 2;
  return puVar7;
}
