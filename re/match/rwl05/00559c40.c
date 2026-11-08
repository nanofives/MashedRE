// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00559c40 (h2): raw form untouched, 42.4%, class (b). C++ check FAILS (code() arity, int vs float*): not fixed. */
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
MGLOBAL(int, DAT_007dc8c0, 0x007dc8c0);
int FUN_00564c80(int p0, uint p1, float * p2);
// MATCH: 0x00559c40 FUN_00559c40

undefined4 * FUN_00559c40(undefined4 *param_1,int *param_2,uint param_3,byte param_4)

{
  ushort uVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint *puVar6;
  uint uVar7;
  int *piVar8;
  int iVar9;
  undefined1 local_20 [32];
  
  if (param_3 == 0xffffffff) {
    uVar3 = param_1[1];
    param_3 = 0;
    if (uVar3 != 0) {
      piVar8 = (int *)param_1[0x16];
      do {
        if (*piVar8 != -1) break;
        param_3 = param_3 + 1;
        piVar8 = piVar8 + 1;
      } while (param_3 < uVar3);
    }
    if (param_3 == uVar3) {
      return (undefined4 *)0x0;
    }
    uVar7 = 1;
    uVar3 = *(uint *)(param_1[0x16] + param_3 * 4);
    param_3 = param_3 << 5;
    uVar4 = uVar3 & 1;
    while (uVar4 != 0) {
      uVar7 = uVar7 * 2;
      param_3 = param_3 + 1;
      uVar4 = uVar7 & uVar3;
    }
    if ((uint)param_1[0x11] <= param_3) {
      return (undefined4 *)0x0;
    }
  }
  iVar9 = (param_3 >> 5) * 4;
  uVar3 = 1 << ((byte)param_3 & 0x1f);
  *(uint *)(param_1[0x16] + iVar9) = *(uint *)(param_1[0x16] + iVar9) | uVar3;
  uVar4 = ~uVar3;
  *(short *)(param_2 + 8) = (short)param_3;
  param_2[9] = (int)param_1;
  *(int **)(param_1[0x14] + param_3 * 4) = param_2;
  *(uint *)(param_1[0x17] + iVar9) = *(uint *)(param_1[0x17] + iVar9) & uVar4;
  *(uint *)(param_1[0x19] + iVar9) = *(uint *)(param_1[0x19] + iVar9) & uVar4;
  *(uint *)(param_1[0x18] + iVar9) = *(uint *)(param_1[0x18] + iVar9) & uVar4;
  iVar5 = (**(code **)(*param_2 + 0x18))(param_2,local_20);
  if (iVar5 == 0) {
    puVar6 = (uint *)(param_1[0x1a] + iVar9);
    uVar3 = *(uint *)(param_1[0x1a] + iVar9) & uVar4;
  }
  else {
    FUN_00564c80(*param_1,param_3,iVar5);
    puVar6 = (uint *)(param_1[0x1a] + iVar9);
    uVar3 = *puVar6 | uVar3;
  }
  *puVar6 = uVar3;
  uVar1 = *(ushort *)(param_2 + 8);
  if ((param_4 & 1) == 0) {
    puVar6 = (uint *)(param_1[0x17] + (uint)(uVar1 >> 5) * 4);
    uVar3 = *puVar6 & ~(1 << ((byte)uVar1 & 0x1f));
  }
  else {
    puVar6 = (uint *)(param_1[0x17] + (uint)(uVar1 >> 5) * 4);
    uVar3 = *puVar6 | 1 << ((byte)uVar1 & 0x1f);
  }
  *puVar6 = uVar3;
  bVar2 = (byte)*(ushort *)(param_2 + 8);
  uVar3 = (uint)(*(ushort *)(param_2 + 8) >> 5);
  if ((param_4 & 2) == 0) {
    uVar7 = 1 << (bVar2 & 0x1f);
    iVar5 = uVar3 * 4;
    if ((((*(uint *)(iVar5 + param_1[0x18]) & uVar7) != 0) && (param_2[9] != 0)) &&
       ((*(uint *)(iVar5 + *(int *)(param_2[9] + 0x60)) & uVar7) != 0)) {
      (**(code **)(*param_2 + 0x14))(param_2);
      puVar6 = (uint *)(param_1[0x18] + (uint)(*(ushort *)(param_2 + 8) >> 5) * 4);
      *puVar6 = *puVar6 & ~(1 << ((byte)*(ushort *)(param_2 + 8) & 0x1f));
      param_1[0x13] = param_1[0x13] + -1;
    }
    puVar6 = (uint *)(param_1[0x19] + (uint)(*(ushort *)(param_2 + 8) >> 5) * 4);
    uVar3 = *puVar6 & ~(1 << ((byte)*(ushort *)(param_2 + 8) & 0x1f));
  }
  else {
    puVar6 = (uint *)(param_1[0x19] + uVar3 * 4);
    uVar3 = *puVar6 | 1 << (bVar2 & 0x1f);
  }
  *puVar6 = uVar3;
  *(uint *)(iVar9 + param_1[0x18]) = *(uint *)(iVar9 + param_1[0x18]) & uVar4;
  if ((((param_4 & 4) != 0) &&
      ((param_2[9] == 0 ||
       ((*(uint *)(*(int *)(param_2[9] + 0x60) + (uint)(*(ushort *)(param_2 + 8) >> 5) * 4) &
        1 << ((byte)*(ushort *)(param_2 + 8) & 0x1f)) == 0)))) &&
     ((uint)param_1[0x13] < (uint)param_1[0x12])) {
    iVar5 = (**(code **)(*param_2 + 0x10))(param_2);
    if (iVar5 != 0) {
      puVar6 = (uint *)(param_1[0x18] + (uint)(*(ushort *)(param_2 + 8) >> 5) * 4);
      *puVar6 = *puVar6 | 1 << ((byte)*(ushort *)(param_2 + 8) & 0x1f);
      DAT_007dc8c0 = 0;
      param_1[0x13] = param_1[0x13] + 1;
      return param_1;
    }
    if (DAT_007dc8c0 == 0) {
      DAT_007dc8c0 = 1;
    }
  }
  return param_1;
}
