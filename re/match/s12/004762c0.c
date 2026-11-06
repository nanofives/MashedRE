// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x004762c0 (h2): 100%. Raw converter failed to compile (fcos/fsin pseudo-calls, __fastcall callee).
   Hand form: cdecl float callees (0x004b4510 range random, 0x004c3b30 one-float), intrinsic sin/cos, and the
   radius held in a double local (a float local spilled/reloaded and gave 90%). Residual: none. */
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
double cos(double);
#pragma intrinsic(sin, cos)
MGLOBAL(float, DAT_005cc320, 0x005cc320);
float FUN_004b4510(float p0, float p1);
float FUN_004c3b30(float p0);
// MATCH: 0x004762c0 FUN_004762c0
void FUN_004762c0(float *param_1)
{
  float a;
  double r;

  param_1[2] = FUN_004b4510(-1.0f, 1.0f);
  a = FUN_004b4510(0.0f, 6.2831855f);
  r = FUN_004c3b30(DAT_005cc320 - param_1[2] * param_1[2]);
  param_1[0] = (float)(cos(a) * r);
  param_1[1] = (float)(sin(a) * r);
}
