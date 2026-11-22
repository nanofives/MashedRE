// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c92c0: RO-AUDIOINIT-45D460-REACH-4, see below. */
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
/* RO-AUDIOINIT-45D460-REACH-4 (2026-11-22): new source from the listing of MASHED.exe.unpatched 0x005c92c0..0x005c92ec (no
   Ghidra function, no hooks.csv row before this row). 0x005c9230 stores its address at record +0xc
   (re/match/s28/005c9230.c line 48). Stores param_2 at +0x50 (0x005c92d3), calls 0x005aea50(*(param_1+0x34)+2, 1, 0x30806) (0x005c92da), stores the
   result at +0x4c (0x005c92e2), returns param_1 when the result is non-zero, else 0 (neg/sbb/and at 0x005c92e5). */

int FUN_005aea50(int param_1, int param_2, int param_3);

// MATCH: 0x005c92c0 FUN_005c92c0
int FUN_005c92c0(int param_1, undefined4 param_2)
{
  int iVar1;

  *(undefined4 *)(param_1 + 0x50) = param_2;
  iVar1 = FUN_005aea50(*(int *)(param_1 + 0x34) + 2, 1, 0x30806);
  *(int *)(param_1 + 0x4c) = iVar1;
  return (iVar1 != 0) ? param_1 : 0;
}
