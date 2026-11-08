// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c5a60: 83.7% regs 88.4%. Refcount-- with test of a copy: original keeps edx=count-1 stored and eax copy for test and +1; ours uses one register. Tried <=0, ++ form, copy variable, permuter (no gain). class (b). */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4054, 0x007d4054);
MGLOBAL(undefined4, DAT_00618138, 0x00618138);
undefined4 FUN_004c7650(undefined4 p0);
int FUN_004d8060(int p0, undefined4 p1);
// MATCH: 0x004c5a60 FUN_004c5a60
undefined4 FUN_004c5a60(int *param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = param_1[0x15] + -1;
  param_1[0x15] = iVar1;
  if (iVar1 <= 0) {
    iVar1 = iVar1 + 1;
    param_1[0x15] = iVar1;
    FUN_004d8060((int)&DAT_00618138,(undefined4)param_1);
    if (param_1[1] != 0) {
      *(int *)param_1[3] = param_1[2];
      *(int *)(param_1[2] + 4) = param_1[3];
    }
    if (*param_1 != 0) {
      FUN_004c7650(*param_1);
      *param_1 = 0;
    }
    param_1[0x15] = param_1[0x15] + -1;
    (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d4054 + 8 + DAT_007d3ff8),param_1);
  }
  return 1;
}
