// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005aef30 (h3): 100.0% exact (regs 100.0%). __beginthread cdecl and FUN_005aef70 declared as real functions (relocheck unchecked: ___beginthread, SetThreadPriority). */
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
typedef void *HANDLE;
typedef int BOOL;
typedef unsigned int uintptr_t;
void __cdecl FUN_005aef70(void *p);
__declspec(dllimport) BOOL __stdcall SetThreadPriority(HANDLE p0, int p1);
uintptr_t __cdecl __beginthread(void (__cdecl *p0)(void *), uint p1, void * p2);
// MATCH: 0x005aef30 FUN_005aef30
undefined4 * FUN_005aef30(undefined4 *param_1,undefined4 param_2)

{
  HANDLE hThread;
  
  param_1[2] = param_2;
  hThread = (HANDLE)__beginthread(FUN_005aef70,param_1[4],(void *)param_1);
  if (hThread == (HANDLE)0xffffffff) {
    return (undefined4 *)0x0;
  }
  *param_1 = (undefined4)hThread;
  SetThreadPriority(hThread,param_1[3]);
  return param_1;
}
