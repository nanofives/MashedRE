// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c0a60: 100% regs 100%. Branch order parent!=0 first; non-zero branch is do-while with its own return, zero branch a for loop. */
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
void FUN_004c0ad0(int p0, undefined4 p1);
// MATCH: 0x004c0a60 FUN_004c0a60
int FUN_004c0a60(int param_1)
{
  undefined4 uVar1;
  int iVar2;

  iVar2 = *(int *)(param_1 + 0x98);
  if (*(int *)(param_1 + 4) != 0) {
    uVar1 = *(undefined4 *)(*(int *)(param_1 + 4) + 0xa0);
    *(undefined4 *)(param_1 + 0xa0) = uVar1;
    if (iVar2 != 0) {
      do {
        FUN_004c0ad0(iVar2,uVar1);
        iVar2 = *(int *)(iVar2 + 0x9c);
      } while (iVar2 != 0);
      return param_1;
    }
  }
  else {
    *(int *)(param_1 + 0xa0) = param_1;
    for (; iVar2 != 0; iVar2 = *(int *)(iVar2 + 0x9c)) {
      FUN_004c0ad0(iVar2,param_1);
    }
  }
  return param_1;
}
