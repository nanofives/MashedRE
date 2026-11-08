// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched. 100%. Ghidra idiom -(uint)(x != 0) & y is a ternary x ? y : 0 here (neg/sbb/and).
// h2 hand-matched.
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

undefined4 FUN_004e55d0(int p0);
undefined4 *FUN_004f0f10(uint p0, undefined4 p1, undefined4 p2);
// MATCH: 0x004e42d0 FUN_004e42d0
uint FUN_004e42d0(uint param_1, undefined4 param_2, int param_3)
{
  int e = FUN_004e55d0(param_3);
  undefined4 *r = FUN_004f0f10(param_1, e, e + 0x10);
  *(undefined4 **)(param_3 + 0x78) = r;
  return (r != 0) ? param_1 : 0;
}
