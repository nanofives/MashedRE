// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 100.0%. Only fix: raw file lacked 'typedef long *LPLONG' (did not compile). Class: none. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef void *HANDLE;
typedef int BOOL;
typedef unsigned long DWORD;
typedef long LONG;
typedef long *LPLONG;
__declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE p0, LONG p1, LPLONG p2);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE p0, DWORD p1);
/* converter noise: extern int LPLONG; dropped (no address) */
// MATCH: 0x005b8090 FUN_005b8090
undefined4
FUN_005b8090(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,undefined4 param_6)

{
  undefined4 *puVar1;
  uint uVar2;
  
  WaitForSingleObject((HANDLE)param_1[3],0xffffffff);
  if ((*(ushort *)((int)param_1 + 6) == *(ushort *)(param_1 + 2)) &&
     (*(short *)((int)param_1 + 10) != 0)) {
    ReleaseSemaphore((HANDLE)param_1[3],1,(LPLONG)0x0);
    return 0;
  }
  puVar1 = (undefined4 *)(*param_1 + (uint)*(ushort *)((int)param_1 + 6) * 0x14);
  puVar1[1] = param_2;
  puVar1[2] = param_3;
  puVar1[4] = param_6;
  puVar1[3] = param_4;
  *puVar1 = param_5;
  uVar2 = *(ushort *)((int)param_1 + 6) + 1;
  if (uVar2 == *(ushort *)(param_1 + 1)) {
    uVar2 = 0;
  }
  if (uVar2 == *(ushort *)(param_1 + 2)) {
    *(undefined2 *)((int)param_1 + 10) = 1;
  }
  *(short *)((int)param_1 + 6) = (short)uVar2;
  ReleaseSemaphore((HANDLE)param_1[3],1,(LPLONG)0x0);
  return 1;
}
