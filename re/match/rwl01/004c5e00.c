// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c5e00: 77.7% regs 84.5%.  class (b) Address operand order fixed by writing DAT_007d3ff8 + k + DAT_007d4054. Remaining: original loads [DAT_007d4080] after the first branch and the do-while over the embedded list node (FUN_004c5a60(node-8,0), break on 0) allocates registers differently; goto-based Ghidra loop kept.. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4054, 0x007d4054);
MGLOBAL(undefined4 *, DAT_007d4080, 0x007d4080);
MGLOBAL(int, DAT_007d4058, 0x007d4058);
MGLOBAL(undefined4, DAT_00618150, 0x00618150);
undefined4 FUN_004c5a60(int * p0, int p1);
undefined4 FUN_004cc9f0(int p0);
int FUN_004d8060(int p0, undefined4 p1);
// MATCH: 0x004c5e00 FUN_004c5e00

undefined4 FUN_004c5e00(undefined4 param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  int iVar4;
  
  iVar4 = *(int *)(DAT_007d3ff8 + 0x24 + DAT_007d4054);
  if (iVar4 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(iVar4);
    *(undefined4 *)(DAT_007d3ff8 + 0x24 + DAT_007d4054) = 0;
    *(undefined2 *)(DAT_007d3ff8 + 0x28 + DAT_007d4054) = 0;
  }
  puVar2 = DAT_007d4080;
  puVar3 = (undefined4 *)(DAT_007d3ff8 + DAT_007d4054);
  if ((puVar3[2] != 0) && (puVar3[3] != 0)) {
    for (puVar1 = (undefined4 *)*puVar3; puVar1 != puVar3; puVar1 = (undefined4 *)*puVar1) {
      if (puVar1 + -4 == DAT_007d4080) {
        if ((undefined4 *)puVar3[4] == DAT_007d4080) {
          puVar3[4] = 0;
        }
        puVar3 = (undefined4 *)puVar2[2];
        goto joined_r0x004c5e91;
      }
    }
  }
  goto LAB_004c5eef;
  while( true ) {
    puVar1 = (undefined4 *)*puVar3;
    iVar4 = FUN_004c5a60((int *)(puVar3 + -2),0);
    puVar3 = puVar1;
    if (iVar4 == 0) break;
joined_r0x004c5e91:
    if (puVar3 == puVar2 + 2) break;
  }
  FUN_004d8060((int)&DAT_00618150,(undefined4)puVar2);
  *(undefined4 *)puVar2[5] = puVar2[4];
  *(undefined4 *)(puVar2[4] + 4) = puVar2[5];
  (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d3ff8 + 0xc + DAT_007d4054),puVar2);
  DAT_007d4080 = (undefined4 *)0x0;
LAB_004c5eef:
  iVar4 = *(int *)(DAT_007d3ff8 + 8 + DAT_007d4054);
  if (iVar4 != 0) {
    FUN_004cc9f0(iVar4);
    *(undefined4 *)(DAT_007d3ff8 + 8 + DAT_007d4054) = 0;
  }
  iVar4 = *(int *)(DAT_007d3ff8 + 0xc + DAT_007d4054);
  if (iVar4 != 0) {
    FUN_004cc9f0(iVar4);
    *(undefined4 *)(DAT_007d3ff8 + 0xc + DAT_007d4054) = 0;
  }
  DAT_007d4058 = DAT_007d4058 + -1;
  return param_1;
}
