// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x004790e0 (h5): 100% (regs 100%). Register-arg function (world pointer in EAX, Ghidra in_EAX):
   pilot finding 1, FUN_004790e0 is `static` and called from a small stand-in caller in this file (the real caller
   is FUN_00479330 at 0x00479e2e, which loads [ebx+0x105d4] into EAX). The stand-in is not the real TU.
   Callback FUN_00479030 is passed by address (0x004790e2). Residual: none. */
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
int FUN_00479030(int param_1, int param_2);
int FUN_004e5c70(int param_1, int (*param_2)(int, int), int param_3);
// MATCH: 0x004790e0 FUN_004790e0
static void FUN_004790e0(int param_1)
{
  FUN_004e5c70(param_1, FUN_00479030, 0);
}

/* stand-in caller (0x00479e28..0x00479e2e of FUN_00479330 passes [ebx+0x105d4] in EAX) */
void FUN_004790e0_caller(int param_1)
{
  FUN_004790e0(*(int *)(param_1 + 0x105d4));
}
