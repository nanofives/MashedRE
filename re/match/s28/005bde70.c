// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005bde70 : 42.3% (regs 65, ops 83). By hand: cdecl 4 args, switch(param_3) cases 1 (QueryInterface-like vtbl[3] then vtbl[0xb]) and 2, return 1 else 0. The original keeps param_3 in ebx and the constant 1 in edi, with the case-1 and default tails duplicated; ours puts 1 in ebx and param_3 in edi. Permuter reached 67% only by turning the hr compare unsigned (drops the failure test), rejected. Residual class (b), register allocation / tail layout. */

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

typedef int (__stdcall *F2)(int *, int *);
typedef int (__stdcall *F1)(int *);
// MATCH: 0x005bde70 FUN_005bde70
int FUN_005bde70(int *param_1, int *param_2, int param_3, int param_4)
{
  int iVar1;
  int r;

  r = 0;
  if (param_2 != (int *)0) {
    r = 1;
    switch (param_3) {
    case 1:
      param_1[3] = (int)param_2;
      iVar1 = ((F2)(*(int **)param_2)[3])(param_2, param_1);
      if (iVar1 < 0) {
        param_1[3] = 0;
        return 0;
      }
      param_1[2] = ((F1)(*(int **)param_1[3])[0xb])((int *)param_1[3]);
      break;
    case 2:
      param_1[3] = (int)param_2;
      param_1[0] = param_2[1];
      param_1[2] = param_4;
      break;
    }
    param_1[4] = param_3;
    param_1[1] = 0;
  }
  return r;
}
