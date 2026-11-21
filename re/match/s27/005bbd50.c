// FLAGS: /O2 /GS /Ob1
/* MATCH-S27 h1: 45.7% (regs 48.6%, permuter best (150 s). Residual not classified beyond register/schedule differences. Class: b. */
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
#ifdef __cplusplus
#else
#endif
/* PORT-TOP-AUDIO-5BA1D0-STOPS: body rewritten from the listing 0x005bbd50..0x005bbdaf. The Ghidra form cleared the
   descriptor from its second dword and returned the descriptor's address; the original returns the buffer the method at
   +0xc wrote to [esp+4] when it returns >= 0, else 0 (0x005bbd9d..0x005bbdaa). The method is __stdcall (no add esp after
   0x005bbd9a). Descriptor at [esp+8]: dword 0 = 0x24 (0x005bbd92), dword 1 = (param_2 != 0 ? 0x10 : 0) | 1 (0x005bbd61..0x005bbd76). */
typedef int (__stdcall *PFN4_005bbd50)(int *, uint *, int **, int);
// MATCH: 0x005bbd50 FUN_005bbd50
int *FUN_005bbd50(int *param_1,int param_2)

{
  int iVar1;
  int *res;
  uint desc [9];

  for (iVar1 = 0; iVar1 < 9; iVar1 = iVar1 + 1) {
    desc[iVar1] = 0;
  }
  res = (int *)0x0;
  desc[2] = 0;
  desc[4] = 0;
  desc[1] = (uint)(-(param_2 != 0) & 0x10) | 1;
  desc[0] = 0x24;
  iVar1 = (*(PFN4_005bbd50 *)(*param_1 + 0xc))(param_1,desc,&res,0);
  return (int *)((uint)res & ((iVar1 < 0) - 1));
}
