// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005ab560: by hand. Best 98.2% (regs 98.2%, ops 100.0%, 57/57 ins). Fields addressed from piVar1 = param_1 + 1 (esi base) instead of param_1: 61 -> 88%; not hoisting the DAT_007dd624 read out of the else branch: 98%. Residual (a): one load (mov eax,[0x7dd624]) is placed after the sentinel store in ours, before it in the original; a temp before the store fixes the order but changes the second load's register (93%). Imports declared dllimport stdcall. */
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
MGLOBAL(void *, DAT_007dd620, 0x007dd620);
MGLOBAL(int *, DAT_007dd624, 0x007dd624);
MGLOBAL(int *, DAT_007dd62c, 0x007dd62c);
MGLOBAL(undefined1, DAT_007dd61c, 0x007dd61c);
int FUN_005aeed0(undefined1 * p0);
void FUN_005ab710(int p0, int p1, int p2, int p3, int * p4);
// MATCH: 0x005ab560 FUN_005ab560
void FUN_005ab560(int *param_1,int param_2,int param_3,int param_4,int param_5)
{
  int *piVar1;
  int iVar3;
  piVar1 = param_1 + 1;
  piVar1[2] = 3;
  piVar1[7] = *(int *)(*param_1 + 0xc);
  piVar1[4] = param_3;
  piVar1[5] = param_3;
  piVar1[6] = param_2;
  piVar1[0xd] = param_4;
  piVar1[0xe] = param_5;
  WaitForSingleObject(DAT_007dd620,0xffffffff);
  iVar3 = FUN_005aeed0(&DAT_007dd61c);
  if (iVar3 != 0) {
    DAT_007dd62c = piVar1;
    ReleaseSemaphore(DAT_007dd620,1,(long *)0);
    FUN_005ab710(piVar1[7],piVar1[4],0,1,param_1);
    return;
  }
  piVar1[1] = (int)&DAT_007dd624;
  *piVar1 = (int)DAT_007dd624;
  *(int **)((int)DAT_007dd624 + 4) = piVar1;
  DAT_007dd624 = piVar1;
  piVar1[2] = 2;
  ReleaseSemaphore(DAT_007dd620,1,(long *)0);
  return;
}
