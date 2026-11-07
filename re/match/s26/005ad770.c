// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ad770 (h3): 69.9% exact, regs 84.1%. Residual class (b) memcpy intrinsic for the three copy loops (26.5 -> 59.3%), permuter reordered independent stores (69.9%). */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(undefined1, DAT_007dd6c4, 0x007dd6c4);
MGLOBAL(undefined1, DAT_009146f0, 0x009146f0);
uint FUN_005a61b0(undefined4 p0);
undefined4 FUN_005ad6f0(undefined4 p0);
void FUN_005ad8b0(int p0);
void FUN_005ad8d0(int p0);
void FUN_005ae0b0(int p0);
uint FUN_005ae800(void * p0, undefined4 p1);
void FUN_005b35a0(void * p0, int p1);
#ifdef __cplusplus
extern "C" void *memcpy(void *, const void *, unsigned int);
#else
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
#endif
// MATCH: 0x005ad770 FUN_005ad770
int FUN_005ad770(undefined4 param_1,undefined4 *param_2,undefined4 *param_3,int param_4,int param_5)
{
  uint uVar1;
  int iVar2;

  uVar1 = FUN_005a61b0(param_1);
  if (param_4 == 0) {
    param_4 = FUN_005ae800(&DAT_007dd6c4,0x30803);
    *(undefined4 *)(param_4 + 0x1c) = 0;
  }
  else {
    *(undefined4 *)(param_4 + 0x1c) = 1;
  }
  *(undefined4 *)(param_4 + 0x10) = 0;
  *(undefined4 *)(param_4 + 0xc) = 0;
  if (!(param_5 == 0)) {
    *(uint *)(param_4 + 0x1c) = *(uint *)(param_4 + 0x1c) | 2;
    if (param_2 != 0) {
      *(int *)(param_4 + 0xc) = param_5;
      if (param_3 != 0) {
        iVar2 = FUN_005a61b0(param_1);
        *(int *)(param_4 + 0x10) = iVar2 + param_5;
      }
      memcpy(*(void **)(param_4 + 0xc), param_2, uVar1);
    }
    else if (param_3 != 0) {
      *(int *)(param_4 + 0x10) = param_5;
    }
  } else {
    if (param_2 != 0) {
      iVar2 = FUN_005ad6f0(param_1);
      *(int *)(param_4 + 0xc) = iVar2;
      if (iVar2 == 0) {
        FUN_005ad8d0(param_4);
      }
    }
    if (param_3 != 0) {
      iVar2 = FUN_005ad6f0(param_1);
      *(int *)(param_4 + 0x10) = iVar2;
      if (iVar2 == 0) {
        FUN_005ad8b0(param_4);
        FUN_005ad8d0(param_4);
      }
    }
    if (param_2 != 0) {
      memcpy(*(void **)(param_4 + 0xc), param_2, uVar1);
    }
  }
  if (param_3 != 0) {
    memcpy(*(void **)(param_4 + 0x10), param_3, uVar1);
  }
  FUN_005ae0b0(param_4);
  *(undefined4 *)(param_4 + 0x18) = 0;
  FUN_005b35a0(&DAT_009146f0,param_4);
  *(undefined4 *)(param_4 + 0x14) = param_1;
  return param_4;
}
