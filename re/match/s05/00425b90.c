// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425b90 (41 B), h9 by hand: 100.0% (regs 100.0%). The two locals are one struct {count, obj} passed
   by address; the callback is a real function (Ghidra typed FUN_004516d0 as a byte global). Callee renamed
   RpClumpForAllAtomics -> FUN_004e66d0. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef int (*AtomicCB)(void *atomic, void *data);
int FUN_004516d0(void *atomic, void *data);
int FUN_004e66d0(int clump, AtomicCB cb, void *data);
// MATCH: 0x00425b90 FUN_00425b90
void FUN_00425b90(int *param_1)
{
  struct { int count; int *obj; } ctx;
  ctx.count = 0;
  ctx.obj = param_1;
  FUN_004e66d0(*param_1, FUN_004516d0, &ctx);
}