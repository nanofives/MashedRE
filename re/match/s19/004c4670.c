// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c4670 (h5): 100% (regs 100%). thunk: `jmp FUN_004c4680` as a void tail call with the same 2 stack args.
   FLAGS: /G5 added (h5 finding, 2026-11-06): the RW core in this slice is a Pentium (/G3-/G5) build. Under the
   game flags /O2 /GS /Ob1 alone (= /G6 scheduling and no byte-register ALU ops) this file scores  100% as well. */
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
void FUN_004c4680(float *param_1, float *param_2);

// MATCH: 0x004c4670 FUN_004c4670
void FUN_004c4670(float *param_1, float *param_2)
{
  FUN_004c4680(param_1, param_2);
}
