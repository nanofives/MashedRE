// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a73b0 (h2): best 100.0% (regs 100.0%), class -. C++ fix: variadic prototypes for callees the original pushes extra args to. relocheck. */
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
MGLOBAL(undefined4, DAT_007dca34, 0x007dca34);
MGLOBAL(int, DAT_007dca38, 0x007dca38);
MGLOBAL(int, DAT_007dca20, 0x007dca20);
MGLOBAL(undefined1, DAT_007dca30, 0x007dca30);
void FUN_004522d0(int p0, ...);
void FUN_005a6cb0(undefined4 p0, undefined4 p1);
void FUN_005a6df0(undefined4 p0, ...);
MGLOBAL(int, _DAT_007dca30, 0x007dca30);
// MATCH: 0x005a73b0 FUN_005a73b0
/* [C2 2026-06-01] Voice-array teardown, reverse of FUN_005a71f0. `void FUN_005a73b0(void)`. */

void FUN_005a73b0(void)

{
  undefined4 uVar1;
  int iVar2;
  
  FUN_005a6df0(DAT_007dca34,5,1,0);
  iVar2 = 0;
  if (0 < DAT_007dca38) {
    do {
      FUN_005a6cb0(*(undefined4 *)(DAT_007dca20 + iVar2 * 4),0);
      iVar2 = iVar2 + 1;
    } while (iVar2 < DAT_007dca38);
  }
  uVar1 = DAT_007dca34;
  FUN_005a6cb0(DAT_007dca34,0);
  FUN_004522d0(uVar1);
  _DAT_007dca30 = 0;
  DAT_007dca38 = 0;
  return;
}
