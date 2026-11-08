// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 100% (regs 100%), class -. next ptr loaded at loop top (puVar1=p[4]; p=puVar1), 6th COM arg is param_2 (Ghidra had param_1 twice), COM calls __stdcall, flag computed into a local before the call */
/* MATCH-RWL01 0x004dcaa0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(uint *, DAT_007d70c0, 0x007d70c0);
MGLOBAL(uint, DAT_00911fbc, 0x00911fbc);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(undefined4, DAT_007d70c4, 0x007d70c4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004dcaa0 FUN_004dcaa0

undefined4 FUN_004dcaa0(uint *param_1,uint *param_2)

{
  uint *puVar1;
  uint uVar2;
  uint *puVar3;
  int iVar4;
  uint *puVar5;
  
  puVar5 = (uint *)0x0;
  puVar3 = DAT_007d70c0;
  if (DAT_007d70c0 != (uint *)0x0) {
    do {
      puVar1 = (uint *)puVar3[4];
      if (puVar3[1] == 0) {
        if ((uint *)*puVar3 == param_1) {
          puVar3[1] = 1;
          *param_2 = puVar3[2];
          puVar3[3] = (uint)param_2;
          return 1;
        }
        if ((puVar5 == (uint *)0x0) || ((uint *)*puVar5 < (uint *)*puVar3)) {
          puVar5 = puVar3;
        }
      }
      puVar3 = puVar1;
    } while (puVar1 != (uint *)0x0);
    if (puVar5 != (uint *)0x0) {
      (*(int (__stdcall *)(int))*(int *)(*(int *)puVar5[2] + 8))(puVar5[2]);
      puVar5[2] = 0;
      goto LAB_004dcb2f;
    }
  }
  puVar5 = (uint *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d70c4,0x40411);
  puVar5[4] = (uint)DAT_007d70c0;
  DAT_007d70c0 = puVar5;
LAB_004dcb2f:
  uVar2 = ~DAT_00911fbc >> 0xf & 2;
  iVar4 = (*(int (__stdcall *)(int *,uint *,int,int,uint,uint *,int))*(int *)(*DAT_007d4110 + 0x68))
                    (DAT_007d4110,param_1,0x208,0,uVar2,param_2,0);
  if (iVar4 != 0) {
    return 0;
  }
  *puVar5 = (uint)param_1;
  puVar5[1] = 1;
  uVar2 = *param_2;
  puVar5[3] = (uint)param_2;
  puVar5[2] = uVar2;
  return 1;
}
