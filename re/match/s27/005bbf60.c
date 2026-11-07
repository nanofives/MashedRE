// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005bbf60: 92.3% (regs 94.9%). stdcall vtable call and hr >= 0 fixed. Residual: order of the two bool-arg loads (original loads [edi+4] and [edi+8] together before the setne pairs). Locals for each arg and permuter did not move it. Class: a.
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
typedef int (__stdcall *VFN)(void *, undefined4, void *);
undefined4 FUN_005bbed0(undefined4 * p0, int p1, int p2);
// MATCH: 0x005bbf60 FUN_005bbf60
undefined4 FUN_005bbf60(int *param_1,undefined4 *param_2,undefined4 *param_3)
{
  int iVar1;

  *param_3 = 0;
  param_3[1] = 0;
  param_3[2] = 0;
  iVar1 = ((VFN)*(int *)(*param_1 + 0x14))(param_1,*param_2,param_3);
  if (iVar1 >= 0) {
    iVar1 = FUN_005bbed0(param_3,param_2[1] != 0,param_2[2] != 0);
    if (iVar1 != 0) {
      return 1;
    }
  }
  return 0;
}
