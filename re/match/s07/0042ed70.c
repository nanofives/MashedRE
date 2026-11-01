// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042ed70 (h5, by hand): 89.4% (regs 89.4%), raw 11.3%. The file holds the two short getters 0x0042b8b0/0x0042b8c0 (same bytes as their own s07 files): with them defined earlier in the TU the compiler keeps the x87 values live across the getter calls (externals spill every float, ~51%). sin/cos intrinsics, float r = FUN_004282a0(..)*0.5f, 7-arg FUN_00427ff0 whose 7th arg is param_6 as float. Tried: double/long double locals, angle local, inline SinF/CosF helpers, cast placements, operand orders, permuter 86 compiles. Residual (b): fmul 0.5 / fld param_6 / fmul deg2rad scheduled after the first two arg pushes in the original, and an fxch st(1) pair after fsin; no source order tried moves either. */
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
double sin(double);
double cos(double);
#pragma intrinsic(sin, cos)
MGLOBAL(short, DAT_0067ea54, 0x0067ea54);
MGLOBAL(short, DAT_0067ea56, 0x0067ea56);
void FUN_00427ff0(int p0, float p1, float p2, int p3, int p4, int p5, float p6);
float FUN_004282a0(int p0, int p1);
// MATCH: 0x0042b8b0 FUN_0042b8b0
short FUN_0042b8b0(void)
{
  return DAT_0067ea54;
}
// MATCH: 0x0042b8c0 FUN_0042b8c0
short FUN_0042b8c0(void)
{
  return DAT_0067ea56;
}
// MATCH: 0x0042ed70 FUN_0042ed70
void FUN_0042ed70(int param_1, float param_2, float param_3, int param_4, int param_5, float param_6)
{
  float r;

  r = FUN_004282a0(param_1, param_5) * 0.5f;
  FUN_00427ff0(param_1, param_2 - (float)cos(param_6 * 0.017453292f) * r,
               FUN_0042b8c0() * (float)sin(param_6 * 0.017453292f) * (r * 0.8f) / FUN_0042b8b0() + param_3,
               param_4, param_5, 0, param_6);
}
