// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ae250: 64.9% (regs 73.7%, ops 93.0%), 56 vs 57 insns. Hand rewrite: uint count with while(count>0) (jbe/ja), FUN_005ae800 arg order (&DAT_007dda50, 0x30800), typed ints. Residual: original pushes edi in the prologue before the first branch (early return pops it) and orders the arg loads/stores differently (ecx/eax arg loads before the imul, edi load after); the DDK places push edi after the alloc branch. The permuter's 80.7% variant read an uninitialised local, so it was discarded. Class (b). cxx pending. */
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
typedef unsigned char undefined1;
typedef unsigned int uint;
MGLOBAL(int *, DAT_007dda74, 0x007dda74);
MGLOBAL(undefined1, DAT_007dda50, 0x007dda50);
int FUN_005ae300(int *p0);
void FUN_005ae330(int *p0);
int * FUN_005ae800(undefined1 * p0, int p1);
// MATCH: 0x005ae250 FUN_005ae250
int * FUN_005ae250(int param_1,int param_2,int param_3,uint param_4,int *param_5)
{
  int *puVar1;
  int *piVar2;

  if (param_5 == 0) {
    param_5 = FUN_005ae800(&DAT_007dda50,0x30800);
    if (param_5 == 0) {
      return 0;
    }
    *param_5 = 0;
  }
  else {
    *param_5 = 1;
  }
  param_5[2] = param_3;
  puVar1 = param_5 + 3;
  param_5[4] = (int)puVar1;
  param_5[1] = (param_1 + 0xc) * param_2 + 0x1c + param_3;
  *puVar1 = (int)puVar1;
  piVar2 = param_5 + 5;
  param_5[6] = (int)&DAT_007dda74;
  *piVar2 = (int)DAT_007dda74;
  *(int **)((int)DAT_007dda74 + 4) = piVar2;
  DAT_007dda74 = piVar2;
  while (param_4 > 0) {
    param_4 = param_4 - 1;
    if (FUN_005ae300(param_5) == 0) {
      FUN_005ae330(param_5);
      return 0;
    }
  }
  return param_5;
}
