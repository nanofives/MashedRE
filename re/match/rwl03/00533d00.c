// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00533d00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
MGLOBAL(undefined4, DAT_0091246c, 0x0091246c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_0052ddf0(int p0, int p1);
// MATCH: 0x00533d00 FUN_00533d00
uint * FUN_00533d00(uint param_1,undefined4 *param_2,int param_3,uint param_4,undefined4 param_5)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  
  puVar1 = (uint *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_0091246c,0x3011e);
  uVar2 = FUN_0052ddf0(param_1,param_5);
  puVar1[8] = uVar2;
  *puVar1 = param_4;
  puVar1[1] = param_1;
  puVar1[5] = 0;
  if ((param_4 & 2) == 0) {
    uVar2 = (**(code **)(DAT_007d3ff8 + 0x108))(param_1 * 0x40 + 0xf,0x3011e);
    puVar1[3] = uVar2;
    puVar1[2] = uVar2 + 0xf & 0xfffffff0;
  }
  else {
    puVar1[2] = 0;
    puVar1[3] = 0;
  }
  uVar2 = (**(code **)(DAT_007d3ff8 + 0x108))(param_1 << 4,0x3011e);
  iVar4 = 0;
  puVar1[4] = uVar2;
  if (0 < (int)param_1) {
    iVar3 = 0;
    puVar5 = param_2;
    do {
      *(undefined4 *)(iVar3 + 0xc + puVar1[4]) = 0;
      if (param_3 != 0) {
        *(undefined4 *)(iVar3 + puVar1[4]) = *(undefined4 *)((param_3 - (int)param_2) + (int)puVar5)
        ;
      }
      *(int *)(iVar3 + 4 + puVar1[4]) = iVar4;
      if (param_2 != (undefined4 *)0x0) {
        *(undefined4 *)(iVar3 + 8 + puVar1[4]) = *puVar5;
      }
      iVar4 = iVar4 + 1;
      puVar5 = puVar5 + 1;
      iVar3 = iVar3 + 0x10;
    } while (iVar4 < (int)param_1);
  }
  puVar1[6] = (uint)puVar1;
  return puVar1;
}
