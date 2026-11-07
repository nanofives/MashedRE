// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b2080: 82.6% (regs 82.6%, ops 98.6%). volatile on every param_1 field access (original reloads each field, no CSE; plausible since the struct is semaphore-guarded shared state): 58 -> 82.6%. Residual: compare operand order of the two shorts at +0x92/+0x94 and push edi placement; permuter no gain. Class: a.
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
typedef unsigned char byte;
void FUN_005b2190(int p0);
int FUN_005b2220(int p0);
undefined4 FUN_005b8130(int * p0, undefined4 * p1);
// MATCH: 0x005b2080 FUN_005b2080
void FUN_005b2080(int param_1)

{
  int iVar1;
  int iVar2;
  
  if (((*(volatile short *)(param_1 + 0x92) != *(volatile short *)(param_1 + 0x94)) ||
      (*(volatile short *)(param_1 + 0x96) != 0)) && ((*(volatile uint *)(param_1 + 0x188) & 0x22ff8) == 0)) {
    iVar2 = FUN_005b8130((int *)(param_1 + 0x8c),(undefined4 *)(param_1 + 0xd8));
    if (iVar2 != 0) {
      FUN_005b2190(param_1);
    }
  }
  iVar2 = *(volatile int *)(param_1 + 0x148);
  if ((((*(byte *)(iVar2 + 0x10 + *(volatile int *)(param_1 + 0x15c) * 0x14) & 1) == 0) &&
      (*(volatile int *)(param_1 + 0x168) != 0)) &&
     (iVar1 = *(volatile int *)(param_1 + 0x16c),
     (*(byte *)(iVar1 + 0x10 + *(volatile int *)(param_1 + 0x174) * 0x14) & 6) == 0)) {
    if (((*(int *)(iVar2 + 0xc + *(volatile int *)(param_1 + 0x154) * 0x14) == 0) &&
        (*(int *)(iVar1 + 0xc + *(volatile int *)(param_1 + 0x174) * 0x14) == 0)) &&
       (((*(byte *)(iVar2 + 0x10 + *(volatile int *)(param_1 + 0x154) * 0x14) & 0x10) == 0 ||
        (*(volatile int *)(param_1 + 0xec) == -1)))) {
      if (*(int *)(iVar1 + 0x10 + *(volatile int *)(param_1 + 0x178) * 0x14) == 1) {
        *(volatile uint *)(param_1 + 0x188) = *(volatile uint *)(param_1 + 0x188) & 0xfffeffff;
        return;
      }
    }
    else {
      *(volatile uint *)(param_1 + 0x188) = *(volatile uint *)(param_1 + 0x188) | 0x10000;
      FUN_005b2220(param_1);
    }
  }
  return;
}
