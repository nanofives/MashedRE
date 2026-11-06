// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x0047a0f0: 100% (regs 100%) by hand (h6), raw converter 57.9%.
   Fix: the &param_2 Ghidra shows is a separate local the compiler placed in param_2's dead home slot (param_2 lives
   in esi): write a dword local and pass &local, return param_2. FUN_00440bc0 passed as a function. Residual: none. */
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
MGLOBAL(undefined4, DAT_006bf1d8, 0x006bf1d8);
MGLOBAL(undefined4, DAT_006bf1dc, 0x006bf1dc);
void FUN_00440bc0(void);
undefined4 FUN_0047b9b0(undefined4 p0, void *p1, void (*p2)(void));
void FUN_004b6520(undefined4 p0, undefined4 p1);
// MATCH: 0x0047a0f0 FUN_0047a0f0
undefined4 FUN_0047a0f0(undefined4 param_1,undefined4 param_2)
{
  undefined4 local;
  DAT_006bf1d8 = param_2;
  DAT_006bf1dc = 0;
  FUN_004b6520(param_2,0x16264);
  FUN_0047b9b0(param_1,&local,FUN_00440bc0);
  return param_2;
}
