// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 50.0% (regs 75.0%), class b. Structure right (single exit returning iVar2, nested realloc fail path with 8-byte local, goto done); register assignment differs (ours param=edi/iVar2=esi, original param=esi/iVar2=edi/uVar3=ebx) and frame lacks sub esp 8/ebx. Tried: return iVar2 vs 0, single-exit goto, pointer param, decl order, permuter (62.5% variant reorders side effects: rejected). */
/* MATCH-RWL01 0x004e13f0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004e13f0 FUN_004e13f0
int FUN_004e13f0(int param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar3 = *(uint *)(param_1 + 0x14);
  iVar2 = *(int *)(param_1 + 0xc);
  if (*(uint *)(param_1 + 0x10) <= uVar3) {
    iVar1 = *(uint *)(param_1 + 0x10) + 0x20;
    *(int *)(param_1 + 0x10) = iVar1;
    iVar2 = (**(code **)(DAT_007d3ff8 + 0x110))(iVar2,iVar1 * 8,0x1030409);
    if (iVar2 == 0) {
      uStack_8 = 1;
      uStack_4 = FUN_004d7ff0(0x80000013,*(int *)(param_1 + 0x10) << 3);
      FUN_004d8480(&uStack_8);
      *(int *)(param_1 + 0x10) = *(int *)(param_1 + 0x10) + -0x20;
      goto done;
    }
    iVar1 = iVar2;
    if (iVar2 != *(int *)(param_1 + 0xc)) {
      for (; uVar3 != 0; uVar3 = uVar3 - 1) {
        *(int *)(*(int *)(iVar1 + 4) + 0xc) = iVar1;
        iVar1 = iVar1 + 8;
      }
    }
    *(int *)(param_1 + 0xc) = iVar2;
  }
  if (iVar2 != 0) {
    iVar1 = *(int *)(param_1 + 0x14);
    *(int *)(param_1 + 0x14) = iVar1 + 1;
    return iVar2 + iVar1 * 8;
  }
done:
  return iVar2;
}
