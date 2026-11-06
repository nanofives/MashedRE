// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046d2e0 (helper h4): 100% (regs 100%). Two separate early returns (combined && test put return 0 last, 75%). Stride-0xc4 record array, index a*0x11+b. */
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
typedef struct { int v; char pad[0xc0]; } S0046d2e0;
MGLOBALA(S0046d2e0, DAT_00881790, 272, 0x00881790);
// MATCH: 0x0046d2e0 FUN_0046d2e0
int FUN_0046d2e0(int param_1,int param_2,undefined4 *param_3)
{
  if (param_1 >= 0x10) return 0;
  if (param_2 >= 4) return 0;
  *param_3 = DAT_00881790[param_1 * 0x11 + param_2].v;
  return 1;
}
