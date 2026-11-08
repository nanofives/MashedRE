// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 58.7% (regs 66.7%, permuter best (150 s). Residual not classified beyond register/schedule differences. Class: b. */
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
typedef unsigned short ushort;
MGLOBAL(undefined4, DAT_005d0c9c, 0x005d0c9c);
MGLOBAL(undefined4, DAT_005d0ca0, 0x005d0ca0);
MGLOBAL(undefined4, DAT_005d0ca4, 0x005d0ca4);
MGLOBAL(undefined4, DAT_005d0ca8, 0x005d0ca8);
MGLOBAL(undefined4, DAT_005d0c0c, 0x005d0c0c);
MGLOBAL(undefined4, DAT_005d0c10, 0x005d0c10);
MGLOBAL(undefined4, DAT_005d0c14, 0x005d0c14);
MGLOBAL(undefined4, DAT_005d0c18, 0x005d0c18);
void FUN_005bcb80(undefined4 p0, undefined4 * p1);
// MATCH: 0x005bcab0 FUN_005bcab0
void FUN_005bcab0(short *param_1,undefined4 *param_2)

{
  ushort uVar1;
  undefined4 uVar2;
  
  if (*param_1 == -2) {
    param_2[5] = *(undefined4 *)(param_1 + 0xe);
    param_2[6] = *(undefined4 *)(param_1 + 0x10);
    param_2[4] = *(undefined4 *)(param_1 + 0xc);
    param_2[7] = *(undefined4 *)(param_1 + 0x12);
  }
  else {
    FUN_005bcb80(*param_1,param_2 + 4);
  }
  *param_2 = DAT_005d0c9c;
  param_2[1] = DAT_005d0ca0;
  param_2[2] = DAT_005d0ca4;
  param_2[0xb] = DAT_005d0c0c;
  param_2[0xc] = DAT_005d0c10;
  param_2[0xd] = DAT_005d0c14;
  uVar2 = DAT_005d0c18;
  param_2[3] = DAT_005d0ca8;
  param_2[0xe] = uVar2;
  param_2[9] = 0;
  param_2[8] = 1;
  param_2[10] = (uint)(ushort)param_1[6];
  param_2[0xf] = 0;
  if (*param_1 == 1) {
    param_2[0x11] = (undefined4)param_1;
    param_2[0x10] = 0x12;
    return;
  }
  uVar1 = param_1[8];
  param_2[0x10] = uVar1 + 0x12;
  param_2[0x11] = (undefined4)param_1;
  return;
}
