// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005aeda0 (h3): 90.0% exact, regs 100.0%. Residual class (b) last 5 instructions: uVar2/uVar3 register roles swapped in the shift/or tail; 4 tail orderings tried; (x & 0x80000000) != 0 fixed the 4 jge to jns. */
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
// MATCH: 0x005aeda0 FUN_005aeda0
void FUN_005aeda0(uint param_1,uint param_2,uint *param_3,uint *param_4)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  uVar2 = 0;
  uVar3 = 0;
  uVar1 = 0;
  for (; (param_1 & 0xfffffffe) != 0; param_1 = param_1 >> 1) {
    uVar1 = uVar1 * 2;
    if ((param_2 & 0x80000000) != 0) {
      uVar1 = uVar1 | 1;
      param_2 = param_2 & 0x7fffffff;
    }
    if ((param_1 & 1) != 0) {
      uVar2 = uVar2 + param_2;
      uVar3 = uVar3 + uVar1;
      if ((uVar2 & 0x80000000) != 0) {
        uVar3 = uVar3 + 1;
        uVar2 = uVar2 & 0x7fffffff;
      }
    }
    param_2 = param_2 * 2;
  }
  uVar1 = uVar1 * 2;
  if ((param_2 & 0x80000000) != 0) {
    uVar1 = uVar1 | 1;
    param_2 = param_2 & 0x7fffffff;
  }
  if ((param_1 & 1) != 0) {
    uVar2 = uVar2 + param_2;
    uVar3 = uVar3 + uVar1;
    if ((uVar2 & 0x80000000) != 0) {
      uVar3 = uVar3 + 1;
      uVar2 = uVar2 & 0x7fffffff;
    }
  }
  *param_3 = uVar3 >> 1;
  *param_4 = (uVar3 << 0x1f) | uVar2;
  return;
}
