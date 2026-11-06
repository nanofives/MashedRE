// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x0045a0d0 (h9, by hand from the raw converter): 100% (regs 100%). Raw 70.0%: Ghidra's
   `-1 < param_1` compiles to cmp -1/jle, the original's test/jl is `param_1 >= 0`. Residual: none. */
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
MGLOBAL(undefined4, DAT_0068ba04, 0x0068ba04);
// MATCH: 0x0045a0d0 FUN_0045a0d0
undefined4 FUN_0045a0d0(int param_1)

{
  if ((param_1 >= 0) && (param_1 < 4)) {
    return (&DAT_0068ba04)[param_1 * 0x16];
  }
  return 0xffffffff;
}
