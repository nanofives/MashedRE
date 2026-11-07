// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bce80: 69.6% (regs 87.0%). Hand: FUN_005bcef0/d640/d6f0/d9e0 are functions (cast stores). Residual (b): original keeps 0x7b8 in eax across both [+0x40] stores (mov eax,0x7b8 hoisted); local var, int* indexing, permuter 222 compiles did not reproduce it. */
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
MGLOBAL(char, DAT_00617ff9, 0x00617ff9);
MGLOBAL(undefined1, DAT_005e7140, 0x005e7140);
MGLOBAL(undefined1, DAT_00635810, 0x00635810);
void FUN_005bd9e0(void);
void FUN_005bd6f0(void);
void FUN_005bcef0(void);
void FUN_005bd640(void);
undefined4 FUN_005a9e10(int p0, void *p1, int p2);
// MATCH: 0x005bce80 FUN_005bce80
void FUN_005bce80(int param_1)
{
  int iVar1;

  FUN_005a9e10(param_1, &DAT_005e7140, 0);
  *(undefined4 *)(param_1 + 0x40) = 0x7b8;
  iVar1 = (int)DAT_00617ff9;
  *(undefined4 *)(param_1 + 0x28) = (undefined4)FUN_005bcef0;
  *(undefined4 *)(param_1 + 0x34) = (undefined4)FUN_005bd640;
  *(uint *)(param_1 + 0x40) = (iVar1 + -1) * 0x10000000 | 0x7b8;
  *(undefined4 *)(param_1 + 0x2c) = 0;
  *(undefined4 *)(param_1 + 0x30) = (undefined4)FUN_005bd6f0;
  *(undefined4 *)(param_1 + 0x3c) = (undefined4)FUN_005bd9e0;
  *(unsigned short *)(param_1 + 0x44) = 0x60;
  *(undefined4 *)(param_1 + 0x54) = (undefined4)&DAT_00635810;
}
