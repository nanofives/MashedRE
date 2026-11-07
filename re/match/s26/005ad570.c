// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ad570 (h3): 100.0% exact (regs 100.0%). alloc-failure test merged after the if/else: if (p!=0) p+0x18=0 inside, common return-0 test after. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(undefined4, DAT_007dd6a8, 0x007dd6a8);
MGLOBAL(undefined1, DAT_007dd684, 0x007dd684);
void FUN_005ae0b0(undefined4 * p0);
uint FUN_005ae800(int * p0, undefined4 p1);
// MATCH: 0x005ad570 FUN_005ad570
int FUN_005ad570(int param_1)

{
  int *piVar1;
  
  if (param_1 == 0) {
    param_1 = FUN_005ae800((int *)&DAT_007dd684,0x3080a);
    if (param_1 != 0) {
      *(undefined4 *)(param_1 + 0x18) = 0;
    }
  }
  else {
    *(undefined4 *)(param_1 + 0x18) = 2;
  }
  if (param_1 == 0) {
    return 0;
  }
  *(undefined2 *)(param_1 + 0x14) = 0;
  *(undefined2 *)(param_1 + 0x16) = 0;
  *(undefined4 *)(param_1 + 0x10) = 0;
  *(undefined4 *)(param_1 + 0xc) = 0;
  *(undefined4 *)(param_1 + 0x1c) = 0;
  FUN_005ae0b0((undefined4 *)param_1);
  piVar1 = (int *)(param_1 + 0x20);
  *(undefined4 *)(param_1 + 0x24) = 0;
  *piVar1 = 0;
  *piVar1 = (int)DAT_007dd6a8;
  *(int **)(param_1 + 0x24) = (int *)&DAT_007dd6a8;
  *(int **)((int)DAT_007dd6a8 + 4) = piVar1;
  DAT_007dd6a8 = (undefined4)piVar1;
  return param_1;
}
