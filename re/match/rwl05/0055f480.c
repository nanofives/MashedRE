// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055f480: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl05_20261107/results.tsv. */
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
// MATCH: 0x0055f480 FUN_0055f480
void FUN_0055f480(int param_1,int param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  iVar1 = *(int *)(param_3 + 0x3c);
  *(uint *)(param_1 + 8) = uVar2;
  *(uint *)(param_2 + 0x10) = uVar2;
  iVar1 = iVar1 + 1;
  uVar2 = *(int *)(param_1 + 8) + 0xf + iVar1 * 4 & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar2;
  *(uint *)(param_2 + 0x14) = uVar2;
  uVar2 = *(int *)(param_1 + 8) + 0xf + iVar1 * 0xc & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar2;
  *(uint *)(param_2 + 0x18) = uVar2;
  uVar2 = iVar1 * 0x10 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar2;
  *(uint *)(param_2 + 0x1c) = uVar2;
  uVar2 = iVar1 * 0x20 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar2;
  *(uint *)(param_2 + 0x20) = uVar2;
  uVar2 = *(int *)(param_1 + 8) + 0xf + iVar1 * 4 & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar2;
  *(uint *)(param_2 + 0x24) = uVar2;
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + 0xf + *(int *)(param_3 + 8) * 2 & 0xfffffff0;
  return;
}
