// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00546cb0 (h2): 100.0% (regs 100). Raw had int-typed globals and float10 temp; floats for DAT_005cc320/32c and a float temp t fed to FUN_004c3b30 gives the dead-param-slot spill. */
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
typedef long double float10;
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
float10 FUN_004c3b30(float p0);
// MATCH: 0x00546cb0 FUN_00546cb0
void FUN_00546cb0(float *param_1,float *param_2)
{
  float10 fVar1;
  float t = (param_2[10] - (param_2[5] + *param_2)) + DAT_005cc320;
  fVar1 = (float10)FUN_004c3b30(t);
  param_1[2] = (float)((float10)DAT_005cc32c * fVar1);
  fVar1 = (float10)DAT_005cc32c / fVar1;
  param_1[3] = (float)(((float10)param_2[1] - (float10)param_2[4]) * fVar1);
  *param_1 = (float)(((float10)param_2[8] + (float10)param_2[2]) * fVar1);
  param_1[1] = (float)(((float10)param_2[9] + (float10)param_2[6]) * fVar1);
}
