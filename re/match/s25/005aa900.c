// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005aa900 (h2): best 100.0% (regs 100.0%), class -. ushort "> 0" and swapped if/else on param_1[8]. relocheck. */
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
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
int * FUN_005aa9c0(int * p0);
// MATCH: 0x005aa900 FUN_005aa900
void FUN_005aa900(int *param_1,int param_2,uint param_3,int param_4)

{
  ushort uVar1;
  int *piVar2;
  int iVar3;
  int *piVar4;
  uint uVar5;
  
  *param_1 = param_2;
  param_1[7] = param_3;
  param_1[8] = (int)param_1 + param_4;
  if ((param_3 & 1) != 0) {
    piVar2 = (int *)FUN_005aa9c0(param_1);
    piVar4 = piVar2 + 2;
    *piVar2 = (int)piVar4;
    piVar2[1] = (int)(piVar4 + (uint)*(ushort *)(*(int *)(param_2 + 0x4c) + 0xc) * 3);
    uVar1 = *(ushort *)(*(int *)(param_2 + 0x4c) + 0xc);
    if (uVar1 > 0) {
      uVar5 = (uint)uVar1;
      do {
        *piVar4 = (int)piVar4;
        piVar4[1] = (int)piVar4;
        piVar4[2] = (int)piVar4;
        piVar4 = piVar4 + 3;
        uVar5 = uVar5 - 1;
      } while (uVar5 != 0);
    }
    iVar3 = piVar2[1];
    uVar1 = *(ushort *)(*(int *)(param_2 + 0x4c) + 0xe);
    if (uVar1 > 0) {
      uVar5 = (uint)uVar1;
      do {
        *(int *)iVar3 = iVar3;
        *(int *)(iVar3 + 4) = iVar3;
        iVar3 = iVar3 + 8;
        uVar5 = uVar5 - 1;
      } while (uVar5 != 0);
    }
  }
  if ((*(byte *)(param_1 + 7) & 2) != 0) {
    if (param_1[8] != 0) {
      piVar4 = (int *)((*(uint *)(*param_1 + 0x40) & 0xfffffff) + param_1[8]);
    }
    else {
      piVar4 = param_1 + 10;
    }
    *piVar4 = 0;
  }
  piVar4 = param_1 + 3;
  param_1[6] = 0;
  param_1[4] = (int)piVar4;
  param_1[5] = 0;
  *piVar4 = (int)piVar4;
  return;
}
