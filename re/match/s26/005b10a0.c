// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005b10a0 (h3): 42.9% exact, regs 78.6%. Residual class (b) original loads the word at [entry+4] into ax first and param_1 last; struct/temp forms tried, same 42.9%. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef int code();
// MATCH: 0x005b10a0 FUN_005b10a0
typedef struct E10a0 { void (*fn)(undefined4, uint, undefined4); unsigned short w; } E10a0;
void FUN_005b10a0(undefined4 param_1,int param_2,int param_3,undefined4 param_4)
{
  E10a0 *e = (E10a0 *)(*(int *)(param_2 + 4)) + param_3;
  uint w = e->w;
  e->fn(param_1, w, param_4);
}
