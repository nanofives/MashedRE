// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005bc0f0: 23.3% (regs 26.0%), raw Ghidra shape (C++ fixes only: code typedef, casts). The decomp is wrong: the original is an 8-arg stdcall vtable call (+0x2c) with in/out param slots followed by two variable-size memcpy (rep movsd/movsb) into the output slots and a vtable +0x4c call returning hr >= 0; not rewritten, budget spent. Class: b.
/* MATCH-S27 0x005bc0f0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define false 0
#endif
// MATCH: 0x005bc0f0 FUN_005bc0f0
bool FUN_005bc0f0(undefined4 *param_1,undefined4 *param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined1 local_4 [4];
  
  puVar1 = param_1;
  puVar6 = param_2;
  iVar2 = (**(code **)(*(int *)*param_1 + 0x2c))((int *)*param_1,param_2,param_4);
  if (-1 < iVar2) {
    puVar4 = puVar6;
    puVar5 = (undefined4 *)&param_2;
    for (uVar3 = (uint)&param_1 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar5 = puVar5 + 1;
    }
    for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
      puVar4 = (undefined4 *)((int)puVar4 + 1);
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    }
    if (local_4 != (undefined1 *)0x0) {
      puVar4 = &param_4;
      puVar5 = (undefined4 *)((int)&param_1 + (int)&param_2);
      for (uVar3 = (uint)local_4 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
        *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
        puVar4 = (undefined4 *)((int)puVar4 + 1);
        puVar5 = (undefined4 *)((int)puVar5 + 1);
      }
    }
    iVar2 = (**(code **)(*(int *)*puVar1 + 0x4c))((int *)*puVar1,puVar6,&param_1,&param_4,local_4);
    return -1 < iVar2;
  }
  return false;
}
