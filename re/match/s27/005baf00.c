// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 82.6% (regs 82.6%, ops 91.3%), /G5. float param + float member self-copy, permuter swap. Residual: fld/fstp float copies of param_2 and member +0x1c come out as integer moves. Class: b. */
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
typedef unsigned int undefined4;
// MATCH: 0x005baf00 FUN_005baf00
void FUN_005baf00(int param_1, float param_2)
{
  float *puVar1;

  *(float *)(param_1 + 0x38) = param_2;
  puVar1 = *(float **)(param_1 + 0xc);
  for (; puVar1 != (float *)(param_1 + 0xc); puVar1 = *(float **)puVar1) {
    puVar1[7] = puVar1[7];
    ((uint *)puVar1)[5] = ((uint *)puVar1)[5] | 0x40;
  }
  if (*(int *)(param_1 + 0x11c) != 0) {
    *(int *)(*(int *)(param_1 + 0x11c) + 0x30) = *(int *)&param_2;
  }
}
