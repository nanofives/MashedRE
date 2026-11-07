// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 100% by whole-symbol bytes (104 B, 0 differing; Ghidra size 46 stops before the 7-entry jump table and case bodies, so match.py instruction score reads 60%). Vtable +0x20 call is __stdcall(obj, &param_1) (callee pops). Class: none. */
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
typedef int code();
typedef void (__stdcall *FUN_005baf90_vfn)(int *, int *);
// MATCH: 0x005baf90 FUN_005baf90
undefined4 FUN_005baf90(int param_1)

{
  (*(FUN_005baf90_vfn *)(**(int **)(param_1 + 0x7c) + 0x20))(*(int **)(param_1 + 0x7c), &param_1);
  switch ((byte)param_1) {
  case 2:
    return 2;
  case 1:
    return 3;
  case 5:
    return 4;
  case 6:
    return 5;
  case 7:
    return 6;
  }
  return 1;
}
