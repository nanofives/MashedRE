// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a6d90 (h2): best 100.0% (regs 100.0%), class -. struct table entry, ushort arg copied to a local before the call. relocheck. */
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
typedef unsigned short ushort;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef struct { int (*fn)(undefined4, uint, undefined4); undefined2 arg; undefined2 pad; } S_005a6d90_ent;
typedef struct { undefined4 a; struct { undefined4 x0; undefined4 x4; S_005a6d90_ent *tab; } *b; } S_005a6d90_obj;
// MATCH: 0x005a6d90 FUN_005a6d90
uint FUN_005a6d90(undefined4 *param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  S_005a6d90_ent *e;
  int iVar1;
  uint a;

  e = ((S_005a6d90_obj *)param_1)->b->tab + param_2;
  a = e->arg;
  iVar1 = e->fn(((S_005a6d90_obj *)param_1)->a,a,param_4);
  return (iVar1 != 0) ? (uint)param_1 : 0;
}
