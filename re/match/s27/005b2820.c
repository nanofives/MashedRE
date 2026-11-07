// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b2820: 42.6% (regs 32%). Branch order swapped to the original (0x10000-set block first), LPLONG noise removed, every param_1 field access volatile (original reloads each field; volatile gets 21% -> 42%). Residual: flag reload kept in edi vs eax, register allocation. Class: b.
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
typedef void *HANDLE;
typedef int BOOL;
typedef unsigned long DWORD;
typedef long LONG;
__declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE p0, LONG p1, long *p2);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE p0, DWORD p1);
// MATCH: 0x005b2820 FUN_005b2820
int FUN_005b2820(int param_1,int *param_2)
{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;

  WaitForSingleObject(*(HANDLE volatile *)(param_1 + 0x18c),0xffffffff);
  iVar3 = *(int *)(*(volatile int *)(param_1 + 0x118) + 0x20);
  if ((*(volatile uint *)(param_1 + 0x188) & 0x10000) != 0) {
    uVar1 = *(uint *)(*(volatile int *)(param_1 + 0x16c) + 0x10 + *(volatile int *)(param_1 + 0x178) * 0x14);
    if ((*(volatile uint *)(param_1 + 0x188) & 0x2000) == 0) {
      if (((uVar1 & 2) == 0) || ((uVar1 & 4) != 0)) {
        goto fail;
      }
    }
    else if ((uVar1 & 2) == 0) {
      ReleaseSemaphore(*(HANDLE volatile *)(param_1 + 0x18c),1,(long *)0);
      return 0;
    }
    iVar4 = *(volatile int *)(param_1 + 0x178) * iVar3 + *(volatile int *)(param_1 + 0x168);
    *param_2 = *(volatile int *)(param_1 + 0x16c) + *(volatile int *)(param_1 + 0x178) * 0x14;
    *(undefined4 *)(*(volatile int *)(param_1 + 0x16c) + 0x10 + *(volatile int *)(param_1 + 0x178) * 0x14) = 4;
    iVar3 = *(volatile int *)(param_1 + 0x178) + 1;
    if (iVar3 == *(volatile int *)(param_1 + 0x17c)) {
      iVar3 = 0;
    }
    *(volatile int *)(param_1 + 0x178) = iVar3;
  }
  else {
    uVar1 = *(uint *)(*(volatile int *)(param_1 + 0x148) + 0x10 + *(volatile int *)(param_1 + 0x158) * 0x14);
    if ((*(volatile uint *)(param_1 + 0x188) & 0x2000) == 0) {
      if (((uVar1 & 2) == 0) || ((uVar1 & 4) != 0)) {
fail:
        ReleaseSemaphore(*(HANDLE volatile *)(param_1 + 0x18c),1,(long *)0);
        return 0;
      }
    }
    else if ((uVar1 & 2) == 0) goto fail;
    iVar4 = *(volatile int *)(param_1 + 0x158) * iVar3 + *(volatile int *)(param_1 + 0x144);
    *param_2 = *(volatile int *)(param_1 + 0x148) + *(volatile int *)(param_1 + 0x158) * 0x14;
    *(undefined4 *)(*(volatile int *)(param_1 + 0x148) + 0x10 + *(volatile int *)(param_1 + 0x158) * 0x14) = 4;
    iVar2 = *(volatile int *)(param_1 + 0x158);
    iVar3 = iVar2 + 1;
    if (iVar3 == *(volatile int *)(param_1 + 0x164)) {
      iVar3 = 0;
    }
    *(volatile int *)(param_1 + 0x158) = iVar3;
    if (*(volatile int *)(param_1 + 0x15c) == iVar2) {
      *(volatile int *)(param_1 + 0x15c) = iVar3;
    }
  }
  *(volatile int *)(param_1 + 0xfc) = *(volatile int *)(param_1 + 0xfc) + -1;
  ReleaseSemaphore(*(HANDLE volatile *)(param_1 + 0x18c),1,(long *)0);
  return iVar4;
}
