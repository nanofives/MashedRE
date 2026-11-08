// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e8c70: best 28.9% (regs 54.4%), residual class b. structure only (543 B), Ghidra form with gotos kept; 28.9. */
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

MGLOBAL(int, DAT_006186d0, 0x006186d0);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0);
int FUN_004d8000(int *p0, undefined1 *p1);
undefined4 * FUN_004d8480(undefined4 *p0);
int FUN_004e8750(undefined1 *p0, int p1);
int * FUN_004f3b60(undefined1 *p0);
int FUN_004f3bc0(undefined1 *p0);
// MATCH: 0x004e8c70 FUN_004e8c70
undefined1 * FUN_004e8c70(int param_1,int param_2,uint param_3)
{
  undefined1 *puVar1;
  int iVar2;
  undefined1 *puVar3;
  undefined4 *puVar4;
  uint uVar5;
  uint uVar6;
  undefined4 *puVar7;
  uint local_8;
  undefined4 local_4;
  if (param_1 < 0) {
    return (undefined1 *)0;
  }
  if (0xffff < param_1) {
    local_8 = 2;
    local_4 = FUN_004d7ff0(6);
    FUN_004d8480(&local_8);
    return (undefined1 *)0;
  }
  if (param_2 < 0) {
    return (undefined1 *)0;
  }
  if ((param_3 & 0xff0000) == 0) {
    if (-1 < (char)(byte)param_3) {
      uVar6 = param_3 >> 2 & 1;
      goto LAB_004e8e3a;
    }
    uVar6 = 2;
  }
  else {
    uVar6 = param_3 >> 0x10 & 0xff;
LAB_004e8e3a:
    if (uVar6 == 1) {
      uVar5 = 4;
      goto LAB_004e8cdc;
    }
  }
  uVar5 = -(uint)(1 < uVar6) & 0x80;
LAB_004e8cdc:
  local_8 = param_3 & 0x1000000;
  iVar2 = DAT_006186d0;
  if (local_8 == 0) {
    if ((param_3 & 8) != 0) {
      iVar2 = DAT_006186d0 + param_1 * 4;
    }
    if (uVar6 != 0) {
      iVar2 = iVar2 + uVar6 * param_1 * 8;
    }
    iVar2 = iVar2 + param_2 * 8;
  }
  puVar1 = (undefined1 *)(**(int (**)(int,int))(DAT_007d3ff8 + 0x108))(iVar2,0x3000f);
  if ((puVar1 == (undefined1 *)0) || (iVar2 = FUN_004f3bc0(puVar1 + 0x20), iVar2 == 0)) {
    return (undefined1 *)0;
  }
  puVar4 = (undefined4 *)(puVar1 + 0x34);
  *(undefined4 *)(puVar1 + 0x5c) = 0;
  *(undefined4 *)(puVar1 + 0x18) = 0;
  *(undefined4 *)(puVar1 + 4) = 0;
  *(undefined4 *)(puVar1 + 0x58) = 0;
  *(undefined2 *)(puVar1 + 0xc) = 0;
  *(undefined4 *)(puVar1 + 0x54) = 0;
  *puVar1 = 8;
  puVar1[1] = 0;
  puVar1[2] = 0;
  puVar1[3] = 0;
  *(undefined2 *)(puVar1 + 0xe) = 1;
  *(uint *)(puVar1 + 0x1c) = uVar6;
  puVar7 = puVar4;
  for (iVar2 = 8; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar7 = 0;
    puVar7 = puVar7 + 1;
  }
  *(undefined4 *)(puVar1 + 0x30) = 0;
  *(undefined4 *)(puVar1 + 0x2c) = 0;
  *(int *)(puVar1 + 0x10) = param_2;
  *(uint *)(puVar1 + 8) = param_3 & 0xf000000 | (byte)param_3 & 0x7b | uVar5;
  *(int *)(puVar1 + 0x14) = param_1;
  if (local_8 == 0) {
    puVar3 = puVar1 + DAT_006186d0;
    if (((param_3 & 8) != 0) && (param_1 != 0)) {
      *(undefined1 **)(puVar1 + 0x30) = puVar3;
      puVar3 = puVar3 + param_1 * 4;
    }
    if (((uVar6 != 0) && (param_1 != 0)) && (uVar6 != 0)) {
      do {
        *puVar4 = (undefined4)puVar3;
        puVar3 = puVar3 + param_1 * 8;
        puVar4 = puVar4 + 1;
        uVar6 = uVar6 - 1;
      } while (uVar6 != 0);
    }
    if (param_2 != 0) {
      *(undefined1 **)(puVar1 + 0x2c) = puVar3;
      iVar2 = 0;
      if (0 < param_2) {
        do {
          iVar2 = iVar2 + 1;
          *(undefined2 *)(*(int *)(puVar1 + 0x2c) + -2 + iVar2 * 8) = 0xffff;
        } while (iVar2 < param_2);
      }
    }
  }
  iVar2 = FUN_004e8750(puVar1,1);
  if (iVar2 < 0) {
    FUN_004f3b60(puVar1 + 0x20);
    (**(void (**)(undefined1 *))(DAT_007d3ff8 + 0x10c))(puVar1);
    return (undefined1 *)0;
  }
  FUN_004d8000(&DAT_006186d0,puVar1);
  return puVar1;
}
