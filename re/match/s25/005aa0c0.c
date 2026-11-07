// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005aa0c0: by hand + permuter. Best 56.2% (regs 76.2%, ops 91.2%, 80/80 ins). Residual (b): original keeps param_1 in esi with a write-back to its arg slot [esp+0x14], spills the list sentinel (param_1+0x10) into the dead param_5 slot [esp+0x24] and reloads it each iteration; ours holds param_1 in edi, keeps the sentinel arithmetic in a register. Permuter swapcmp (param_1 == puVar1 + 4 spelling, +30%) adopted; Ghidra goto-free while(1) form otherwise. */
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
typedef int (*S25_cb)(undefined4 *, undefined4 *, undefined4);
MGLOBAL(undefined4 *, DAT_009146fc, 0x009146fc);
undefined4 * FUN_005aa0c0(undefined4 * param_1, int * param_2, S25_cb param_3, undefined4 param_4, int param_5);
// MATCH: 0x005aa0c0 FUN_005aa0c0
undefined4 * FUN_005aa0c0(undefined4 *param_1,int *param_2,S25_cb param_3,undefined4 param_4,int param_5)
{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  if (param_1 == (undefined4 *)0x0) {
    param_1 = DAT_009146fc;
  }
  if ((param_5 == 1) && (iVar2 = (*param_3)(param_1,param_1,param_4), iVar2 == 1)) {
    if (param_2 != (int *)0x0) {
      *param_2 = (int)param_1;
    }
    return param_1;
  }
  puVar1 = (undefined4 *)param_1[4];
  while( 1 ) {
    if (param_1 == puVar1 + 4) {
      if (param_2 != (int *)0x0) {
        *param_2 = 0;
      }
      return (undefined4 *)0x0;
    }
    puVar3 = puVar1 + -6;
    iVar2 = (*param_3)(param_1,puVar3,param_4);
    if (iVar2 == 1) break;
    puVar3 = (undefined4 *)FUN_005aa0c0(puVar3,param_2,param_3,param_4,0);
    if (puVar3 != (undefined4 *)0x0) {
      return puVar3;
    }
    puVar1 = (undefined4 *)*puVar1;
  }
  if (param_2 != (int *)0x0) {
    *param_2 = (int)param_1;
  }
  return puVar3;
}
