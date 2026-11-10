// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-B) 0x004b3fc0: counts (and, when the out array is non-zero, collects) the atomics of
   a clump through RpClumpForAllAtomics 0x004e66d0 with callback 0x004b3f20. Context record {out array, &count}
   on the stack; returns the count. */
/* ---- MATCH-S29 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct Ctx004b3fc0 { void **out; int *count; } Ctx004b3fc0;
void *FUN_004b3f20(void *atomic, Ctx004b3fc0 *ctx);
void *FUN_004e66d0(void *clump, void *(*cb)(void *, Ctx004b3fc0 *), void *data);
// MATCH: 0x004b3fc0 FUN_004b3fc0
int FUN_004b3fc0(void *param_1, void **param_2)
{
  struct { int count; Ctx004b3fc0 ctx; } s;
  s.ctx.out = param_2;
  s.count = 0;
  s.ctx.count = &s.count;
  FUN_004e66d0(param_1, FUN_004b3f20, &s.ctx);
  return s.count;
}
