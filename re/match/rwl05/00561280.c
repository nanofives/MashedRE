// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00561280 (h2): hand + permuter, 65.1%, class (b): original has a 0xc local frame and spills the third dot product; sqrt via #pragma intrinsic(sqrt). */
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
double sqrt(double);
#pragma intrinsic(sqrt)
int FUN_0055bb70(int p0, int p1, float *p2);
// MATCH: 0x00561280 FUN_00561280
float * FUN_00561280(int param_1,int param_2,float *param_3)
{
  float *b;
  float *m;
  int *e;
  int *r;
  float ta, tb, tc, dx, dy, dz;

  param_2 = param_2 * 0xc;
  e = (int *)(*(int *)(param_1 + 0x10) + param_2);
  r = (int *)e[1];
  b = (float *)(**(int **)(*r + 0x10) + 0x30 + r[1] * 0x40);
  if (FUN_0055bb70(*e,0,param_3) == 0) {
    return 0;
  }
  m = *(float **)(param_2 + 8 + *(int *)(param_1 + 0x10));
  ta = m[4] * param_3[1] + m[8] * param_3[2] + m[0] * param_3[0];
  tb = m[5] * param_3[1] + m[1] * param_3[0] + m[9] * param_3[2];
  param_3[1] = *(float *)(*(int *)(param_2 + 8 + *(int *)(param_1 + 0x10)) + 0x34) + tb;
  tc = m[6] * param_3[1] + m[2] * param_3[0] + m[10] * param_3[2];
  param_3[0] = m[12] + ta;
  param_3[2] = *(float *)(*(int *)(param_2 + 8 + *(int *)(param_1 + 0x10)) + 0x38) + tc;
  dz = b[2] - param_3[2];
  dy = b[1] - param_3[1];
  dx = b[0] - param_3[0];
  param_3[3] = (float)sqrt(dx * dx + dy * dy + dz * dz) + param_3[3];
  param_3[1] = b[1];
  param_3[0] = b[0];
  param_3[2] = b[2];
  return param_3;
}
