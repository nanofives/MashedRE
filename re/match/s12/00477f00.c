// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00477f00 (h4, by hand): 96.4% (regs 96.4%). Struct copy of the normal (gives the mov ecx,esi), dot split as d = x+y; d = d + z (one expression scores 78.6%: the compiler reorders the products). Tried: dot operand orders, Plane/V3 typed params, d += chain, permuter (no gain). Residual (a): the param_2 load is scheduled before the first fld here, after it in the original */
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
float FUN_004c39b0(float *out, float *in);

typedef struct V3 { float x, y, z; } V3;

// MATCH: 0x00477f00 FUN_00477f00
void FUN_00477f00(float *param_1, float *param_2, float *param_3)
{
  float d;
  *(V3 *)param_1 = *(V3 *)param_3;
  FUN_004c39b0(param_1, param_1);
  d = param_1[0] * param_2[0] + param_1[1] * param_2[1];
  d = d + param_1[2] * param_2[2];
  param_1[3] = -d;
}
