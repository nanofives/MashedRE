// FLAGS: /O2 /GS /Ob1
/* MATCH-RWL05 0x005645b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl05_20261107/results.tsv. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
// MATCH: 0x005645b0 FUN_005645b0
int FUN_005645b0(int param_1,undefined4 param_2,uint param_3,undefined4 *param_4,undefined4 param_5)

{
  int iVar1;
  short sVar2;
  uint uVar3;
  uint *puVar4;
  short *psVar5;
  uint uVar6;
  undefined4 *puVar7;
  
  iVar1 = *(int *)(param_1 + 8);
  *(int *)(param_1 + 8) = iVar1 + 0xc01c;
  *(undefined4 *)(iVar1 + 0x7ff0) = param_4[4];
  *(undefined4 *)(iVar1 + 0x7ff4) = param_4[5];
  *(undefined4 *)(iVar1 + 0x7ff8) = param_4[6];
  *(undefined4 *)(iVar1 + 0x7fe0) = *param_4;
  *(undefined4 *)(iVar1 + 0x7fe4) = param_4[1];
  puVar4 = (uint *)(iVar1 + 0x9830);
  *(undefined4 *)(iVar1 + 0x7fe8) = param_4[2];
  *(undefined4 *)(iVar1 + 0xc010) = param_2;
  *(uint *)(iVar1 + 0x801c) = param_3;
  uVar3 = 1;
  do {
    uVar6 = uVar3;
    uVar3 = uVar6 + 1;
    *puVar4 = uVar3;
    puVar4 = puVar4 + 5;
  } while (uVar3 < 0x198);
  psVar5 = (short *)(iVar1 + 0x8820);
  *(undefined4 *)(iVar1 + (uVar6 + 0x2608 + uVar3 * 4) * 4) = 0x3ff;
  *(undefined2 *)(iVar1 + 0xc00e) = 1;
  uVar3 = 0;
  do {
    sVar2 = (short)uVar3;
    uVar3 = uVar3 + 1;
    *psVar5 = sVar2 + 1;
    psVar5 = psVar5 + 2;
  } while (uVar3 < 0x3fe);
  *(undefined2 *)(iVar1 + 0x8820 + uVar3 * 4) = 0x3ff;
  *(undefined2 *)(iVar1 + 0x981a) = 0;
  *(undefined4 *)(iVar1 + 0x981c) = 0x3ff;
  uVar3 = 0;
  do {
    uVar6 = uVar3 & 0xffff;
    uVar3 = uVar3 + 1;
    *(undefined2 *)(iVar1 + 0x9820 + uVar6 * 2) = 0x83ff;
  } while ((int)uVar3 < 8);
  if (param_3 != 0) {
    puVar7 = (undefined4 *)(iVar1 + 0xb810);
    for (uVar3 = param_3 >> 1; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar7 = 0;
      puVar7 = puVar7 + 1;
    }
    for (uVar3 = (uint)((param_3 & 1) != 0); uVar3 != 0; uVar3 = uVar3 - 1) {
      *(undefined2 *)puVar7 = 0;
      puVar7 = (undefined4 *)((int)puVar7 + 2);
    }
  }
  *(undefined4 *)(iVar1 + 0xc018) = 0;
  *(undefined4 *)(iVar1 + 0xc014) = param_5;
  return iVar1;
}
