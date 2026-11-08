// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00578e50: hand-matched (h2). 100.0% regs 100.0%. Callee 0x00578610 takes 7 args (add esp,0x1c). Result in a local set to 0 up front and to 1 BEFORE the 0x005757d0 call gives the edi=1 hold. */
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
undefined4 FUN_00578610(undefined4 p0, int * p1, float p2, float * p3, float * p4, float * p5, undefined4 p6);
// MATCH: 0x00578e50 FUN_00578e50
undefined4
FUN_00578e50(undefined4 param_1,undefined4 param_2,float param_3,int param_4,undefined4 param_5)
{
  int iVar1;
  undefined4 uVar2 = 0;

  iVar1 = FUN_00578610(param_1,(int *)param_2,param_3,(float *)(param_4 + 0x100),(float *)(param_4 + 0x10c),
                       (float *)(param_4 + 0x110),param_5);
  if ((iVar1 != 0) && (*(float *)(param_4 + 0x110) < param_3)) {
    uVar2 = 1;
    FUN_005757d0((int *)param_2,param_4);
  }
  return uVar2;
}
