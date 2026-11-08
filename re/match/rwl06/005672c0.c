// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h3 0x005672c0: best 18.8% (regs 43.8%, ops 43.8%), class (b). sum of separately 16-aligned terms; the compiler reschedules the adds and shares edi/esi differently; single expression, temp, bitfield and N=(n<<2)>>2 forms tried. */
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
typedef unsigned int uint;
// MATCH: 0x005672c0 FUN_005672c0
uint FUN_005672c0(int param_1)

{
  uint M;
  uint N;

  M = *(uint *)(param_1 + 0x28) << 2;
  N = M >> 2;
  return ((((((M / 3) * 0x40 + 0xfU) & 0xfffffff0) + ((*(int *)(param_1 + 0x14) * 4 + 0xfU) & 0xfffffff0) * 2 +
            ((N * 8 + 0xfU) & 0xfffffff0) + ((N * 2 * 4 + 0xfU) & 0xfffffff0)) * 2 +
           (((N * 0x10 + 0xfU) & 0xfffffff0) + 3) * 5 +
           ((*(int *)(param_1 + 0x10) * 4 + 0xfU) & 0xfffffff0)) +
          ((N * 2 * 3 * 0x20 + 0xfU) & 0xfffffff0)) & 0xfffffff0;
}
