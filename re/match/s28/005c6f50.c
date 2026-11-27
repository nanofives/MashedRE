// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c6f50: RO-AUDIOINIT-45D460-REACH-5, see below. */
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
/* RO-AUDIOINIT-45D460-REACH-5 (2026-11-27): new source from the Ghidra decompilation and the listing 0x005c6f50..0x005c7065
   (278 B, pc_funcs.csv; no hooks.csv row or re/match source before this row). 0x005c6cc0 takes its address
   (re/match/s28/005c6cc0.c). Field stores of param_1 from 0x005c6f73..0x005c6ff3 (1.0f = 0x3f800000 at 0x005c6f63, 0xff at
   0x005c6f89, 0x461c4000 at 0x005c6f91, 3 at 0x005c6faa), (float) of the uint at rec+0x18 through fild qword (0x005c6f76),
   rec = *(param_2+0x20) (0x005c6f5d); calls 0x005ab040 (0x005c6ff6), 0x005c6b40 twice (0x005c7008 / 0x005c7014);
   WaitForSingleObject(*(rec+4), -1) (0x005c7028), list insert of param_1+0xd0 at rec+0x1c (0x005c703e..0x005c7051),
   ReleaseSemaphore(*(rec+4), 1, 0) (0x005c7057), returns param_1. */
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void *hHandle, unsigned long dwMilliseconds);
__declspec(dllimport) int __stdcall ReleaseSemaphore(void *hSemaphore, long lReleaseCount, long *lpPreviousCount);
undefined4 FUN_005ab040(void);
void FUN_005c6b40(int param_1);

// MATCH: 0x005c6f50 FUN_005c6f50
int FUN_005c6f50(int param_1, int param_2)
{
  int *piVar1;
  int *piVar2;
  int iVar3;

  iVar3 = *(int *)(param_2 + 0x20);
  *(float *)(param_1 + 0x3c) = 1.0f;
  *(float *)(param_1 + 0x40) = 1.0f;
  *(float *)(param_1 + 0x38) = 1.0f;
  *(float *)(param_1 + 0x44) = 1.0f;
  *(float *)(param_1 + 100) = 1.0f;
  *(float *)(param_1 + 0x34) = (float)*(uint *)(iVar3 + 0x18);
  *(undefined4 *)(param_1 + 0x48) = 0;
  *(undefined4 *)(param_1 + 0x68) = 0x461c4000;
  *(undefined4 *)(param_1 + 0x4c) = 0;
  *(undefined4 *)(param_1 + 0x50) = 0;
  *(undefined4 *)(param_1 + 0x54) = 0;
  *(undefined4 *)(param_1 + 0x58) = 0;
  *(undefined4 *)(param_1 + 0x5c) = 0;
  *(undefined4 *)(param_1 + 0x60) = 0;
  *(undefined4 *)(param_1 + 0x28) = 3;
  *(undefined4 *)(param_1 + 0x2c) = 0;
  *(int *)(param_1 + 0x9c) = iVar3;
  *(undefined4 *)(param_1 + 0x98) = 0;
  *(undefined4 *)(param_1 + 0x88) = 0;
  *(undefined4 *)(param_1 + 0x8c) = 0;
  *(undefined4 *)(param_1 + 0x90) = 0;
  *(undefined4 *)(param_1 + 0x94) = 0;
  *(undefined4 *)(param_1 + 0x80) = 0;
  *(undefined4 *)(param_1 + 0x84) = 0;
  *(undefined4 *)(param_1 + 0x6c) = 0xff;
  *(undefined4 *)(param_1 + 0x70) = 0xff;
  *(undefined4 *)(param_1 + 0x74) = 0xff;
  *(undefined4 *)(param_1 + 0x78) = 0;
  *(undefined4 *)(param_1 + 0x7c) = *(undefined4 *)(iVar3 + 0x30);
  *(undefined4 *)(param_1 + 0xa0) = FUN_005ab040();
  FUN_005c6b40(param_1 + 0xa4);
  FUN_005c6b40(param_1 + 0xb8);
  *(undefined4 *)(param_1 + 0xcc) = 0;
  WaitForSingleObject(*(void **)(iVar3 + 4), 0xffffffff);
  piVar2 = (int *)(param_1 + 0xd0);
  piVar1 = (int *)(iVar3 + 0x1c);
  *(undefined4 *)(param_1 + 0xd4) = 0;
  *piVar2 = 0;
  *piVar2 = *piVar1;
  *(int **)(param_1 + 0xd4) = piVar1;
  *(int **)(*piVar1 + 4) = piVar2;
  *piVar1 = (int)piVar2;
  ReleaseSemaphore(*(void **)(iVar3 + 4), 1, (long *)0x0);
  return param_1;
}
