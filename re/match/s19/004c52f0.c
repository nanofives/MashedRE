// TOOLCHAIN rw9044
// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c52f0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs19_20261102/results.tsv. */
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
typedef int code();
MGLOBAL(int, DAT_007d4028, 0x007d4028);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
extern char s_Invalid_combination_type_0061811c[];
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004c52f0 FUN_004c52f0
uint * FUN_004c52f0(uint *param_1,uint *param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  undefined4 local_48;
  undefined4 local_44;
  uint local_40 [16];
  
  if (param_3 == 0) {
    puVar4 = param_1;
    for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar4 = *param_2;
      param_2 = param_2 + 1;
      puVar4 = puVar4 + 1;
    }
    return param_1;
  }
  if (param_3 == 1) {
    uVar3 = param_2[3];
    uVar6 = param_1[3];
    uVar1 = *(uint *)(DAT_007d4028 + 4 + DAT_007d3ff8) & 0x20000;
    if ((uVar3 & uVar1) != 0) {
      puVar4 = param_1;
      puVar5 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      puVar4 = local_40;
      puVar5 = param_1;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
    puVar4 = param_2;
    puVar5 = param_1;
    if ((uVar6 & uVar1) != 0) {
      puVar4 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar4 = *param_2;
        param_2 = param_2 + 1;
        puVar4 = puVar4 + 1;
      }
      puVar4 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
  }
  else {
    if (param_3 != 2) {
      local_48 = 1;
      local_44 = FUN_004d7ff0(0x80000003,s_Invalid_combination_type_0061811c);
      FUN_004d8480(&local_48);
      return (uint *)0x0;
    }
    uVar3 = param_1[3];
    uVar6 = param_2[3];
    uVar1 = *(uint *)(DAT_007d4028 + 4 + DAT_007d3ff8) & 0x20000;
    if ((uVar3 & uVar1) != 0) {
      puVar4 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar4 = *param_2;
        param_2 = param_2 + 1;
        puVar4 = puVar4 + 1;
      }
      puVar4 = local_40;
      puVar5 = param_1;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
    puVar4 = param_1;
    puVar5 = param_2;
    if ((uVar6 & uVar1) != 0) {
      puVar5 = local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      puVar4 = local_40;
      puVar5 = param_1;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      return param_1;
    }
  }
  (**(code **)(DAT_007d4028 + 8 + DAT_007d3ff8))(local_40,puVar4,puVar5);
  local_40[3] = uVar6 & uVar3;
  puVar4 = local_40;
  puVar5 = param_1;
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar5 = *puVar4;
    puVar4 = puVar4 + 1;
    puVar5 = puVar5 + 1;
  }
  return param_1;
}
