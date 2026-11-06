// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00474e70 (h7): best 86.0% (regs 89.5%), 57/57 ins. Hand form: three float[3] locals, float
   len, callee FUN_004c3ac0 (cdecl, 1 arg, float return; arity from the add esp,0xc after three calls).
   Raw converter was 28.1% (locals merged). Residual (b): the second vector p1-p3 is emitted in the original
   as fld [p1]; fsub [p3] for x but fld [p3]; fsubr [p1] for y and z, plus two scheduling slips (fstp len
   before the param_3 load, first cross product fld/fmul before lea edx). Tried: struct V3 form, inline Sub/
   Cross helpers (82.5/75.4), -(p3-p1), -p3+p1, compound -=, struct copy, local pointer copy, double
   return types, len in the param_2 slot, permuter 154 compiles: none moved it past 86.0. */
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
float FUN_004c3ac0(float *v);
// MATCH: 0x00474e70 FUN_00474e70
float FUN_00474e70(float *param_1, float *param_2, float *param_3)
{
  float a[3];
  float b[3];
  float c[3];
  float len;

  a[0] = *param_2 - *param_1;
  a[1] = param_2[1] - param_1[1];
  a[2] = param_2[2] - param_1[2];
  len = FUN_004c3ac0(a);
  b[0] = *param_1 - *param_3;
  b[1] = param_1[1] - param_3[1];
  b[2] = param_1[2] - param_3[2];
  FUN_004c3ac0(b);
  c[0] = b[2] * a[1] - b[1] * a[2];
  c[1] = b[0] * a[2] - b[2] * a[0];
  c[2] = b[1] * a[0] - b[0] * a[1];
  return FUN_004c3ac0(c) / len;
}
