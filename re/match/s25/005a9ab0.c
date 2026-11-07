// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a9ab0 (h2): best 72.3% (regs 76.8%), class b. param_1 lives in ebp in the original, spilled by the DDK; permuter 300 s. relocheck BAD only because the code differs from the original. */
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
typedef unsigned char byte;
MGLOBAL(int, DAT_007dccc4, 0x007dccc4);
MGLOBAL(int, DAT_009146fc, 0x009146fc);
MGLOBAL(undefined1, DAT_007dcbf4, 0x007dcbf4);
MGLOBAL(undefined1, DAT_007dcc18, 0x007dcc18);
MGLOBAL(undefined1, DAT_007dcc9c, 0x007dcc9c);
void FUN_005a9be0(int p0);
void FUN_005a9c50(int p0);
int FUN_005ae800(int * p0, undefined4 p1);
// MATCH: 0x005a9ab0 FUN_005a9ab0
int FUN_005a9ab0(int param_1,byte param_2,int param_3,int param_4,int param_5,undefined4 param_6)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 *puVar4;

  if ((param_1 == 0) && (DAT_007dccc4 == 0)) {
    param_1 = DAT_009146fc;
  }
  if (param_3 == 0) {
    param_3 = FUN_005ae800((int *)&DAT_007dcc9c,0x30809);
    if (param_3 == 0) {
      return 0;
    }
    *(undefined4 *)(param_3 + 0xc) = 2;
  }
  else {
    *(undefined4 *)(param_3 + 0xc) = 0;
  }
  *(undefined4 *)(param_3 + 0x58) = param_6;
  if ((param_2 & 4) != 0) {
    if (param_4 == 0) {
      iVar3 = FUN_005ae800((int *)&DAT_007dcbf4,0x30809);
      *(int *)(param_3 + 0x4c) = iVar3;
      if (iVar3 == 0) {
        FUN_005a9be0(param_3);
        return 0;
      }
    }
    else {
      *(int *)(param_3 + 0x4c) = param_4;
      *(uint *)(param_3 + 0xc) = *(uint *)(param_3 + 0xc) | 0x10;
    }
    puVar4 = *(undefined4 **)(param_3 + 0x4c);
    for (iVar3 = 6; iVar3 != 0; iVar3 = iVar3 + -1) {
      puVar4 = puVar4 + 1;
      *puVar4 = 0;
    }
    *(uint *)(param_3 + 0xc) = *(uint *)(param_3 + 0xc) | 4;
  }
  if ((param_2 & 8) != 0) {
    if (param_5 == 0) {
      iVar3 = FUN_005ae800((int *)&DAT_007dcc18,0x30809);
      *(int *)(param_3 + 0x50) = iVar3;
      if (iVar3 == 0) {
        FUN_005a9be0(param_3);
        return 0;
      }
    }
    else {
      *(int *)(param_3 + 0x50) = param_5;
      *(uint *)(param_3 + 0xc) = *(uint *)(param_3 + 0xc) | 0x20;
    }
    puVar4 = *(undefined4 **)(param_3 + 0x50);
    *puVar4 = 0;
    puVar4[1] = 0;
    *(uint *)(param_3 + 0xc) = *(uint *)(param_3 + 0xc) | 8;
  }
  FUN_005a9c50(param_3);
  if (param_1 != 0) {
    uVar2 = *(undefined4 *)(param_1 + 0x10);
    piVar1 = (int *)(param_1 + 0x10);
    puVar4 = (undefined4 *)(param_3 + 0x18);
    *(int **)(param_3 + 0x1c) = piVar1;
    *puVar4 = uVar2;
    *(undefined4 **)(*piVar1 + 4) = puVar4;
    *piVar1 = (int)puVar4;
  }
  return param_3;
}
