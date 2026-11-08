// FLAGS: /O2 /GS /Ob1
/* MATCH-RWL05 0x00562010: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
MGLOBAL(int, DAT_00623fac, 0x00623fac);
MGLOBAL(undefined1, FUN_005620c0, 0x005620c0);
undefined4 FUN_004e55d0(int * p0);
undefined4 FUN_00538c80(undefined4 p0, undefined4 * p1, undefined4 p2, undefined4 p3);
void FUN_00562380(int p0, int p1);
// MATCH: 0x00562010 FUN_00562010
int FUN_00562010(int param_1)

{
  ushort *puVar1;
  ushort uVar2;
  undefined4 uVar3;
  uint uVar4;
  undefined1 local_44 [28];
  uint local_28;
  undefined4 local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  
  local_24 = **(undefined4 **)(DAT_00623fac + param_1);
  local_20 = param_1;
  uVar3 = FUN_004e55d0(param_1);
  uVar2 = *(ushort *)(param_1 + 0x84);
  uVar4 = 0;
  if (uVar2 != 0) {
    do {
      local_14 = *(int *)(param_1 + 8);
      puVar1 = (ushort *)(*(int *)(param_1 + 4) + uVar4 * 8);
      local_1c = local_14 + (uint)*puVar1 * 0xc;
      local_18 = local_14 + (uint)puVar1[1] * 0xc;
      local_14 = local_14 + (uint)puVar1[2] * 0xc;
      local_28 = uVar4;
      FUN_00562380(&local_28,local_44);
      FUN_00538c80(uVar3,local_44,FUN_005620c0,&local_28);
      uVar4 = uVar4 + 1;
    } while (uVar4 < uVar2);
  }
  return param_1;
}
