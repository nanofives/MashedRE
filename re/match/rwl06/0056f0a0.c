// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0056f0a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
MGLOBAL(undefined4, DAT_005e5738, 0x005e5738);
void FUN_0056f1f0(int * p0, undefined4 * p1);
// MATCH: 0x0056f0a0 FUN_0056f0a0
void FUN_0056f0a0(int param_1)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  
  uVar2 = *(uint *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4);
  uVar4 = uVar2 + 3 & 0xfffffffc;
  if (uVar2 != uVar4) {
    iVar3 = uVar4 - uVar2;
    do {
      FUN_0056f1f0(param_1,&DAT_005e5738);
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  *(uint *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4) =
       *(int *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4) + (uVar2 - uVar4);
  iVar3 = (int)(uVar4 + ((int)uVar4 >> 0x1f & 3U)) >> 2;
  *(uint *)(param_1 + 4) = *(int *)(param_1 + 4) + uVar4;
  *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + iVar3;
  *(int *)(param_1 + 0x20) = *(int *)(param_1 + 0x20) + iVar3;
  *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + iVar3;
  *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + iVar3;
  *(int *)(param_1 + 0x44) = *(int *)(param_1 + 0x44) + iVar3;
  *(int *)(param_1 + 0x50) = *(int *)(param_1 + 0x50) + iVar3;
  *(int *)(param_1 + 0x5c) = *(int *)(param_1 + 0x5c) + iVar3;
  *(int *)(param_1 + 0x68) = *(int *)(param_1 + 0x68) + iVar3;
  *(int *)(param_1 + 0x8c) = *(int *)(param_1 + 0x8c) + iVar3;
  *(uint *)(param_1 + 0x98) = *(int *)(param_1 + 0x98) + uVar4;
  *(int *)(param_1 + 0xb0) = *(int *)(param_1 + 0xb0) + 1;
  *(uint *)(*(int *)(param_1 + 0xdc) + *(int *)(param_1 + 0xe0) * 4) =
       *(int *)(*(int *)(param_1 + 0xdc) + *(int *)(param_1 + 0xe0) * 4) + uVar4;
  *(uint *)(*(int *)(param_1 + 0xd0) + *(int *)(param_1 + 0xd4) * 4) =
       *(int *)(*(int *)(param_1 + 0xd0) + *(int *)(param_1 + 0xd4) * 4) + uVar2;
  piVar1 = (int *)(*(int *)(param_1 + 0xc4) + *(int *)(param_1 + 0xd4) * 4);
  *piVar1 = *piVar1 + 1;
  *(uint *)(param_1 + 0x100) = *(int *)(param_1 + 0x100) + uVar2;
  *(uint *)(param_1 + 0x104) = *(int *)(param_1 + 0x104) + uVar2;
  *(uint *)(param_1 + 0x108) = *(int *)(param_1 + 0x108) + uVar4;
  *(int *)(param_1 + 0xf8) = *(int *)(param_1 + 0xf8) + 1;
  return;
}
