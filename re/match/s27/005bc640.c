// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 100.0%. Vtable calls (+8, +0x40) are __stdcall (typedef VFN1). Class: none. */
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
typedef int code();
void FUN_005bc450(int * p0);
void __stdcall FUN_005be160(int p0);
typedef void (__stdcall *VFN1)(int *);
// MATCH: 0x005bc640 FUN_005bc640
void FUN_005bc640(int *param_1)

{
  int *piVar1;
  
  piVar1 = (int *)param_1[1];
  if (piVar1 != (int *)0x0) {
    (*(VFN1 *)(*piVar1 + 8))(piVar1);
  }
  piVar1 = (int *)*param_1;
  if (piVar1 != (int *)0x0) {
    if (param_1[1] == 0) {
      (*(VFN1 *)(*piVar1 + 0x40))(piVar1);
    }
    (*(VFN1 *)(*(int *)*param_1 + 8))((int *)*param_1);
  }
  FUN_005be160((int)(param_1 + 2));
  FUN_005be160((int)(param_1 + 8));
  FUN_005bc450(param_1);
  return;
}
