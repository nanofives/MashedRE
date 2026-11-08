// FLAGS: /O2 /Ob1 /G5
/* C++-check edits: casts/prototypes only, DDK bytes unchanged */
/* MATCH-S28 0x005c90a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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
typedef int code();
uint FUN_005b9320(int p0);
void FUN_005c8910(int p0, int p1, int p2);
void FUN_005c9120(int p0);
// MATCH: 0x005c90a0 FUN_005c90a0
void FUN_005c90a0(int param_1,int param_2)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  
  iVar1 = *(int *)(param_1 + 0x20);
  *(int *)(iVar1 + 0x224) = *(int *)(iVar1 + 0x1ec) * 2;
  if (param_2 != 0) {
    iVar3 = *(int *)(iVar1 + 0x1f0);
    if (iVar3 != 0) {
      puVar2 = *(undefined4 **)(iVar3 + 0x10);
      if (puVar2 != (undefined4 *)0x0) {
        (*(int (*)(int, undefined4))*puVar2)(iVar3,puVar2[1]);
      }
      *(undefined4 *)(iVar1 + 0x1f0) = 0;
    }
    iVar3 = FUN_005b9320(iVar1 + 0x34);
    if (iVar3 != 0) {
      FUN_005c8910(param_1,0,1);
      *(undefined4 *)(iVar1 + 0x28) = 7;
    }
    FUN_005c9120(iVar1);
  }
  return;
}
