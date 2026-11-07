// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005bfe60: 100% (regs 100%). Compile-fail raw rewritten; FUN_005bf670 is stdcall (ret 4), 2nd param of this fn is the out pointer (ret 8). Class: none. */
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
__declspec(dllimport) void __stdcall EnterCriticalSection(void *p0);
__declspec(dllimport) void __stdcall LeaveCriticalSection(void *p0);
void __stdcall FUN_005bf670(int p0);
int FUN_005c0e00(int p0, int p1);
// MATCH: 0x005bfe60 FUN_005bfe60
int __stdcall FUN_005bfe60(int *param_1, int *param_2)
{
  int iVar1;

  EnterCriticalSection((void *)param_1[0x4d]);
  if (param_1[0x4e] == 0) {
    iVar1 = FUN_005c0e00(0, 0);
    param_1[0x4e] = iVar1;
    if (iVar1 == 0) {
      return 0x8004020a;
    }
  }
  FUN_005bf670(param_1[0x4e]);
  *param_2 = param_1[0x4e];
  LeaveCriticalSection((void *)param_1[0x4d]);
  return 0;
}
