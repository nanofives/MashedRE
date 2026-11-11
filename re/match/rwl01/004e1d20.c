// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 84.8% (regs 84.8%), class b. Only the placement of the return-0 epilogue differs: original emits it right after the first 0x004cc580 test (later fails jump back to it), ours puts it at the end. Tried: early return 0, original wrapper form, for-loop with return 0 inside, permuter (no gain). */
/* MATCH-RWL01 0x004e1d20: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
#ifndef __cplusplus
#define true 1
#endif
undefined4 FUN_004cc580(undefined4 p0, undefined4 p1, undefined4 p2, uint p3, uint p4);
// MATCH: 0x004e1d20 FUN_004e1d20
int FUN_004e1d20(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  for (puVar1 = *(undefined4 **)(param_1 + 0x10); puVar1 != (undefined4 *)0x0;
      puVar1 = (undefined4 *)puVar1[0xc]) {
    if (((code *)puVar1[5] != (code *)0x0) &&
       (iVar2 = (*(code *)puVar1[5])(param_3,*puVar1,puVar1[1]), 0 < iVar2)) {
      iVar3 = iVar3 + 0xc + iVar2;
    }
  }
  iVar3 = FUN_004cc580(param_2,3,iVar3,0x37002,10);
  if (iVar3 == 0) {
    return 0;
  }
  for (puVar1 = *(undefined4 **)(param_1 + 0x10); puVar1 != (undefined4 *)0x0;
      puVar1 = (undefined4 *)puVar1[0xc]) {
    if (((((code *)puVar1[5] != (code *)0x0) && (puVar1[4] != 0)) &&
        (iVar3 = (*(code *)puVar1[5])(param_3,*puVar1,puVar1[1]), 0 < iVar3)) &&
       ((iVar2 = FUN_004cc580(param_2,puVar1[2],iVar3,0x37002,10), iVar2 == 0 ||
        (iVar3 = (*(code *)puVar1[4])(param_2,iVar3,param_3,*puVar1,puVar1[1]), iVar3 == 0)))) {
      return 0;
    }
  }
  return param_1;
}
