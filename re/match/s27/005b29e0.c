// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 73.3% (regs 73.7%). Added typedef long *LPLONG (raw did not compile), permuter swap 66.7->73.3. Residual: range test operand order (cmp edx,eax vs cmp eax,edx), index temp (lea eax+1 vs lea ecx), bool loop flag in ecx. Class: b. */
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
typedef void *HANDLE;
typedef int BOOL;
typedef unsigned long DWORD;
typedef long LONG;
typedef long *LPLONG;
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
__declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE p0, LONG p1, LPLONG p2);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE p0, DWORD p1);
/* converter noise: extern int LPLONG; dropped (no address) */
// MATCH: 0x005b29e0 FUN_005b29e0
void FUN_005b29e0(int param_1,uint param_2)

{
  byte bVar1;
  uint uVar2;
  bool bVar3;
  uint uVar4;
  int iVar5;
  
  WaitForSingleObject(*(HANDLE *)(param_1 + 0x18c),0xffffffff);
  uVar4 = *(uint *)(*(int *)(param_1 + 0x118) + 0x20);
  uVar2 = *(uint *)(param_1 + 0x144);
  if ((param_2 < uVar2) || (*(int *)(param_1 + 0x160) + uVar2 <= param_2)) {
    *(undefined4 *)
     (*(int *)(param_1 + 0x16c) + 0x10 + ((param_2 - *(int *)(param_1 + 0x168)) / uVar4) * 0x14) = 1
    ;
    ReleaseSemaphore(*(HANDLE *)(param_1 + 0x18c),1,(LPLONG)0x0);
    return;
  }
  uVar4 = (param_2 - uVar2) / uVar4;
  *(undefined4 *)(*(int *)(param_1 + 0x148) + 0x10 + uVar4 * 0x14) = 1;
  iVar5 = uVar4 + 1;
  if (iVar5 == *(int *)(param_1 + 0x164)) {
    iVar5 = 0;
  }
  if (iVar5 == *(int *)(param_1 + 0x158)) {
    *(undefined4 *)(param_1 + 0x154) = *(undefined4 *)(param_1 + 0x15c);
    *(undefined4 *)(param_1 + 0x158) = *(undefined4 *)(param_1 + 0x154);
    ReleaseSemaphore(*(HANDLE *)(param_1 + 0x18c),1,(LPLONG)0x0);
    return;
  }
  iVar5 = *(int *)(param_1 + 0x154);
  bVar3 = true;
  bVar1 = *(byte *)(*(int *)(param_1 + 0x148) + 0x10 + iVar5 * 0x14);
  while ((bVar1 & 1) != 0) {
    iVar5 = iVar5 + 1;
    if (iVar5 == *(int *)(param_1 + 0x164)) {
      iVar5 = 0;
      if (!bVar3) break;
      bVar3 = false;
    }
    bVar1 = *(byte *)(*(int *)(param_1 + 0x148) + 0x10 + iVar5 * 0x14);
  }
  *(int *)(param_1 + 0x154) = iVar5;
  ReleaseSemaphore(*(HANDLE *)(param_1 + 0x18c),1,(LPLONG)0x0);
  return;
}
