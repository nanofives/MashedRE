// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005ab620: by hand. 49.3% (regs 88.0%, ops 96.0%, 75/75 ins). Fields via piVar1 = param_1 + 1 (esi base), flag as int set to 1 inside the branch (not a sete), DAT_007dd60c as a 4-byte store. Residual (b): original keeps the two imports in edi (WaitForSingleObject then ReleaseSemaphore reloaded) and the semaphore else-branch placement; ours differs in prologue order and block layout. Permuter not run (time). */
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
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void * hHandle, unsigned long dwMilliseconds);
__declspec(dllimport) int __stdcall ReleaseSemaphore(void * hSemaphore, long lReleaseCount, long * lpPreviousCount);
MGLOBAL(void *, DAT_007dd618, 0x007dd618);
MGLOBAL(void *, DAT_007dd620, 0x007dd620);
MGLOBAL(int *, DAT_007dd62c, 0x007dd62c);
MGLOBAL(int *, DAT_007dd608, 0x007dd608);
MGLOBAL(undefined4, DAT_007dd60c, 0x007dd60c);
MGLOBAL(undefined4, DAT_007dd61c, 0x007dd61c);
int FUN_005aeed0(undefined4 * p0);
void FUN_00550bd0(undefined4 p0);
void FUN_005ab710(undefined4 p0, int p1, int p2, int p3, int p4);
// MATCH: 0x005ab620 FUN_005ab620
void FUN_005ab620(int param_1)
{
  int *piVar1;
  int iVar2;
  int bVar3;
  bVar3 = 0;
  piVar1 = (int *)(param_1 + 4);
  WaitForSingleObject(DAT_007dd618,0xffffffff);
  WaitForSingleObject(DAT_007dd620,0xffffffff);
  iVar2 = FUN_005aeed0(&DAT_007dd61c);
  if (iVar2 == 0) {
    if (DAT_007dd62c == piVar1) {
      bVar3 = 1;
      FUN_00550bd0(piVar1[7]);
      piVar1[2] = 3;
      piVar1[5] = 0;
    }
    if (DAT_007dd608 == piVar1) {
      DAT_007dd608 = (int *)0;
      DAT_007dd60c = 0;
    }
  }
  else {
    ReleaseSemaphore((void *)DAT_007dd61c,1,(long *)0);
  }
  if (*piVar1 != 0) {
    *(int *)piVar1[1] = *piVar1;
    *(int *)(*piVar1 + 4) = piVar1[1];
    *piVar1 = 0;
    piVar1[2] = 1;
  }
  ReleaseSemaphore(DAT_007dd620,1,(long *)0);
  ReleaseSemaphore(DAT_007dd618,1,(long *)0);
  if (bVar3) {
    FUN_005ab710(piVar1[7],0,2,1,param_1);
  }
  return;
}
