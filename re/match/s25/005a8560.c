// FLAGS: /O2 /Ob1 /G5
/* MATCH-NEARMISS-2-pilot 2026-11-09: 100% BYTE-IDENTICAL by hand, same rewrite as 0x005a8390: RW list-link insert order (link->next = list.next through param_1+0x14, link->prev = &list through param_1+0x18, list.next->prev = link, list.next = link) with DAT_007dcad8 typed as a {next, prev} link. The older note below describes the pre-pilot residual. */
/* best 98.4% (regs 98.4%, ops 100.0%) via match_permute (store of &DAT_007dcad8 moved ahead of the piVar2 load). Residual: position of the
   list-head load (original loads it before the +0x18 store). Same list-link residual as 0x005a8390. Class (b). */
/* MATCH-S25 0x005a8560: hand-matched (h1). */
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

MGLOBAL(void *, DAT_007dcae0, 0x007dcae0);
typedef struct LL { struct LL *next; struct LL *prev; } LL;
MGLOBAL(LL, DAT_007dcad8, 0x007dcad8);
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void *hHandle, unsigned long dwMilliseconds);
__declspec(dllimport) int __stdcall ReleaseSemaphore(void *hSemaphore, long lReleaseCount, long *lpPreviousCount);
undefined4 FUN_005af010(int p0);
int *FUN_005af180(int p0, void *p1, undefined4 p2);
void FUN_005af200(int p0, void *p1, undefined4 p2);
int *FUN_005aab00(int *p0, int p1);
int *FUN_005aab70(int *p0, int p1);
void FUN_005a8640(void);
// MATCH: 0x005a8560 FUN_005a8560
int FUN_005a8560(int param_1)
{
  int *piVar1;
  int iVar3;
  FUN_005af010(param_1 + 0xec);
  if (((*(byte *)(param_1 + 0xd9c) & 1) != 0) && (*(int *)(param_1 + 0xf4) >= 5)) {
    iVar3 = (int)FUN_005af180(param_1 + 0xec,(void *)FUN_005a8640,param_1);
    if (iVar3 != 0) {
      FUN_005af200(iVar3,(void *)FUN_005a8640,param_1);
    }
    FUN_005aab00((int *)param_1,1);
    FUN_005aab70((int *)param_1,1);
    WaitForSingleObject(DAT_007dcae0,0xffffffff);
    piVar1 = (int *)(param_1 + 0x14);
    if (*(int *)(param_1 + 0x14) != 0) {
      **(int **)(param_1 + 0x18) = *(int *)(param_1 + 0x14);
      *(undefined4 *)(*piVar1 + 4) = *(undefined4 *)(param_1 + 0x18);
      *(undefined4 *)(param_1 + 0x18) = 0;
      *piVar1 = 0;
    }
    *(LL **)(param_1 + 0x14) = DAT_007dcad8.next;
    *(LL **)(param_1 + 0x18) = &DAT_007dcad8;
    DAT_007dcad8.next->prev = (LL *)piVar1;
    DAT_007dcad8.next = (LL *)piVar1;
    *(uint *)(param_1 + 0xd9c) = *(uint *)(param_1 + 0xd9c) & 0xfffffffe;
    ReleaseSemaphore(DAT_007dcae0,1,(long *)0x0);
  }
  return param_1;
}
