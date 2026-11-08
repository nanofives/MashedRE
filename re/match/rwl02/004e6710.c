// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e6710: best 54.8% (regs 54.8%), residual class b. orig tests the list head once and pushes regs before it, bottom-tested loop with ecx cursor; tried do/while, while, rotated; raw do-while kept (54.8), permuter flat. */
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

MGLOBAL(int, DAT_007d722c, 0x007d722c);
// MATCH: 0x004e6710 FUN_004e6710
int FUN_004e6710(int param_1,int (*param_2)(int,undefined4),undefined4 param_3)
{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  puVar2 = *(undefined4 **)(param_1 + 0x18);
  do {
    if (puVar2 == (undefined4 *)(param_1 + 0x18)) {
      return param_1;
    }
    puVar1 = (undefined4 *)*puVar2;
    iVar3 = (*param_2)((int)puVar2 + (-4 - DAT_007d722c),param_3);
    puVar2 = puVar1;
  } while (iVar3 != 0);
  return param_1;
}
