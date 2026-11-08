// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h3 0x005659f0: best 58.8% (regs 49.0%, ops 92.2%), class (b). double temps t0..t5 (6 results then 6 stores) give 58.8; original spills only t2..t5 as float; float/double mixes tried (64 combos). */
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
// MATCH: 0x005659f0 FUN_005659f0
void FUN_005659f0(int param_1,int param_2,int param_3,float param_4,float param_5)

{
  double t0;
  double t1;
  double t2;
  double t3;
  double t4;
  double t5;

  t0 = param_4 * *(float *)(param_1 + param_2 * 4) - param_5 * *(float *)(param_1 + param_3 * 4);
  t1 = param_4 * *(float *)(param_1 + 0xc + param_2 * 4) - param_5 * *(float *)(param_1 + 0xc + param_3 * 4);
  t2 = param_4 * *(float *)(param_1 + 0x18 + param_2 * 4) - param_5 * *(float *)(param_1 + 0x18 + param_3 * 4);
  t3 = param_4 * *(float *)(param_1 + param_3 * 4) + param_5 * *(float *)(param_1 + param_2 * 4);
  t4 = param_4 * *(float *)(param_1 + 0xc + param_3 * 4) + param_5 * *(float *)(param_1 + 0xc + param_2 * 4);
  t5 = param_5 * *(float *)(param_1 + 0x18 + param_2 * 4) + param_4 * *(float *)(param_1 + 0x18 + param_3 * 4);
  *(float *)(param_1 + param_2 * 4) = t0;
  *(float *)(param_1 + 0xc + param_2 * 4) = t1;
  *(float *)(param_1 + 0x18 + param_2 * 4) = t2;
  *(float *)(param_1 + param_3 * 4) = t3;
  *(float *)(param_1 + 0xc + param_3 * 4) = t4;
  *(float *)(param_1 + 0x18 + param_3 * 4) = t5;
}
