// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004728a0 (h7): 100%. Four FUN_00472820 checks on matrix rows +0x20, +0x10, +0, +0x30. The else-if chain
   ending in "? 1 : 0" placed the return-0 block after the first test; plain ifs, AND chains and "!= 0" gave 71-81%.
   Residual: none. */
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
int FUN_00472820(float *param_1);
// MATCH: 0x004728a0 FUN_004728a0
int FUN_004728a0(float *param_1)
{
  if (!FUN_00472820(param_1 + 8)) return 0;
  else if (!FUN_00472820(param_1 + 4)) return 0;
  else if (!FUN_00472820(param_1)) return 0;
  else return FUN_00472820(param_1 + 12) ? 1 : 0;
}
