// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 53.8% (regs 53.8%, permuter best (150 s). Residual not classified beyond register/schedule differences. Class: b. */
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
int FUN_005b2820(int p0, int * p1);
void FUN_005b29e0(int p0, uint p1);
void FUN_005b81d0(int p0);
__declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE p0, LONG p1, LPLONG p2);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE p0, DWORD p1);
void thunk_FUN_005ab620(int p0);
/* converter noise: extern int LPLONG; dropped (no address) */
// MATCH: 0x005b3300 FUN_005b3300
void FUN_005b3300(int param_1)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  iVar2 = param_1;
  if ((char)((uint)*(undefined4 *)(param_1 + 0x188) >> 8) < '\0') {
    bVar1 = *(byte *)(iVar3 + 0x10 + *(int *)(iVar2 + 0x150) * 0x14);
    FUN_005b81d0(param_1 + 0x8c);
    thunk_FUN_005ab620(iVar2 + 0x28);
    *(uint *)(iVar2 + 0x188) = *(uint *)(iVar2 + 0x188) & 0xffffffd7;
    *(uint *)(iVar2 + 0x188) = *(uint *)(iVar2 + 0x188) | 0x2000;
    WaitForSingleObject(*(HANDLE *)(iVar2 + 0x18c),0xffffffff);
    iVar3 = *(int *)(iVar2 + 0x148);
    while ((bVar1 & 8) != 0) {
      *(undefined4 *)(iVar3 + 0x10 + *(int *)(iVar2 + 0x150) * 0x14) = 1;
      *(int *)(iVar2 + 0x150) = *(int *)(iVar2 + 0x150) + 1;
      if (*(int *)(iVar2 + 0x150) == *(int *)(iVar2 + 0x164)) {
        *(undefined4 *)(iVar2 + 0x150) = 0;
      }
      iVar3 = *(int *)(iVar2 + 0x148);
      bVar1 = *(byte *)(iVar3 + 0x10 + *(int *)(iVar2 + 0x150) * 0x14);
    }
    ReleaseSemaphore(*(HANDLE *)(iVar2 + 0x18c),1,(LPLONG)0x0);
    while (iVar3 = FUN_005b2820(iVar2,&param_1), iVar3 != 0) {
      FUN_005b29e0(iVar2,iVar3);
    }
    if (((*(uint *)(iVar2 + 0x188) & 0x10000) != 0) && (uVar4 = 0, *(int *)(iVar2 + 0x164) != 0)) {
      iVar3 = 0;
      do {
        uVar4 = uVar4 + 1;
        *(undefined4 *)(*(int *)(iVar2 + 0x148) + 0x10 + iVar3) = 1;
        iVar3 = iVar3 + 0x14;
      } while (uVar4 < *(uint *)(iVar2 + 0x164));
    }
    if (*(int *)(iVar2 + 0x16c) != 0) {
      *(undefined4 *)(*(int *)(iVar2 + 0x16c) + 0xc + *(int *)(iVar2 + 0x174) * 0x14) = 0;
    }
    *(undefined4 *)(iVar2 + 0x154) = 0;
    *(undefined4 *)(iVar2 + 0x158) = 0;
    *(undefined4 *)(iVar2 + 0x150) = 0;
    *(undefined4 *)(iVar2 + 0x15c) = 0;
    *(undefined4 *)(iVar2 + 0x14c) = 0;
    *(uint *)(iVar2 + 0x188) = *(uint *)(iVar2 + 0x188) & 0xffffdfff;
  }
  return;
}
