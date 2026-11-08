// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00568c80: hand pass (h2), body kept in raw form. 73.0% regs 94.6% ops 94.6%. Instruction stream identical; only register assignment differs (orig reads param_3 into ecx after the first store and keeps esi/edi as scratch, ours preloads param_3 into esi). Tried operand order, local copy of param_3, permuter 240 compiles (97.3% variant dropped a store, rejected as not equivalent). Class b. */
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
// MATCH: 0x00568c80 FUN_00568c80
void FUN_00568c80(int param_1,uint *param_2,int param_3,int param_4,int param_5)
{
  uint uVar1;

  uVar1 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  *param_2 = uVar1;
  uVar1 = *(int *)(param_1 + 8) + 0xf + param_3 * 0x4c & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  param_2[1] = uVar1;
  uVar1 = *(int *)(param_1 + 8) + 0xf + param_3 * 4 & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  param_2[2] = uVar1;
  uVar1 = param_5 * 0x10 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  param_2[3] = uVar1;
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + 0xf + param_4 * 4 & 0xfffffff0;
}
