// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00476320 (h2): 91.7% (regs 91.7%, ops 97.2%), raw 77.8%. The 0x005cd7a8 factor (0.017453292f) is a
   float literal (as a global the compiler loads it first: -6%); y held in a double local gives the original
   fld p2[1]; fadd st(1); fstp st(0). Residual: p1[2] = p2[2] + z loads p2[2] first in the original (ours loads the
   spilled z), and the discarded FUN_004c39b0 result is popped before add esp (ours after). Tried: struct V3 form,
   RW macro forms (x*s, +=), operand swaps, double/int/void callee return, double x/z/s, permuter (no gain).
   Class (b).
   MATCH-RACEPATH-PILOT 2026-11-09: CLOSED, 103 B BYTE-IDENTICAL. The z sum is done in double and cast back
   (`(float)(param_2[2] + (double)z)`): param_2[2] loads first, the z spill is the fadd operand, and the result's
   fstp st(0) after the call moves past add esp like the original. */
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
double sin(double);
#pragma intrinsic(sin)
void FUN_004762c0(float *p0);
float FUN_004c39b0(float *p0, float *p1);
// MATCH: 0x00476320 FUN_00476320
void FUN_00476320(float *param_1, float *param_2, float param_3)
{
  float s, x, z; double y;

  s = (float)sin(param_3 * 0.017453292f);
  FUN_004762c0(param_1);
  x = s * param_1[0];
  param_1[0] = x;
  y = s * param_1[1];
  param_1[1] = y;
  z = s * param_1[2];
  param_1[2] = z;
  param_1[0] = x + param_2[0];
  param_1[1] = param_2[1] + y;
  param_1[2] = (float)(param_2[2] + (double)z);
  FUN_004c39b0(param_1, param_1);
}
