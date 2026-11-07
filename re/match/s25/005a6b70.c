// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a6b70 (h2): best 84.6% (regs 84.6%), class a. original duplicates the null-return epilogue inline, DDK shares it; locals as array. relocheck BAD only because the code differs from the original. */
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
MGLOBAL(int *, DAT_007dca0c, 0x007dca0c);
int * FUN_005a6c60(undefined4 * p0);
void FUN_005a6c90(int * p0);
int FUN_005ad0b0(int p0, int p1, int p2, int p3);
// MATCH: 0x005a6b70 FUN_005a6b70
int * FUN_005a6b70(int *param_1,int param_2,undefined4 *param_3)

{
  int *piVar1;
  int iVar2;
  int *piVar3;
  undefined4 loc[6];

  if (param_3 == (undefined4 *)0x0) {
    loc[4] = 0;
    loc[3] = 0;
    loc[2] = 0;
    loc[1] = 0;
    loc[0] = 2;
    loc[5] = 0;
    param_3 = loc;
  }
  piVar1 = FUN_005a6c60(param_3);
  if (piVar1 == (int *)0x0) {
    return (int *)0x0;
  }
  *piVar1 = (int)param_1;
  piVar1[4] = piVar1[4] | 2;
  piVar1[1] = 0;
  piVar1[5] = 0;
  if ((param_2 != 0) || (param_2 = **(int **)(*param_1 + 0x4c), param_2 != 0)) {
    iVar2 = FUN_005ad0b0(param_2,*param_1,param_3[3],param_3[4]);
    piVar1[1] = iVar2;
    if (iVar2 == 0) {
      FUN_005a6c90(piVar1);
      return (int *)0x0;
    }
  }
  piVar3 = (int *)*piVar1;
  if (piVar3[8] == 0) {
    piVar3 = piVar3 + 10;
  }
  else {
    piVar3 = (int *)((*(uint *)(*piVar3 + 0x40) & 0xfffffff) + piVar3[8]);
  }
  *piVar3 = *piVar3 + 1;
  piVar3 = piVar1 + 2;
  piVar1[3] = 0;
  *piVar3 = 0;
  *piVar3 = (int)DAT_007dca0c;
  piVar1[3] = (int)&DAT_007dca0c;
  *(int **)((int)DAT_007dca0c + 4) = piVar3;
  DAT_007dca0c = piVar3;
  return piVar1;
}
