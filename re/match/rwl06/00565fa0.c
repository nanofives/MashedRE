// FLAGS: /O2 /Ob1 /G5
/* RESULT: pending */
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
// MATCH: 0x00565fa0 FUN_00565fa0
void FUN_00565fa0(float *param_1,float *param_2,float *param_3,float param_4)
{
  if (param_2[0] < param_3[0]) {
    param_1[4] = param_2[0] - param_4;
    param_1[0] = param_4 + param_3[0];
  }
  else {
    param_1[4] = param_3[0] - param_4;
    param_1[0] = param_4 + param_2[0];
  }
  if (param_2[1] < param_3[1]) {
    param_1[5] = param_2[1] - param_4;
    param_1[1] = param_4 + param_3[1];
  }
  else {
    param_1[5] = param_3[1] - param_4;
    param_1[1] = param_4 + param_2[1];
  }
  if (param_2[2] < param_3[2]) {
    param_1[6] = param_2[2] - param_4;
    param_1[2] = param_4 + param_3[2];
    return;
  }
  param_1[6] = param_3[2] - param_4;
  param_1[2] = param_4 + param_2[2];
}
