// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004957a0 (h2): 100% (regs 100%). Hand-written: two separate early-return ifs (a combined && put the return-0 block at the end), 8-byte pair struct copy for the two 0x89-stride tables. */
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
typedef struct { undefined4 a, b; } pair8;
MGLOBAL(int, DAT_00772fac, 0x00772fac);
MGLOBAL(int, DAT_00771e80, 0x00771e80);
MGLOBALA(pair8, DAT_00772150, 0x89, 0x00772150);
MGLOBALA(pair8, DAT_007721d0, 0x89, 0x007721d0);
MGLOBALA(undefined4, DAT_00772290, 0x112, 0x00772290);
// MATCH: 0x004957a0 FUN_004957a0
int FUN_004957a0(int param_1, int param_2, pair8 *param_3, pair8 *param_4, undefined4 *param_5)
{
  if (param_1 >= DAT_00772fac) {
    return 0;
  }
  if (param_2 >= DAT_00771e80) {
    return 0;
  }
  if (param_3 != 0) {
    *param_3 = DAT_00772150[param_1 * 0x89 + param_2];
  }
  if (param_4 != 0) {
    *param_4 = DAT_007721d0[param_1 * 0x89 + param_2];
  }
  if (param_5 != 0) {
    *param_5 = DAT_00772290[param_1 * 0x112 + param_2];
  }
  return 1;
}
