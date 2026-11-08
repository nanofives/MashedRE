// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ae650: 74.1% (regs 76.9%, ops 90.7%), 106 vs 108 insns. Hand rewrite with typed uint pointers, memset intrinsic for the cleared cell body (rep stosd/stosb), uint param_4 > 0 for jbe, branch order DAT_007ddab0 != 0 first. Residual: original pushes ebp/esi in the prologue before the first branch (early return pops them); the DDK emits those two pushes later, after the alloc block, and orders a few arg loads differently. Same prologue-push placement and deferred add-esp behaviour as 005ae250/005af690/005b0df0, so likely compiler-version (a). cxx pending. */
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
typedef unsigned int uint;
typedef unsigned int size_t;
MGLOBAL(int, DAT_007ddab0, 0x007ddab0);
MGLOBAL(uint *, DAT_007dda80, 0x007dda80);
void FUN_005ae780(uint * p0);
uint * FUN_005ae800(int p0, int p1);
uint * FUN_005aea00(uint p0, int p1);
void * memset(void *, int, size_t);
#pragma intrinsic(memset)
// MATCH: 0x005ae650 FUN_005ae650
uint * FUN_005ae650(int param_1,uint param_2,uint param_3,uint param_4,uint *param_5,int param_6)
{
  uint *puVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;

  if (param_3 == 0) {
    param_3 = 1;
  }
  if (param_5 == 0) {
    if (DAT_007ddab0 != 0) {
      param_5 = FUN_005ae800(DAT_007ddab0,0x30800);
    }
    else {
      param_5 = FUN_005aea00(0x24,param_6);
    }
    if (param_5 == 0) {
      return 0;
    }
    param_5[6] = 2;
  }
  else {
    param_5[6] = 3;
  }
  uVar2 = param_1 + -1 + param_3 & ~(param_3 - 1);
  puVar1 = param_5 + 4;
  uVar5 = param_2 + 7 >> 3;
  *param_5 = uVar2;
  param_5[1] = param_2;
  param_5[3] = param_3;
  param_5[2] = uVar5;
  *puVar1 = (uint)puVar1;
  param_5[5] = (uint)puVar1;
  if (param_4 > 0) {
    do {
      puVar3 = FUN_005aea00(uVar2 * param_2 + uVar5 + 8 + param_3,param_6);
      if (puVar3 == 0) {
        FUN_005ae780(param_5);
        return 0;
      }
      puVar3[1] = 0;
      *puVar3 = 0;
      uVar4 = *puVar1;
      puVar3[1] = (uint)puVar1;
      *puVar3 = uVar4;
      *(uint **)(*puVar1 + 4) = puVar3;
      *puVar1 = (uint)puVar3;
      memset(puVar3 + 2,0,uVar5);
      param_4 = param_4 - 1;
    } while (param_4 != 0);
  }
  puVar1 = param_5 + 7;
  param_5[8] = (uint)&DAT_007dda80;
  *puVar1 = (uint)DAT_007dda80;
  *(uint **)((uint)DAT_007dda80 + 4) = puVar1;
  DAT_007dda80 = puVar1;
  return param_5;
}
