// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 62.5% (regs 65.9%, ops 84.1%), /O2 /Ob1 /G5 (was /O1 14.8%). Rewritten as switch(+0x48) with q = param_1 + 0x5c base (original addresses the +0x1xx fields off edi = esi+0x5c). Residual: 88/88 instructions, register assignment and branch layout of the case-2 block. Class: b. */
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
typedef unsigned int uint;
typedef unsigned char byte;
void FUN_005b0ec0(int p0);
void FUN_005b2080(int p0);
undefined4 FUN_005b2b00(int p0);
// MATCH: 0x005b7980 FUN_005b7980
int FUN_005b7980(int param_1)
{
  int q = param_1 + 0x5c;
  int iVar2;
  undefined4 uVar1;

  switch (*(int *)(param_1 + 0x48)) {
  case 1:
    if (*(int *)(param_1 + 0x50) != 3) {
      FUN_005b0ec0(param_1);
    }
    return param_1;
  case 2:
    if ((*(int *)(param_1 + 0x50) != 6) && (*(int *)(param_1 + 0x50) != 7)) {
      FUN_005b0ec0(param_1);
    }
    if ((*(uint *)(q + 0x188) & 0x10000) == 0) {
      if ((*(int *)(q + 0x154) != *(int *)(q + 0x14c)) ||
         (uVar1 = 7, *(int *)(*(int *)(q + 0x148) + 0x10 + *(int *)(q + 0x14c) * 0x14) != 1)) {
        uVar1 = 6;
      }
    }
    else if (((*(byte *)(*(int *)(q + 0x16c) + 0x10 + *(int *)(q + 0x178) * 0x14) & 1) != 0) ||
            (uVar1 = 6, (*(byte *)(*(int *)(q + 0x16c) + 0x10 + *(int *)(q + 0x178) * 0x14) & 4) != 0)) {
      uVar1 = 7;
    }
    *(undefined4 *)(param_1 + 0x50) = uVar1;
    break;
  case 3:
    break;
  default:
    return param_1;
  }
  iVar2 = FUN_005b2b00(q);
  if ((iVar2 == 3) || (iVar2 == 4)) {
    if (*(int *)(param_1 + 0x50) == 4) {
      FUN_005b0ec0(param_1);
    }
    if ((iVar2 == 4) && (*(int *)(param_1 + 0x48) == 2)) {
      *(undefined4 *)(param_1 + 0x48) = 1;
      FUN_005b0ec0(param_1);
    }
  }
  FUN_005b2080(q);
  return param_1;
}
