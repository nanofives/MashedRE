// FLAGS: /O2 /Ob1 /G5
/* RESULT: pending */
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
// MATCH: 0x00568860 FUN_00568860
void FUN_00568860(int param_1,uint *param_2,undefined4 param_3,int param_4)

{
  uint uVar1;
  
  param_2[10] = param_2[5] = *(int *)(param_4 + 0x30) + 1;
  param_2[2] = *(int *)(param_4 + 0x30) * 2;
  param_2[8] = *(int *)(param_4 + 0x30) * 2;
  uVar1 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  param_2[3] = uVar1;
  uVar1 = *(int *)(param_1 + 8) + 0xf + param_2[5] * 0x14 & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  param_2[9] = uVar1;
  uVar1 = param_2[10] * 0x20 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  *param_2 = uVar1;
  uVar1 = *(int *)(param_1 + 8) + 0xf + param_2[2] * 8 & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  param_2[6] = uVar1;
  *(uint *)(param_1 + 8) = param_2[8] * 0x40 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  param_2[0xd] = *(uint *)(param_4 + 0x34);
  param_2[0x10] = *(int *)(param_4 + 0x34) * 2 + 0x10;
  uVar1 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  param_2[0xb] = uVar1;
  uVar1 = param_2[0xd] * 0x120 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  param_2[0xe] = uVar1;
  *(uint *)(param_1 + 8) = param_2[0x10] * 0x10 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  param_2[0x13] = *(uint *)(param_4 + 0x38);
  param_2[0x18] = *(uint *)(param_4 + 0x38);
  uVar1 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  param_2[0x11] = uVar1;
  uVar1 = *(int *)(param_1 + 8) + 0xf + param_2[0x13] * 0x4c & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  param_2[0x16] = uVar1;
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + param_2[0x18] * 0x60;
  param_2[0x1f] = 1;
  return;
}
