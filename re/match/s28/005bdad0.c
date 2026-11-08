// FLAGS: /O2 /Oy- /Ob1 /G5
// best 40.9 (regs 48.9, ops 69.3) with /O2 /Oy- /Ob1 /G5 (original has an ebp frame; /Oy- was the biggest step, 38.7 -> 40.9; callee arities fixed). Residual: original keeps 0 (param_1 = 0) in edi and compares memory against edi, and the 64x64 multiply is the inline or/mul/mul sequence over two 8-byte stack locals ([ebp-8] x [ebp-0x10]); ours uses a different register plan and a different 64-bit multiply form (longlong a*b rewrite tried, lowered ops%). Class (b).
/* MATCH-S28 0x005bdad0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned __int64 undefined8;
typedef unsigned int uint;
typedef unsigned char byte;
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;
typedef int (__stdcall *VM3)(int *, undefined1 *, undefined1 *);
#define CONCAT44(a,b) ((ulonglong)(((ulonglong)(uint)(a)<<32)|(uint)(b)))
undefined4 FUN_005b1180(int p0, int p1, int p2);
void FUN_005bde50(int p0);
void FUN_005bde70(int p0, undefined4 p1);
void FUN_005bdef0(int p0);
void FUN_005bdf40(int p0, int p1);
undefined4 FUN_005bf350(int p0, int p1, int p2);
undefined8 __stdcall __alldiv(uint p0, uint p1, uint p2, uint p3);
// MATCH: 0x005bdad0 FUN_005bdad0
int FUN_005bdad0(int param_1)
{
  int iVar1;
  longlong lVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 local_24;
  undefined4 local_20;
  undefined1 local_1c[8];
  uint local_14;
  undefined4 local_10;
  uint local_c;
  int local_8;

  iVar1 = *(int *)(param_1 + 0x20);
  param_1 = 0;
  if ((*(char *)(iVar1 + 0x749) == *(char *)(iVar1 + 0x74b)) && (*(int *)(iVar1 + 0x7a8) == 0)) {
    return 0;
  }
  if (*(int *)(iVar1 + 0x794) == 0) {
    uVar3 = FUN_005bf350(*(int *)(iVar1 + 0x6f8), 1, *(int *)(iVar1 + 0x6fc));
    FUN_005bde70(iVar1 + 0x788, uVar3);
    if (*(int *)(iVar1 + 0x794) == 0) {
      return 0;
    }
  }
  if (*(int *)(iVar1 + 0x7a8) == 0) {
    uVar3 = FUN_005b1180(iVar1 + 0x748, 2, *(int *)(iVar1 + 0x6fc));
    FUN_005bde70(iVar1 + 0x79c, uVar3);
  }
  if ((*(int *)(iVar1 + 0x7a0) == 0) &&
      (iVar4 = (*(VM3 *)(**(int **)(iVar1 + 0x794) + 0x14))(*(int **)(iVar1 + 0x794), (undefined1 *)&local_c, local_1c), -1 < iVar4)) {
    local_14 = *(uint *)(iVar1 + 0x72c);
    local_10 = 0;
    if (local_8 == 0) {
      lVar2 = (ulonglong)local_c * (ulonglong)local_14;
    }
    else {
      lVar2 = CONCAT44((int)((ulonglong)local_c * (ulonglong)local_14 >> 0x20) + local_8 * local_14,
                       (int)((ulonglong)local_c * (ulonglong)local_14));
    }
    local_20 = (undefined4)((ulonglong)lVar2 >> 0x20);
    local_24 = (undefined4)lVar2;
    uVar3 = (undefined4)__alldiv(local_24, local_20, 10000000, 0);
    *(undefined4 *)(*(int *)(*(int *)(iVar1 + 0x7a8) + 0xc) + 8) = uVar3;
  }
  FUN_005bdf40(iVar1 + 0x79c, iVar1 + 0x788);
  iVar4 = *(int *)(iVar1 + 0x7a8);
  if ((iVar4 != 0) && (*(int *)(iVar1 + 0x7a0) == *(int *)(iVar1 + 0x7a4))) {
    FUN_005bde50(iVar1 + 0x79c);
    param_1 = iVar4;
  }
  if ((*(int *)(iVar1 + 0x794) != 0) && (*(int *)(iVar1 + 0x78c) == *(int *)(iVar1 + 0x790))) {
    FUN_005bdef0(iVar1 + 0x788);
  }
  return param_1;
}
