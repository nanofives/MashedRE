// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-C) 0x004b3f20: the RpClumpForAllAtomics callback of 0x004b3fc0. Context {out, &count}:
   when out is non-zero, out[*count] = atomic; then ++*count; returns the atomic. Ghidra has no function here
   (re/console/pc_funcs.csv carries its extent, 34 B). */
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
// MATCH: 0x004b3f20 FUN_004b3f20
void *FUN_004b3f20(void *atomic, Ctx004b3fc0 *ctx)
{
  void **out = ctx->out;
  int *count = ctx->count;
  if (out != 0) {
    out[*count] = atomic;
    ++*count;
    return atomic;
  }
  ++*count;
  return atomic;
}
