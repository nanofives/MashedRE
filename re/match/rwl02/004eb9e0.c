// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004eb9e0: best 64.7% (regs 82.4%), residual class b. orig keeps min in ecx and max in edx, and and eax,0xffff after mov ax; ours swaps min/max registers and loads differently; permuter swapcmp + *param_4 stored before *param_3 (independent outputs). */
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

// MATCH: 0x004eb9e0 FUN_004eb9e0
void FUN_004eb9e0(ushort *param_1,int param_2,int *param_3,uint *param_4)
{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  if (param_2 != 0) {
    uVar2 = 0xffffffff;
    uVar3 = 0;
    do {
      uVar1 = (uint)*param_1;
      if (uVar2 >= uVar1) {
        uVar2 = uVar1;
      }
      if (uVar3 <= uVar1) {
        uVar3 = uVar1;
      }
      param_1 = param_1 + 1;
      param_2 = param_2 + -1;
    } while (param_2 != 0);
    *param_4 = uVar2;
    *param_3 = (uVar3 - uVar2) + 1;
    return;
  }
  *param_3 = 0;
  *param_4 = 0;
}
