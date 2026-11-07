// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 31.6% (regs 89.5%, ops 100.0%). Tail call via 'return FUN(...)', volatile first flags store (reload), return param_1 on the fall-through. Residual: param_1/param_2 land in ecx/edx instead of eax/ecx and the stores chain differently. Class: b. */
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
typedef unsigned char byte;
int FUN_005c7570(int p0, int p1);
// MATCH: 0x005b92e0 FUN_005b92e0
int FUN_005b92e0(int param_1, int param_2)
{
  uint f = *(uint *)(param_1 + 0x28);
  if (param_2 != 0) {
    f = f | 0x200;
  } else {
    f = f & 0xfffffdff;
  }
  *(volatile uint *)(param_1 + 0x28) = f;
  *(uint *)(param_1 + 0x28) = f | 0x100;
  if ((*(byte *)(*(int *)(param_1 + 0x94) + 0x50) & 8) != 0) {
    return FUN_005c7570(*(int *)(param_1 + 0x11c), param_2);
  }
  return param_1;
}
