// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005b0cf0: 43.9% (regs 47.0%, ops 83.3%), 66 vs 63 insns. Raw converter body plus compare spellings that match the original cmp operand order ((int)v >= local_8, *p >= iVar4) and (0 < count) for jbe. Residual: register allocation. The original keeps iVar4 in ebx and does not hoist param_6 (loaded just before the store); the DDK hoists param_6 into ebx and spills iVar4. Tried compare spellings, permuter (240 compiles, +1.6% only, semantics not verified so not taken). Class (b). cxx pending. */
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
// MATCH: 0x005b0cf0 FUN_005b0cf0
void FUN_005b0cf0(uint *param_1,int param_2,int param_3,uint param_4,int param_5,uint param_6)

{
  int iVar1;
  uint uVar2;
  uint *puVar3;
  int iVar4;
  uint uVar5;
  int local_8;
  int local_4;
  
  iVar1 = param_2;
  uVar5 = 0;
  param_2 = *(int *)(param_2 + 8);
  local_8 = 0;
  local_4 = 4;
  if (param_3 != -1) {
    local_8 = param_3;
    param_2 = param_3 + 1;
  }
  iVar4 = 0;
  if (param_5 != -1) {
    local_4 = param_5 + 1;
    iVar4 = param_5;
  }
  if (0 < *(uint *)(iVar1 + 4)) {
    puVar3 = param_1 + 8;
    do {
      uVar2 = puVar3[-2];
      if ((int)uVar2 < 0) {
        uVar2 = uVar2 & 0x7fffffff;
      }
      if (((((int)uVar2 >= local_8) && ((int)uVar2 < param_2)) && ((int)*puVar3 >= iVar4)) &&
         (((int)*puVar3 < local_4 && ((puVar3[-1] == param_4 || ((int)param_4 < 0)))))) {
        puVar3[1] = param_6;
        puVar3[-2] = uVar2 | 0x80000000;
      }
      uVar5 = uVar5 + 1;
      puVar3 = puVar3 + 4;
    } while (uVar5 < *(uint *)(iVar1 + 4));
  }
  *param_1 = *param_1 | 0x10;
  return;
}
