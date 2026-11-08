// FLAGS: /O2
/* MATCH-S28 h1 0x005c6b60 : 54.8% (regs 77). Signed divide-by-64 form (cdq, and 0x3f, add, sar 6) and a ternary tail found; original keeps param_1 in ecx loaded first and pops esi before the jne, ours puts param_1 in esi. Permuter best 59.4 (not kept). Residual class (b), register allocation. */

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

// MATCH: 0x005c6b60 FUN_005c6b60
void FUN_005c6b60(int *param_1,int param_2,int param_3)
{
  int a;
  int b;

  a = (param_2 * 0x100 - param_1[1]) / 0x40;
  b = (param_3 * 0x100 - param_1[2]) / 0x40;
  param_1[3] = a;
  param_1[4] = b;
  *param_1 = (a != 0 || b != 0) ? 0x40 : 0;
}
