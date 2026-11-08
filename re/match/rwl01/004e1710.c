// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004e1710: 100% (regs 100%). Permuter swap fixed the init order of the zero store and the loop variable. */
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
undefined4 FUN_004e15f0(undefined4 * p0, int * p1, undefined4 p2);
// MATCH: 0x004e1710 FUN_004e1710
undefined4 FUN_004e1710(int param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  
  puVar2 = (undefined4 *)0x0;
  *(undefined4 *)(param_1 + 0x14) = 0;
  for (puVar1 = *(undefined4 **)(*(int *)(param_1 + 4) + 8); puVar1 != (undefined4 *)0x0;
      puVar1 = (undefined4 *)puVar1[2]) {
    iVar3 = FUN_004e15f0(puVar1,(int *)puVar2,param_1);
    if (iVar3 == 0) {
      return 0;
    }
    if (puVar2 == (undefined4 *)0x0) {
      *(undefined4 *)(param_1 + 8) = *puVar1;
    }
    puVar2 = puVar1;
  }
  puVar1 = *(undefined4 **)(param_1 + 4);
  iVar3 = FUN_004e15f0(puVar1,(int *)puVar2,param_1);
  if (iVar3 == 0) {
    return 0;
  }
  if (puVar2 == (undefined4 *)0x0) {
    *(undefined4 *)(param_1 + 8) = *puVar1;
  }
  *(undefined4 *)(param_1 + 0x18) = 0;
  return 1;
}
