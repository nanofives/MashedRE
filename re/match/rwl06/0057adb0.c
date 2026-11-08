// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h3 0x0057adb0: best 100.0% (regs 100.0%, ops 100.0%), class (-). callee arg 3 passed as a dword, arg 7 as undefined4; result var r=1 set before the call (permuter swap); byte-identical. */
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
bool FUN_005757d0(int * p0, int p1);
undefined4 FUN_0057a9a0(undefined4 p0, undefined4 p1, float p2, int p3, int p4, float * p5, undefined4 p6);
// MATCH: 0x0057adb0 FUN_0057adb0
undefined4
FUN_0057adb0(undefined4 param_1,undefined4 param_2,float param_3,int param_4,undefined4 param_5)

{
  int iVar1;
  undefined4 r;

  r = 0;
  iVar1 = FUN_0057a9a0(param_1,param_2,param_3,param_4 + 0x100,param_4 + 0x10c,
                       (float *)(param_4 + 0x110),param_5);
  if ((iVar1 != 0) && (*(float *)(param_4 + 0x110) < param_3)) {
    r = 1;
    FUN_005757d0((int *)param_2,param_4);
  }
  return r;
}
