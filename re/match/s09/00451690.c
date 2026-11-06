// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00451690 (h1): 100% (regs 100%). Raw converter 52.4%: the float argument passed straight from
   *(float *)(p+0x44) compiles to mov/push. A float local copy gives the original fld/fstp. Residual: none. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef unsigned int undefined4;
typedef unsigned int uint;
undefined4 FUN_0052df70(int * p0, float p1);
undefined4 FUN_00533ec0(uint * p0);
// MATCH: 0x00451690 FUN_00451690
undefined4 FUN_00451690(undefined4 param_1,int param_2)
{
  float f = *(float *)(param_2 + 0x44);
  FUN_0052df70(*(int **)(*(int *)(param_2 + 0x50) + 0x20),f);
  FUN_00533ec0(*(uint **)(param_2 + 0x50));
  *(undefined4 *)(param_2 + 0x48) = 5;
  return param_1;
}
