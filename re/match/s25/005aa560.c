// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005aa560 (h2): best 79.8% (regs 79.8%), class b. prologue/arg load order and register allocation; 568 B, structure right, permuter 300 s. relocheck BAD only because the code differs from the original. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
MGLOBAL(int, DAT_007dcd20, 0x007dcd20);
MGLOBAL(int *, DAT_007dccf0, 0x007dccf0);
void FUN_004522d0(undefined1 * p0);
void FUN_005a9f60(int * p0, int p1, int p2);
int FUN_005aa030(int p0, int p1);
int * FUN_005aa7a0(int p0, uint * p1, undefined1 * p2, uint p3);
uint FUN_005aa7e0(int p0, uint p1, uint * p2);
void FUN_005aa900(int * p0, int p1, uint p2, uint p3);
undefined4 * FUN_005aad40(void);
undefined1 * FUN_005aea00(uint p0, uint p1);
// MATCH: 0x005aa560 FUN_005aa560
int * FUN_005aa560(int param_1,undefined4 *param_2,uint param_3,undefined1 *param_4,
                   int * (*param_5)(int, undefined4, undefined1 *, uint),undefined4 param_6)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  undefined1 *puVar4;
  uint uVar5;
  int *piVar6;
  undefined1 *local_208;
  uint local_204;
  undefined1 local_200[512];

  param_3 = param_3 & 7;
  if (DAT_007dcd20 != 0) {
    if (param_2 == (undefined4 *)0x0) {
      param_2 = FUN_005aad40();
    }
    iVar3 = FUN_005aa030(*param_2,param_1);
    if (iVar3 == 0) {
      return (int *)0x0;
    }
  }
  local_208 = (undefined1 *)0x0;
  puVar4 = param_4;
  if (param_4 == (undefined1 *)0x0) {
    puVar4 = local_200;
    if ((0x200 < *(ushort *)(param_1 + 0x44)) &&
       (puVar4 = FUN_005aea00(*(ushort *)(param_1 + 0x44),0x1080b), local_208 = puVar4
       , puVar4 == (undefined1 *)0x0)) {
      return (int *)0x0;
    }
    if (!(*(int *)(param_1 + 0x3c) == 0)) {
      (**(void (**)(int, undefined1 *))(param_1 + 0x3c))(0,puVar4);
    } else {
      puVar4 = (undefined1 *)0;
    }
  }
  uVar5 = FUN_005aa7e0(param_1,param_3,&local_204);
  if ((param_5 == 0) ||
     (piVar6 = (*param_5)(param_1,param_6,puVar4,uVar5), piVar6 == (int *)0x0)) {
    piVar6 = FUN_005aa7a0(param_1,&param_3,puVar4,uVar5);
    if (piVar6 == (int *)0x0) {
      if (local_208 != (undefined1 *)0x0) {
        FUN_004522d0(local_208);
      }
      return (int *)0x0;
    }
  }
  else {
    param_3 = param_3 | 0x20;
  }
  piVar6[9] = (int)param_2;
  FUN_005aa900(piVar6,param_1,param_3,local_204);
  if (DAT_007dcd20 != 0) {
    iVar3 = param_2[3];
    piVar1 = (int *)(param_2 + 3);
    piVar2 = piVar6 + 5;
    piVar6[6] = (int)piVar1;
    *piVar2 = iVar3;
    *(int **)(*piVar1 + 4) = piVar2;
    *piVar1 = (int)piVar2;
  }
  if ((*(int *)(*piVar6 + 0x28) != 0) &&
     (iVar3 = (**(int (**)(int *, undefined4 *, undefined1 *, int * (*)(int, undefined4, undefined1 *, uint), undefined4))(*piVar6 + 0x28))(piVar6,param_2,puVar4,param_5,param_6), iVar3 == 0)) {
    if (local_208 != (undefined1 *)0x0) {
      FUN_004522d0(local_208);
    }
    if (DAT_007dcd20 != 0) {
      *(int *)piVar6[6] = piVar6[5];
      *(int *)(piVar6[5] + 4) = piVar6[6];
    }
    FUN_005a9f60(piVar6,0,0);
    return (int *)0x0;
  }
  if (local_208 != (undefined1 *)0x0) {
    FUN_004522d0(local_208);
  }
  piVar1 = DAT_007dccf0;
  piVar2 = piVar6 + 1;
  *piVar2 = (int)piVar1;
  piVar6[2] = (int)&DAT_007dccf0;
  *(int **)((int)DAT_007dccf0 + 4) = piVar2;
  DAT_007dccf0 = piVar2;
  return piVar6;
}
