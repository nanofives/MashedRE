// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00567630: hand-matched (h2). 100.0% regs 100.0%. Re-derived from the disassembly (Ghidra's folded form was wrong): a running sum of 12 aligned terms ((x*k+0xf)&~0xf), n=<<2 value divided by 3 as uint (mul 0xaaaaaaab) and shifted back, final (sum+T+0xf)&~0xf. */
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
// MATCH: 0x00567630 FUN_00567630
int FUN_00567630(int param_1)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;

  iVar1 = *(int *)(param_1 + 0x28) << 2;
  iVar3 = *(int *)(param_1 + 0x14);
  iVar2 = *(int *)(param_1 + 0x20) + *(int *)(param_1 + 0x18);
  iVar4 = ((*(int *)(param_1 + 0x10) * 4 + 0xf) & 0xfffffff0) * 3;
  iVar4 += (((uint)iVar1 >> 2) * 0x10 + 0xf) & 0xfffffff0;
  iVar5 = (iVar1 * 4 + 0xf) & 0xfffffff0;
  iVar4 += iVar5 * 7;
  iVar4 += (iVar3 * 0x30 + 0xf) & 0xfffffff0;
  iVar4 += (iVar3 * 0x20 + 0xf) & 0xfffffff0;
  iVar4 += (iVar2 * 4 + 0xf) & 0xfffffff0;
  iVar4 = ((((uint)iVar1 / 3) * 4 + 0xf) & 0xfffffff0) + iVar4 * 2;
  iVar4 += (iVar2 * 0x40 + 0xf) & 0xfffffff0;
  iVar4 += (iVar1 * 0x40 + 0xf) & 0xfffffff0;
  return (iVar4 + ((iVar2 * 8 + 0xf) & 0xfffffff0) + 0xf) & 0xfffffff0;
}
