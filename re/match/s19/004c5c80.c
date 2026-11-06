// FLAGS: /O2 /GS /Ob1 /G5
// MATCH-S19 swap leg: return type void -> undefined4 returning param_1 (the original leaves param_1 in EAX). DDK bytes unchanged (match.py 100%, relocheck OK); the VS2022 port then returns the full EAX the original does.
/* MATCH-S19 0x004c5c80 (h4): 100% (regs 100%), relocheck OK. Raw converter body, RwGlobals pointer first. FLAGS add /G5: this RW-core code matches only with /G5 (also /G3,/G4); under the game flags /O2 /GS /Ob1 alone it is 80.0% (same param-load schedule as 0x004c5800). */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4054, 0x007d4054);
// MATCH: 0x004c5c80 FUN_004c5c80
undefined4 FUN_004c5c80(undefined4 param_1)

{
  *(undefined4 *)(DAT_007d3ff8 + DAT_007d4054 + 0x10) = param_1;
  return param_1;
}
