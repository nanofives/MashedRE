// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f07d0 (h1): 100% under /O2 /Ob1 /G5. Plugin-global setter on RwGlobals (DAT_007d3ff8) + plugin offset
   DAT_00911ae0: NULL selects the default slot. The raw converter's if/else with two stores scored 50% (the
   compiler hoisted one global load above the test); the form that matches assigns the default into the argument,
   stores once and returns the argument (eax holds the new value on both paths in the original). */
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
typedef struct { char pad[0x3c]; void *cur; char pad2[0x14]; void *def; } Glob_004f07d0;
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
// MATCH: 0x004f07d0 FUN_004f07d0
void *FUN_004f07d0(void *param_1)
{
  if (!param_1) {
    param_1 = ((Glob_004f07d0 *)(DAT_007d3ff8 + DAT_00911ae0))->def;
  }
  ((Glob_004f07d0 *)(DAT_007d3ff8 + DAT_00911ae0))->cur = param_1;
  return param_1;
}
