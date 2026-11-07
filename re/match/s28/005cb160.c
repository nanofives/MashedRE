// FLAGS: /O1 /Oy-
// best 24.4 (regs 24.4, ops 62.2). D3DX vec3 x 3x3 transform, hand-written C (dest, v, m; returns v). Original is hand-scheduled FPU code (3 accumulators on the x87 stack, saves edx/ebx/eax around it); no C form reproduces the schedule. Class (c).
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
// MATCH: 0x005cb160 FUN_005cb160
float *FUN_005cb160(float *param_1, float *param_2, float *param_3)
{
  float fVar1 = param_2[0];
  float fVar4 = param_2[1];
  float fVar7 = param_2[2];
  param_1[0] = fVar1 * param_3[0] + fVar4 * param_3[4] + fVar7 * param_3[8];
  param_1[1] = fVar1 * param_3[1] + fVar4 * param_3[5] + fVar7 * param_3[9];
  param_1[2] = fVar1 * param_3[2] + fVar4 * param_3[6] + fVar7 * param_3[10];
  return param_2;
}
