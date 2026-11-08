// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a7460 (h2): best 100.0% (regs 100.0%), class -. permuter swap, variadic 005a6df0 prototype. relocheck. */
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
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(int *, DAT_007dca24, 0x007dca24);
MGLOBAL(int, DAT_007dca3c, 0x007dca3c);
MGLOBAL(int, DAT_007dca40, 0x007dca40);
void FUN_005a6dc0(int p0, undefined4 p1, undefined4 p2, undefined4 p3);
void FUN_005a6df0(int p0, ...);
undefined4 FUN_005a6e10(undefined4 p0, undefined4 p1, undefined4 p2, uint * p3);
// MATCH: 0x005a7460 FUN_005a7460
void FUN_005a7460(int *param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;

  if ((*(byte *)(param_1 + 3) & 1) != 0) {
    *(int *)param_1[1] = *param_1;
    *(int *)(*param_1 + 4) = param_1[1];
    iVar1 = param_1[2];
    if (iVar1 != 0) {
      iVar3 = FUN_005a6e10(iVar1,6,2,0);
      if (iVar3 != 0) {
        FUN_005a6dc0(iVar1,6,1,0);
      }
      FUN_005a6df0(iVar1,9,1,0);
      param_1[2] = 0;
    }
    piVar2 = DAT_007dca24;
    *param_1 = (int)piVar2;
    param_1[1] = (int)&DAT_007dca24;
    DAT_007dca24[1] = (int)param_1;
    DAT_007dca24 = param_1;
    DAT_007dca3c = DAT_007dca3c + -1;
    DAT_007dca40 = DAT_007dca40 - param_1[4];
    param_1[8] = param_1[9];
    param_1[3] = param_1[3] & 0xfffffffc;
    param_1[0x16] = 0;
    param_1[4] = 0;
  }
  return;
}
