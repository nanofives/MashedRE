// FLAGS: /O1 /Ob1 /G5
/* MATCH-RWL05 0x0055b750 (h2): hand, 42.4%, class (b): flags /O1 /Ob1 /G5 (raw form was /O1 37%). Original is frameless with st(n) reuse of x,y,z; mine builds an ebp frame and hoists the n loads. Source form not found. */
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
// MATCH: 0x0055b750 FUN_0055b750
void FUN_0055b750(int *param_1,float *param_2,float *param_3)
{
  float *m;
  float *n;
  float x, y, z, t1, t2, t3;

  m = (float *)(param_1[1] * 0x40 + **(int **)(*param_1 + 0x10));
  x = param_2[0] - m[12];
  y = param_2[1] - m[13];
  z = param_2[2] - m[14];
  n = (float *)(param_1[1] * 0x20 + (*(int **)(*param_1 + 0x10))[2]);
  t1 = n[5] * z - n[6] * y;
  t2 = n[6] * x - n[4] * z;
  t3 = n[4] * y - n[5] * x;
  param_3[0] = n[0] + t1;
  param_3[1] = *(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 8) + 4 + param_1[1] * 0x20) + t2;
  param_3[2] = *(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 8) + 8 + param_1[1] * 0x20) + t3;
}
