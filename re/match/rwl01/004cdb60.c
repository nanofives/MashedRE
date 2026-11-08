// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004cdb60: 100% regs 100%. Global sums written DAT_007d3ff8 + k + DAT_007d4628 (source operand order sets load order and base/index). class -. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d4628, 0x007d4628);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d462c, 0x007d462c);
undefined4 FUN_004cc9f0(int p0);
// MATCH: 0x004cdb60 FUN_004cdb60

undefined4 FUN_004cdb60(undefined4 param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(DAT_007d3ff8 + 0x210 + DAT_007d4628);
  if (iVar1 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);
    *(undefined4 *)(DAT_007d3ff8 + 0x210 + DAT_007d4628) = 0;
    *(undefined4 *)(DAT_007d3ff8 + 0x214 + DAT_007d4628) = 0;
  }
  iVar1 = *(int *)(DAT_007d3ff8 + 4 + DAT_007d4628);
  if (iVar1 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);
    *(undefined4 *)(DAT_007d3ff8 + 4 + DAT_007d4628) = 0;
    *(undefined4 *)(DAT_007d3ff8 + 8 + DAT_007d4628) = 0;
  }
  iVar1 = *(int *)(DAT_007d3ff8 + 0x21c + DAT_007d4628);
  while (iVar1 != 0) {
    *(undefined4 *)(DAT_007d3ff8 + 0x21c + DAT_007d4628) = *(undefined4 *)(iVar1 + 0x30);
    (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d3ff8 + 0x218 + DAT_007d4628),iVar1);
    iVar1 = *(int *)(DAT_007d3ff8 + 0x21c + DAT_007d4628);
  }
  iVar1 = *(int *)(DAT_007d3ff8 + 0x218 + DAT_007d4628);
  if (iVar1 != 0) {
    FUN_004cc9f0(iVar1);
    *(undefined4 *)(DAT_007d3ff8 + 0x218 + DAT_007d4628) = 0;
  }
  if (*(int *)(DAT_007d3ff8 + DAT_007d4628) != 0) {
    FUN_004cc9f0(*(int *)(DAT_007d3ff8 + DAT_007d4628));
    *(undefined4 *)(DAT_007d3ff8 + DAT_007d4628) = 0;
  }
  DAT_007d462c = DAT_007d462c + -1;
  return param_1;
}
