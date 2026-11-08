// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 63.2% (regs 67.5%), class b. Control flow now follows the original (resize arm first, malloc arm second, for(uVar4<count) loop). Residual is register/frame allocation: original saves ebp at entry (counter) and ebx only in the loop, uses edi=iVar1+4 strength-reduced base, and has no shared return-0 epilogues. Tried arm order, loop forms. */
/* MATCH-RWL01 0x004d4dd0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
undefined4 FUN_004d4910(int * p0, undefined4 p1);
undefined4 FUN_004d4a20(int * p0, int * p1);
undefined4 FUN_004d7ff0(undefined4 p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004d4dd0 FUN_004d4dd0
int * FUN_004d4dd0(int *param_1)

{
  int iVar1;
  code *pcVar2;
  int iVar3;
  uint uVar4;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  if (*param_1 == 0) {
    uVar4 = *(int *)(DAT_00911ae0 + 0x38 + DAT_007d3ff8) * 0xb4;
    if (param_1[2] != 0) {
      if (((uint)param_1[9] < uVar4) && (iVar3 = FUN_004d4910(param_1,uVar4), iVar3 == 0)) {
        return (int *)0x0;
      }
      iVar3 = FUN_004d4a20(param_1,param_1);
      if (iVar3 == 0) {
        return (int *)0x0;
      }
    }
    else {
      iVar3 = (**(code **)(DAT_007d3ff8 + 0x108))(uVar4,0x1030409);
      param_1[8] = iVar3;
      if (iVar3 == 0) {
        uStack_8 = 1;
        uStack_4 = FUN_004d7ff0(0x80000013,uVar4);
        FUN_004d8480(&uStack_8);
        return (int *)0x0;
      }
      param_1[9] = uVar4;
      param_1[2] = iVar3;
    }
    *param_1 = 1;
    if (param_1[2] != 0) {
      iVar3 = 0;
      for (uVar4 = 0; uVar4 < (uint)param_1[1]; uVar4++) {
        iVar1 = *(int *)(iVar3 + param_1[2]);
        pcVar2 = *(code **)(iVar1 + 0x14);
        if (pcVar2 != (code *)0x0) {
          (*pcVar2)(iVar3 + param_1[2]);
        }
        *(int *)(*(int *)(iVar3 + param_1[2]) + 0x3c) =
             *(int *)(*(int *)(iVar3 + param_1[2]) + 0x3c) + -1;
        if ((*(int *)(*(int *)(iVar3 + param_1[2]) + 0x3c) == 0) &&
           (pcVar2 = *(code **)(iVar1 + 0xc), pcVar2 != (code *)0x0)) {
          (*pcVar2)(*(int *)(iVar3 + param_1[2]));
        }
        *(undefined4 *)(iVar3 + 0xc + param_1[2]) = 0;
        iVar3 = iVar3 + 0x28;
      }
    }
  }
  return param_1;
}
