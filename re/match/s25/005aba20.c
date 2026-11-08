// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005aba20 (h2): best 81.9% (regs 85.2%), class b. 642 B, structure right (struct copies, memcpy intrinsic), branch layout and register allocation; permuter 300 s. relocheck BAD only because the code differs from the original. */
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
typedef struct { int a[7]; } S28_005aba20;
MGLOBAL(undefined1, DAT_007dd634, 0x007dd634);
void * __cdecl memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
void FUN_005a7b00(undefined4 p0);
undefined4 FUN_005a7b50(int p0);
void FUN_005abcb0(int p0);
int FUN_005ac5f0(undefined4 * p0, undefined4 * p1);
void FUN_005ac980(int p0, undefined4 * p1, int p2);
int FUN_005acd10(int p0, undefined4 * p1);
void FUN_005ae0b0(int p0);
int FUN_005ae650(int p0, uint p1, uint p2, int p3, uint p4, undefined4 p5);
int FUN_005ae800(undefined1 * p0, undefined4 p1);
undefined4 * FUN_005aea00(uint p0, undefined4 p1);
undefined4 FUN_005b3b60(undefined4 * p0, undefined4 p1, undefined4 * p2);
// MATCH: 0x005aba20 FUN_005aba20
int FUN_005aba20(int param_1,undefined4 param_2,undefined4 *param_3,undefined4 *param_4,uint param_5
                ,int param_6)

{
  int (*pcVar1)(int, undefined4);
  int iVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  uint uVar6;
  undefined1 *puVar8;
  undefined4 local_1c [7];

  if (((param_4 != (undefined4 *)0x0) && (param_4[1] != 0)) && (*(uint *)(param_1 + 0x24) >= 1)) {
    iVar2 = FUN_005acd10(param_1,param_4);
    if (iVar2 == 0) {
      return 0;
    }
  }
  else {
    if ((*(int *)(param_1 + 0x28) != 0) &&
       (iVar2 = *(int *)(*(int *)(param_1 + 0x28) + *(int *)(param_1 + 0x2c) * 4), iVar2 != 0)) {
      FUN_005ac980(iVar2,local_1c,1);
      param_4 = local_1c;
      local_1c[0] = *param_3;
      local_1c[4] = param_3[4];
      local_1c[5] = param_3[5];
    }
    else if (param_4 == (undefined4 *)0x0) {
      param_4 = param_3;
    }
  }
  iVar2 = FUN_005ac5f0(param_3,param_4);
  if (iVar2 != 0) {
    param_4[4] = param_3[4];
    param_4[5] = param_3[5];
  }
  param_4[2] = uVar3;
  uVar3 = FUN_005b3b60(param_3,0,param_4);
  if (param_6 != 0) {
    *(undefined4 *)(param_6 + 0x54) = 4;
  }
  else {
    if (*(int *)(param_1 + 0x34) == 0) {
      puVar8 = &DAT_007dd634;
    }
    else {
      if (*(int *)(param_1 + 0x3c) == 0) {
        iVar2 = FUN_005ae650(*(int *)(param_1 + 0x34) + 0x5c,8,1,1,0,0x30806);
        *(int *)(param_1 + 0x3c) = iVar2;
        if (iVar2 == 0) {
          return 0;
        }
      }
      puVar8 = *(undefined1 **)(param_1 + 0x3c);
    }
    param_6 = FUN_005ae800(puVar8,0x30806);
    if (param_6 == 0) {
      return 0;
    }
    *(undefined4 *)(param_6 + 0x54) = 1;
  }
  FUN_005ae0b0(param_6);
  *(int *)(param_6 + 0xc) = param_1;
  *(undefined4 *)(param_6 + 0x4c) = 0;
  *(undefined4 *)(param_6 + 0x48) = 0;
  *(undefined4 *)(param_6 + 0x58) = param_2;
  *(undefined4 *)(param_6 + 0x10) = 0;
  *(undefined4 *)(param_6 + 0x14) = 0;
  *(undefined4 *)(param_6 + 0x18) = 0;
  *(undefined1 *)(param_6 + 0x1c) = 0;
  *(undefined1 *)(param_6 + 0x1d) = 0;
  *(undefined1 *)(param_6 + 0x28) = 0;
  *(undefined1 *)(param_6 + 0x29) = 0;
  *(undefined4 *)(param_6 + 0x20) = 0;
  *(undefined4 *)(param_6 + 0x24) = 0;
  *(S28_005aba20 *)(param_6 + 0x10) = *(S28_005aba20 *)param_3;
  if ((param_3[4] != 0) && (param_3[5] > 0)) {
    puVar4 = FUN_005aea00(param_3[5],0x30806);
    *(undefined4 **)(param_6 + 0x20) = puVar4;
    if (puVar4 == (undefined4 *)0x0) {
      return 0;
    }
    memcpy(puVar4,(void *)param_3[4],param_3[5]);
  }
  *(undefined4 *)(param_6 + 0x2c) = 0;
  *(undefined4 *)(param_6 + 0x30) = 0;
  *(undefined4 *)(param_6 + 0x34) = 0;
  *(undefined1 *)(param_6 + 0x38) = 0;
  *(undefined1 *)(param_6 + 0x39) = 0;
  *(undefined1 *)(param_6 + 0x44) = 0;
  *(undefined1 *)(param_6 + 0x45) = 0;
  *(undefined4 *)(param_6 + 0x3c) = 0;
  *(undefined4 *)(param_6 + 0x40) = 0;
  *(S28_005aba20 *)(param_6 + 0x2c) = *(S28_005aba20 *)param_4;
  if ((param_4[4] != 0) && (param_4[5] > 0)) {
    puVar4 = FUN_005aea00(param_4[5],0x30806);
    *(undefined4 **)(param_6 + 0x3c) = puVar4;
    if (puVar4 == (undefined4 *)0x0) {
      return 0;
    }
    memcpy(puVar4,(void *)param_4[4],param_4[5]);
  }
  if (*(char *)(param_6 + 0x39) == '\x01') {
    if (!((param_5 & 1) == 0)) {
      uVar6 = 2;
    } else {
      uVar6 = param_5 & 2;
    }
    *(uint *)(param_6 + 0x54) = *(uint *)(param_6 + 0x54) | uVar6;
  }
  *(uint *)(param_6 + 0x54) = *(uint *)(param_6 + 0x54) | param_5 & 8;
  if (*(int *)(*(int *)(param_6 + 0xc) + 0x34) != 0) {
    *(int *)(param_6 + 0x50) = param_6 + 0x5c;
  }
  pcVar1 = *(int (**)(int, undefined4))(*(int *)(param_6 + 0xc) + 0xc);
  if ((pcVar1 != 0) && (iVar2 = (*pcVar1)(param_6,param_2), iVar2 == 0)) {
    FUN_005abcb0(param_6);
    return 0;
  }
  uVar3 = FUN_005a7b50(param_6);
  FUN_005a7b00(uVar3);
  return param_6;
}
