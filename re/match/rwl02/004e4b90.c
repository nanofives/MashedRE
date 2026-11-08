// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched.
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

MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cd108, 0x005cd108);
double cos(double);
#pragma intrinsic(cos)
// MATCH: 0x004e4b90 RpLightSetConeAngle
int RpLightSetConeAngle(int param_1, float param_2)
{
  if (!(param_2 < DAT_005d757c) && !(param_2 > DAT_005cd108)) {
    *(float *)(param_1 + 0x28) = (float)-cos((double)param_2);
    return param_1;
  }
  return 0;
}
