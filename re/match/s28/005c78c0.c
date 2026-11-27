// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c78c0: RO-AUDIOINIT-45D460-REACH-6, see below. */
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
/* RO-AUDIOINIT-45D460-REACH-6 (2026-11-27): new source from the listing of MASHED.exe.unpatched 0x005c78c0..0x005c792c
   (109 B; no Ghidra function, no hooks.csv row, no source before this row). Its address is stored in .data at 0x006350cc
   and 0x00636024 (file dwords). When DAT_007de1b0 is non-zero (0x005c78c6), calls 0x005a6dc0(DAT_007de1b0[i], 6, 1, 0)
   for i = 0 .. DAT_007de1b4-1, unsigned (0x005c78d5..0x005c78f4, both globals re-read each pass). Then, with param_3
   non-zero (0x005c78fb), stores param_3[1] at 0x007de1b0 (0x005c7902) and param_3[0] at 0x007de1b4 (0x005c790a), else 0
   into both (0x005c7918 / 0x005c7922). Returns param_1 (0x005c790f / 0x005c7914). */

MGLOBAL(undefined4 *, DAT_007de1b0, 0x007de1b0);
MGLOBAL(uint, DAT_007de1b4, 0x007de1b4);

int FUN_005a6dc0(int param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4);

// MATCH: 0x005c78c0 FUN_005c78c0
int FUN_005c78c0(int param_1, undefined4 param_2, undefined4 *param_3)
{
  uint i;

  i = 0;
  if (DAT_007de1b0 != 0) {
    while (i < DAT_007de1b4) {
      FUN_005a6dc0(DAT_007de1b0[i], 6, 1, 0);
      i = i + 1;
    }
  }
  if (param_3 != 0) {
    DAT_007de1b0 = (undefined4 *)param_3[1];
    DAT_007de1b4 = *param_3;
    return param_1;
  }
  DAT_007de1b0 = 0;
  DAT_007de1b4 = 0;
  return param_1;
}
