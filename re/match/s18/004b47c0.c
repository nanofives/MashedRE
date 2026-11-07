// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 h2 0x004b47c0 (30 B): g2s body; 100%. Fix: param_2 is a float passed through (Ghidra's callee
   prototype FUN_0052e0e0(int *, float) made the raw output convert int->float). Casts added for C++. */
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
typedef unsigned int uint;
undefined4 FUN_0052e0e0(int * p0, float p1);
undefined4 FUN_00533ec0(uint * p0);
// MATCH: 0x004b47c0 FUN_004b47c0
void FUN_004b47c0(int param_1,float param_2)

{
  FUN_0052e0e0(*(int **)(param_1 + 0x20),param_2);
  FUN_00533ec0((uint *)param_1);
  return;
}
