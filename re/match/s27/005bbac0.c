// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005bbac0: 64.9% (regs 97.3%). Branch order and int-return callee fixed. Residual: original hoists the [esp+0x14] load of param_4 above the branch (and orders the arg loads differently); single-join, local copy and permuter did not move it. Class: a.
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
typedef unsigned char byte;
int FUN_005abe30(int p0, int p1, undefined4 p2, int p3);
int FUN_005bc0f0(undefined4 * p0, undefined4 * p1, undefined4 p2, undefined4 p3);
// MATCH: 0x005bbac0 FUN_005bbac0
int FUN_005bbac0(int param_1,int param_2,undefined4 param_3,undefined4 param_4)
{
  int iVar1;

  if ((*(byte *)(*(int *)(*(int *)(param_1 + 0x58) + 0x20) + 0x50) & 8) != 0) {
    iVar1 = FUN_005abe30(*(int *)(param_1 + 0x4c),param_2,param_3,param_4);
    if (iVar1 == 0) {
      return 0;
    }
  }
  else {
    iVar1 = FUN_005bc0f0((undefined4 *)(param_1 + 0x5c),(undefined4 *)param_3,param_4,*(undefined4 *)(param_2 + 8));
    if (iVar1 == 0) {
      return 0;
    }
  }
  return param_1;
}
