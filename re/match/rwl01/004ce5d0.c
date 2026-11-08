// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004ce5d0: 63.0% regs 71.7%.  class (b) Pushes hoisted and branch order (realloc-style [+0x110] first) match; remaining: original loads both globals and the capacity into registers before sub esp,8 and keeps a different frame layout for the 8-byte error struct. Tried capacity local, compare orders.. */
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
typedef int code();
MGLOBAL(int, DAT_007d4628, 0x007d4628);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
typedef int code2(int, int);
typedef int code3(int, int, int);
undefined4 FUN_004d7ff0(undefined4 p0, int p1);
undefined4 * FUN_004d8480(undefined4 * p0);
typedef int code2(int, int);
typedef int code3(int, int, int);
undefined4 FUN_004d7ff0(undefined4 p0, int p1);
undefined4 * FUN_004d8480(undefined4 * p0);
typedef int code2(int, int);
typedef int code3(int, int, int);
undefined4 FUN_004d7ff0(undefined4 p0, int p1);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004ce5d0 FUN_004ce5d0
undefined4 FUN_004ce5d0(int param_1)
{
  int iVar1;
  int iVar2;
  undefined4 uStack_8;
  undefined4 uStack_4;

  iVar2 = *(int *)(DAT_007d4628 + DAT_007d3ff8 + 0x214);
  if (param_1 > iVar2) {
    iVar1 = *(int *)(DAT_007d4628 + DAT_007d3ff8 + 0x210);
    if (iVar1 != 0) {
      iVar1 = (**(code3 **)(DAT_007d3ff8 + 0x110))(iVar1,param_1,0x1040018);
    }
    else {
      iVar1 = (**(code2 **)(DAT_007d3ff8 + 0x108))(param_1,0x1040018);
    }
    if (iVar1 == 0) {
      uStack_8 = 1;
      uStack_4 = FUN_004d7ff0(0x80000013,param_1);
      FUN_004d8480(&uStack_8);
      return 0;
    }
    *(int *)(DAT_007d4628 + DAT_007d3ff8 + 0x210) = iVar1;
    *(int *)(DAT_007d4628 + DAT_007d3ff8 + 0x214) = param_1;
  }
  return *(undefined4 *)(DAT_007d4628 + DAT_007d3ff8 + 0x210);
}
