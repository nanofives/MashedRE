// FLAGS: /O2 /GS /Ob1
/* 0x00411350 FUN_00411350 (87 B): best 100.0% exact, regs 100.0%.
   Ghidra's extraout_ST0 is the x87 value left over the __ftol call (FUN_004a2c48 = CRT _ftol), not a
   register-passed argument: no static callee needed. param_1 is unsigned (fild + fadd 2^32 at 0x005cc94c),
   scale 1/3000 is the float at 0x005cc948 (0x39aec33e). Plain float local + (int) cast. No residual. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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

// MATCH: 0x00411350 FUN_00411350
void FUN_00411350(unsigned int param_1, float *param_2, int *param_3, int *param_4)
{
  float t;

  t = (float)param_1 * 0.00033333333f;
  *param_3 = (int)t;
  *param_2 = t - (float)*param_3;
  *param_4 = 0;
  while (*param_3 > 59) {
    *param_4 = *param_4 + 1;
    *param_3 = *param_3 - 60;
  }
}
