// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00478030 (h4, by hand): 100%. 8-corner loop over the min/max box written directly */
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
int FUN_00477f50(float *param_1, float *param_2, float *param_3);

// MATCH: 0x00478030 FUN_00478030
int FUN_00478030(float *param_1, float *param_2)
{
  int i;
  int r;
  float c[3];

  for (i = 0; i < 8; i++) {
    if (i & 1) c[0] = param_2[0]; else c[0] = param_2[3];
    if (i & 2) c[1] = param_2[1]; else c[1] = param_2[4];
    if (i & 4) c[2] = param_2[2]; else c[2] = param_2[5];
    r = FUN_00477f50(param_1, c, 0);
    if (r == 0 || r == 1) return 0;
  }
  return 1;
}
