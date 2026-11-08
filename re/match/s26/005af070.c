// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005af070 (h3): 97.0% exact, regs 98.5%. Residual class (b) store scheduling of [esi+0x50]/[esi+0x54]: original stores 0x54 early and 0x50 after 0x44; 6 source orderings tried. FUN_005af140 is a function (was MGLOBAL byte, 82.6%); Ghidra param roles 6/7 swapped for 0x50/0x54. */
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
typedef int code();
typedef long LONG;
void FUN_005af140(void);
LONG * FUN_005aefa0(LONG * p0, LONG p1);
void FUN_005b0b90(undefined4 * p0);
// MATCH: 0x005af070 FUN_005af070
int FUN_005af070(int param_1,undefined4 *param_2,undefined4 param_3,undefined4 param_4,
                undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8)

{
  int iVar1;
  LONG *pLVar2;
  
  pLVar2 = FUN_005aefa0((LONG *)param_1,0);
  if (pLVar2 != 0) {
    pLVar2 = FUN_005aefa0((LONG *)(param_1 + 4),0);
    if (pLVar2 != 0) {
      pLVar2 = FUN_005aefa0((LONG *)(param_1 + 0x3c),0);
      if (pLVar2 != 0) {
        *param_2 = 0;
        param_2[2] = 0;
        param_2[1] = 0;
        FUN_005b0b90((undefined4 *)param_3);
        iVar1 = param_1 + 0x18;
        *(int *)(param_1 + 0x1c) = iVar1;
        *(int *)iVar1 = iVar1;
        iVar1 = param_1 + 0x20;
        *(int *)(param_1 + 0x24) = iVar1;
        *(int *)iVar1 = iVar1;
        *(undefined4 *)(param_1 + 0x48) = param_4;
        *(undefined4 *)(param_1 + 0x4c) = param_5;
        *(undefined4 *)(param_1 + 0x30) = 0;
        *(undefined4 *)(param_1 + 0xc) = 0;
        *(undefined4 *)(param_1 + 0x14) = 0;
        *(undefined4 *)(param_1 + 0x40) = 0;
        *(undefined4 *)(param_1 + 0x10) = 0;
        *(undefined4 *)(param_1 + 0x54) = param_6;
        *(code **)(param_1 + 0x28) = (code *)FUN_005af140;
        *(int *)(param_1 + 0x2c) = param_1;
        *(undefined4 *)(param_1 + 0x38) = 1;
        *(undefined4 *)(param_1 + 0x34) = 0x3f800000;
        *(undefined4 *)(param_1 + 8) = 6;
        *(undefined4 *)(param_1 + 0x44) = 0x20;
        *(undefined4 *)(param_1 + 0x50) = param_7;
        *(undefined4 *)(param_1 + 0x58) = param_8;
        return param_1;
      }
    }
  }
  return 0;
}
