// FLAGS: /O2 /Ob1 /G5
/* MATCH-NEARMISS-2-pilot 2026-11-09: 100% BYTE-IDENTICAL by hand: the list-head init at param_1+0xc written as rwLinkListInitialize order through a typed {next, prev} link (next = self, then prev = self), no piVar1 temp, then param_1[5] = 0. The older note below describes the pre-pilot residual. */
/* MATCH-S25 h3 0x005a7ee0: by hand + permuter. Best 97.7% (regs 97.7%, 44/44 ins). Residual (a): one store order, original does [esi+0x14]=0 before [eax]=eax (list head self-link); the swap that fixes it moves lea edi/[esi+0x10] store out of order instead (95.5% the other way). piVar2 += 4 for the +0x10 step, permuter swap of the two list-head stores. */
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
typedef void (*S25_fn1)(int *);
MGLOBAL(int *, DAT_007dca7c, 0x007dca7c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_005ae0c0(int * p0, int * p1, undefined4 p2);
void FUN_005ae010(int * p0, int * p1);
void FUN_005adfe0(int * p0, int * p1);
typedef struct LL { struct LL *next; struct LL *prev; } LL;
// MATCH: 0x005a7ee0 FUN_005a7ee0
int * FUN_005a7ee0(int *param_1,undefined4 param_2)
{
  int *piVar1;
  int *piVar2;
  piVar2 = param_1 + 9;
  ((LL *)(param_1 + 3))->next = (LL *)(param_1 + 3);
  ((LL *)(param_1 + 3))->prev = (LL *)(param_1 + 3);
  param_1[5] = 0;
  if (*param_1 != 0) {
    FUN_005ae0c0(piVar2,piVar2,param_2);
    FUN_005ae010(param_1,piVar2);
    piVar2 = piVar2 + 4;
  }
  if (param_1[1] != 0) {
    FUN_005adfe0(param_1,piVar2);
    (**(S25_fn1 *)(DAT_007d3ff8 + 0xf4))(piVar2);
  }
  piVar1 = param_1 + 7;
  param_1[8] = 0;
  *piVar1 = 0;
  *piVar1 = (int)DAT_007dca7c;
  param_1[8] = (int)&DAT_007dca7c;
  *(int **)((int)DAT_007dca7c + 4) = piVar1;
  DAT_007dca7c = piVar1;
  return param_1;
}
