// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00553e00 (h2): 100.0% (regs 100). Raw: FUN_005c4d50 takes the pushed arg *param_1 (push hoisted above the branch, one add esp,4 after merge); real fn-ptr types for the vtable calls. */
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
typedef int (__cdecl *FN_t)(int);
typedef int (__cdecl *FN2_t)(int,int*);
MGLOBAL(undefined4, DAT_00912a24, 0x00912a24);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_0055deb0(int p0);
int FUN_005c4d50(int p0);
// MATCH: 0x00553e00 FUN_00553e00
int * FUN_00553e00(int *param_1)
{
  int *piVar1;
  int iVar2;

  if (param_1 != (int *)0x0) {
    iVar2 = (int)FUN_00553e00((int *)param_1[5]);
    param_1[5] = iVar2;
    iVar2 = FUN_0055deb0(*param_1);
    if (iVar2 == 1) {
      piVar1 = (int *)param_1[5];
      if (*param_1 != 0) {
        if (param_1[1] == -1) {
          FUN_005c4d50(*param_1);
        }
        else {
          (*(FN_t *)(DAT_007d3ff8 + 0x10c))(*param_1);
        }
      }
      *param_1 = 0;
      (*(FN2_t *)(DAT_007d3ff8 + 0x11c))(DAT_00912a24,param_1);
      return piVar1;
    }
  }
  return param_1;
}
