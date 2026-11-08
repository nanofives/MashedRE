// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h1 0x005751f0: 100.0% (regs 100.0%), class -. float f[6] array stops slot sharing; sum paren form. Tried: brute force sum order. cxx:ok. */
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
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x005751f0 FUN_005751f0
double FUN_005751f0(float *param_1,float *param_2,float *param_3)
{
  double u0, u1, u2;
  float f[6];
  double c2, n;

  u0 = param_2[0] - param_1[0];
  u1 = param_2[1] - param_1[1];
  u2 = param_2[2] - param_1[2];
  f[0] = param_3[0] - param_1[0];
  f[1] = param_3[1] - param_1[1];
  f[2] = param_3[2] - param_1[2];
  f[3] = f[2] * u1 - f[1] * u2;
  f[4] = f[0] * u2 - f[2] * u0;
  c2 = f[1] * u0 - f[0] * u1;
  n = f[2] * f[2] + (f[1] * f[1] + f[0] * f[0]);
  return (c2 * c2 + f[4] * f[4] + f[3] * f[3]) / (n * n + DAT_005cc320);
}
