// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004d4910: best 55.4% (regs 85%, ops 97%), class (b). Callee FUN_004d7ff0 has 2 args, stack pair as an array, count unsigned. The original needs only ebx/esi/edi (count moves edx->ebx once param_2 is dead); ours takes ebp for the count. Permuter variants were broken (dereferenced address 0), discarded. */
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
undefined4 FUN_004d7ff0(undefined4 p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004d4910 FUN_004d4910
undefined4 FUN_004d4910(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint iVar4;
  undefined4 loc[2];
  
  iVar3 = *(int *)(param_1 + 0x20);
  iVar2 = (**(code **)(DAT_007d3ff8 + 0x110))(iVar3,param_2,0x1040409);
  if (iVar2 != 0) {
    iVar4 = *(uint *)(param_1 + 4);
    *(int *)(param_1 + 0x20) = iVar2;
    *(int *)(param_1 + 8) = iVar2;
    iVar2 = iVar2 - iVar3;
    *(undefined4 *)(param_1 + 0x24) = param_2;
    if (*(int *)(param_1 + 0x14) != 0) {
      *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + iVar2;
    }
    if (*(int *)(param_1 + 0x1c) != 0) {
      *(int *)(param_1 + 0x1c) = *(int *)(param_1 + 0x1c) + iVar2;
    }
    if (0 < iVar4) {
      iVar3 = 0;
      do {
        iVar1 = *(int *)(iVar3 + 8 + *(int *)(param_1 + 8));
        if (iVar1 != 0) {
          *(int *)(iVar3 + 8 + *(int *)(param_1 + 8)) = iVar1 + iVar2;
        }
        iVar1 = *(int *)(iVar3 + 0xc + *(int *)(param_1 + 8));
        if (iVar1 != 0) {
          *(int *)(iVar3 + 0xc + *(int *)(param_1 + 8)) = iVar1 + iVar2;
        }
        iVar1 = *(int *)(iVar3 + 0x10 + *(int *)(param_1 + 8));
        if (iVar1 != 0) {
          *(int *)(iVar3 + 0x10 + *(int *)(param_1 + 8)) = iVar1 + iVar2;
        }
        iVar1 = *(int *)(iVar3 + 0x14 + *(int *)(param_1 + 8));
        if (iVar1 != 0) {
          *(int *)(iVar3 + 0x14 + *(int *)(param_1 + 8)) = iVar1 + iVar2;
        }
        iVar1 = *(int *)(iVar3 + 0x18 + *(int *)(param_1 + 8));
        if (iVar1 != 0) {
          *(int *)(iVar3 + 0x18 + *(int *)(param_1 + 8)) = iVar1 + iVar2;
        }
        iVar1 = *(int *)(iVar3 + 0x1c + *(int *)(param_1 + 8));
        if (iVar1 != 0) {
          *(int *)(iVar3 + 0x1c + *(int *)(param_1 + 8)) = iVar1 + iVar2;
        }
        iVar3 = iVar3 + 0x28;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
    }
    return 1;
  }
  loc[0] = 1;
  loc[1] = FUN_004d7ff0(0x80000013,param_2);
  FUN_004d8480(loc);
  return 0;
}
