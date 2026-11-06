// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004726f0 (h7): 95.8% (regs 95.8%). 3-element dot clamped to [-1, 1] (0x5cc33c / 0x5cc320). Only diff:
   the first fld [eax+4] is scheduled after mov ecx,[esp+8] instead of before it (the 2-element twin 0x004726b0
   gets it before). Tried about 20 forms: every grouping/operand order of the sum, split statements, double
   accumulator, V3 struct (const), inline dot3 / inline clamp helpers, ternary clamp, and the permuter (86
   compiles, no gain). Residual class (a): scheduling. */
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
MGLOBAL(float, DAT_005cc33c, 0x005cc33c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x004726f0 FUN_004726f0
float FUN_004726f0(float *param_1,float *param_2)
{
  float d = param_1[2] * param_2[2] + (param_1[0] * param_2[0] + param_1[1] * param_2[1]);
  if (d < DAT_005cc33c) {
    return DAT_005cc33c;
  }
  if (d > DAT_005cc320) {
    return DAT_005cc320;
  }
  return d;
}
