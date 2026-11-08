// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 100% (regs 100%), relocheck OK, C++ check OK, was 80.6%. Natural for loop over a {int a,b} array instead of the Ghidra do-while. Supersedes the header below. */
/* MATCH-RWL05 0x0055dca0: hand-matched (h3). 80.6% (regs 80.6, ops 93.5). unsigned `> 0` guard gives jbe; residue is ebp used as a shared zero constant (original uses immediates). class (b). */
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
typedef int (*Alloc)(int, int);
typedef int (*Alloc3)(int, int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_00559b50(int p0, int p1, int p2, int p3);
int FUN_0055f450(int *p0, int p1, int p2);
struct P2 { int a, b; };
// MATCH: 0x0055dca0 FUN_0055dca0
int *FUN_0055dca0(int *param_1, int param_2, int param_3, int param_4)
{
  int iVar1;
  int uVar2;
  unsigned int uVar3;

  uVar2 = FUN_00559b50(param_2, param_4, param_3, 0);
  *param_1 = uVar2;
  uVar2 = FUN_0055f450(param_1, param_2, param_3);
  param_1[1] = uVar2;
  iVar1 = *(int *)(param_2 + 0x24);
  param_1[3] = 0;
  param_1[4] = iVar1;
  iVar1 = iVar1 * 4;
  uVar2 = (*(Alloc *)(DAT_007d3ff8 + 0x108))(iVar1, 0x30900);
  param_1[2] = uVar2;
  uVar2 = (*(Alloc *)(DAT_007d3ff8 + 0x108))(iVar1, 0x30900);
  param_1[5] = uVar2;
  param_1[6] = *(int *)(param_2 + 4);
  uVar2 = (*(Alloc3 *)(DAT_007d3ff8 + 0x114))(*(int *)(param_2 + 4), 8, 0x30900);
  param_1[7] = uVar2;
  for (uVar3 = 0; uVar3 < *(unsigned int *)(param_2 + 4); uVar3++) {
    ((struct P2 *)param_1[7])[uVar3].a = 0;
    ((struct P2 *)param_1[7])[uVar3].b = 0;
  }
  return param_1;
}
