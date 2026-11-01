// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00431b20 (h9): 100% (regs 100%). Residual: none. Raw g2s 10% (unkbyte10 fsin pseudo-call). Hand form: CRT sin as
   intrinsic (#pragma intrinsic(sin)), float global times the double constant 1.57 at 0x005cd8f0. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBAL(float, DAT_007f0f00, 0x007f0f00);
MGLOBAL(double, DAT_005cd8f0, 0x005cd8f0);
double sin(double);
#pragma intrinsic(sin)
// MATCH: 0x00431b20 FUN_00431b20
double FUN_00431b20(void)
{
  return sin(DAT_007f0f00 * DAT_005cd8f0);
}
