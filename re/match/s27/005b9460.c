// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b9460: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs27_20261107/results.tsv. */
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
void FUN_005b92e0(int p0, int p1);
uint FUN_005b9320(int p0);
void FUN_005bbf30(int * p0);
undefined4 FUN_005bbf60(int * p0, undefined4 * p1, undefined4 * p2);
void FUN_005c75c0(int p0, undefined4 p1);
// MATCH: 0x005b9460 FUN_005b9460
void FUN_005b9460(int param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  
  iVar1 = FUN_005b9320(param_1);
  if (iVar1 != 0) {
    FUN_005b92e0(param_1,0);
  }
  uVar3 = *(uint *)(*(int *)(param_1 + 0x94) + 0x50) >> 3 & 1;
  if (uVar3 != 0) {
    uVar2 = 0;
    if (param_2 != 0) {
      uVar2 = *(undefined4 *)(param_2 + 0x4c);
    }
    FUN_005c75c0(*(undefined4 *)(param_1 + 0x11c),uVar2);
  }
  if ((*(int *)(param_1 + 0x84) != 0) && (uVar3 == 0)) {
    FUN_005bbf30((int *)(param_1 + 0xdc));
  }
  *(undefined4 *)(param_1 + 0x84) = 0;
  *(undefined4 *)(param_1 + 0x38) = 0x3f800000;
  *(undefined4 *)(param_1 + 0x3c) = 0x3f800000;
  if (param_2 != 0) {
    if ((uVar3 == 0) &&
       (iVar1 = FUN_005bbf60((int *)*(undefined4 *)(*(int *)(param_1 + 0x94) + 0x54),(undefined4 *)(param_2 + 0x5c),
                             (undefined4 *)(param_1 + 0xdc)), iVar1 == 0)) {
      return;
    }
    *(int *)(param_1 + 0x84) = param_2;
    *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) & 0xffffffdf | 0x1000;
  }
  return;
}
