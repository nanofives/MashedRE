// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x005687d0: hand-matched (h2). 58.7% regs 63.0% ops 65.2%. Ghidra's decomp formula was wrong (not the bytes): rewritten from the disassembly as 8 aligned terms (n=[+0x38]: *0x60+0xf, *0x4c+0xf; m=[+0x30]: *0x80+0xf, *0x20+0x2f, *0x14+0x23, *0x10+0xf; k=[+0x34]: *0x20+0x10f, *0x120+0xf), summed in that order in the original. Every term and constant matches; the compiler reorders the add chain (k terms first) whatever the source order (tried t+= forward/reverse, temps, pairs, load order, permuter 448 compiles). Class b (term order and edi use point to inline-helper shape not found). */
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
// MATCH: 0x005687d0 FUN_005687d0
int FUN_005687d0(int param_1)
{
  int n = *(int *)(param_1 + 0x38);
  int k = *(int *)(param_1 + 0x34);
  int m = *(int *)(param_1 + 0x30);
  int v0 = (n * 0x60 + 0xf) & 0xfffffff0;
  int v1 = (n * 0x4c + 0xf) & 0xfffffff0;
  int v2 = (m * 0x80 + 0xf) & 0xfffffff0;
  int v3 = (m * 0x20 + 0x2f) & 0xfffffff0;
  int v4 = (m * 0x14 + 0x23) & 0xfffffff0;
  int v5 = (m * 0x10 + 0xf) & 0xfffffff0;
  int v6 = (k * 0x20 + 0x10f) & 0xfffffff0;
  int v7 = (k * 0x120 + 0xf) & 0xfffffff0;
  return v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7;
}
