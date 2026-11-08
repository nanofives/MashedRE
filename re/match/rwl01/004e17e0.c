// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 100% (regs 100%), class -. puVar2=0 assigned before the puVar3[5]=0 store so the zero register is not shared with the immediate store */
/* MATCH-RWL01 0x004e17e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
undefined4 FUN_004e15f0(uint * p0, uint * p1, uint * p2);
// MATCH: 0x004e17e0 FUN_004e17e0
uint * FUN_004e17e0(uint param_1)

{
  uint *puVar1;
  uint *puVar2;
  uint *puVar3;
  uint *puVar4;
  int iVar5;
  uint uVar6;
  
  if (param_1 < 0x400) {
    param_1 = 0x400;
  }
  puVar3 = (uint *)(**(code **)(DAT_007d3ff8 + 0x108))(0x1c,0x40409);
  if (puVar3 != (uint *)0x0) {
    uVar6 = param_1 + 0x1f & 0xffffffe0;
    if (uVar6 < 0x80) {
      uVar6 = 0x80;
    }
    puVar4 = (uint *)(**(code **)(DAT_007d3ff8 + 0x108))(uVar6 + 0x8b,0x1040409);
    if (puVar4 != (uint *)0x0) {
      puVar4[1] = uVar6;
      puVar4[2] = 0;
      *puVar4 = (int)puVar4 + 0x8bU & 0xffffff80;
      *puVar3 = uVar6;
      puVar3[1] = (uint)puVar4;
      puVar3[3] = 0;
      puVar3[4] = 0;
      puVar3[6] = 1;
      puVar2 = (uint *)0x0;
      puVar3[5] = 0;
      for (puVar1 = (uint *)puVar4[2]; puVar1 != (uint *)0x0; puVar1 = (uint *)puVar1[2]) {
        iVar5 = FUN_004e15f0(puVar1,puVar2,puVar3);
        if (iVar5 == 0) goto LAB_004e18c8;
        if (puVar2 == (uint *)0x0) {
          puVar3[2] = *puVar1;
        }
        puVar2 = puVar1;
      }
      puVar1 = (uint *)puVar3[1];
      iVar5 = FUN_004e15f0(puVar1,puVar2,puVar3);
      if (iVar5 != 0) {
        if (puVar2 == (uint *)0x0) {
          puVar3[2] = *puVar1;
        }
        puVar3[6] = 0;
        return puVar3;
      }
LAB_004e18c8:
      (**(code **)(DAT_007d3ff8 + 0x10c))(puVar4);
    }
    (**(code **)(DAT_007d3ff8 + 0x10c))(puVar3);
  }
  return (uint *)0x0;
}
