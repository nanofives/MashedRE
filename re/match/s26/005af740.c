// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005af740: 100% by hand: FUN_005af780 is a callback FUNCTION (passed as push imm), not a global: declared as a function and typed through cb_t. Callees FUN_005af180/FUN_005af200 (cdecl, 3 args) are in the h2 list. cxx pending. */
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
typedef long LONG;
typedef void (*cb_t)(void);
void FUN_005af780(void);
int * FUN_005af180(int p0, cb_t p1, int p2);
void FUN_005af200(int *p0, cb_t p1, int p2);
__declspec(dllimport) LONG __stdcall InterlockedExchange(LONG * p0, LONG p1);
// MATCH: 0x005af740 FUN_005af740
void FUN_005af740(int param_1,int param_2,int param_3)
{
  int *uVar1;

  if (param_3 == 2) {
    InterlockedExchange((LONG *)(param_1 + 0x3c),*(LONG *)(param_2 + 8));
  }
  uVar1 = FUN_005af180(param_1,FUN_005af780,param_3);
  FUN_005af200(uVar1,FUN_005af780,param_3);
  return;
}
