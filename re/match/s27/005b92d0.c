// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b92d0: RO-AUDIOINIT-45D460-REACH-4, see below. */
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
/* RO-AUDIOINIT-45D460-REACH-4 (2026-11-22): new source from the listing of MASHED.exe.unpatched 0x005b92d0..0x005b92de (no
   hooks.csv row before this row; extent in re/console/pc_funcs.csv). 0x005b8680 stores its address at +0x3c
   (re/match/s27/005b8680.c line 56). Stores 1 at *param_2 (0x005b92d4) and returns param_1 (0x005b92da). */

// MATCH: 0x005b92d0 FUN_005b92d0
int FUN_005b92d0(int param_1, undefined4 *param_2)
{
  *param_2 = 1;
  return param_1;
}
