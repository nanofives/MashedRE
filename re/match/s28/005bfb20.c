// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005bfb20 : 100.0% (regs 100). By hand: cdecl 3 args (obj,out1,out2), vtbl[5] stdcall fills two int64, divides by 10000000 via compiler __alldiv (0x004a4170). Residual: none. */

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

unsigned __int64 __stdcall FUN_004a4170(unsigned int p0, unsigned int p1, unsigned int p2, unsigned int p3);
// MATCH: 0x005bfb20 FUN_005bfb20
int FUN_005bfb20(int *param_1, int *param_2, int *param_3)
{
  int iVar1;
  __int64 a;
  __int64 b;

  iVar1 = (**(int (__stdcall **)(int *, __int64 *, __int64 *))(*param_1 + 0x14))(param_1, &a, &b);
  if (iVar1 >= 0) {
    *param_2 = (int)(a / 10000000);
    *param_3 = (int)((b - a) / 10000000);
    iVar1 = 0;
  }
  return iVar1;
}
