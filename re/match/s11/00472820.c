// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00472820 (h7): 100%. Three _finite checks (FUN_004a3907, cdecl double) then "z*z + (x*x + y*y) <= 1e-6 ->
   return 0" (the <= form gives test ah,0x41 / jnp; ">" gave jne). Residual: none. */
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
MGLOBAL(float, DAT_005ce54c, 0x005ce54c);
int FUN_004a3907(double x);
// MATCH: 0x00472820 FUN_00472820
int FUN_00472820(float *param_1)
{
  if (!FUN_004a3907(param_1[0]) || !FUN_004a3907(param_1[1]) || !FUN_004a3907(param_1[2])) {
    return 0;
  }
  if (param_1[2] * param_1[2] + (param_1[0] * param_1[0] + param_1[1] * param_1[1]) <= DAT_005ce54c) {
    return 0;
  }
  return 1;
}
