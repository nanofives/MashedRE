// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005bbdb0: 18.8% (regs 25.9%), raw Ghidra shape (C++ fixes only); 38 vs 85 ins, the Ghidra body drops the vtable-call arguments (same artefact as 0x005bac00). Not rewritten, budget spent. Class: b.
/* MATCH-S27 0x005bbdb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#define CONCAT12(a,b) ((uint)(((uint)(uchar)(a)<<16)|(ushort)(b)))
MGLOBAL(undefined4, DAT_005d099c, 0x005d099c);
MGLOBAL(undefined4, DAT_005d1144, 0x005d1144);
undefined4 FUN_005bbed0(undefined4 * p0, int p1, int p2);
// MATCH: 0x005bbdb0 FUN_005bbdb0
undefined4
FUN_005bbdb0(int *param_1,int *param_2,int param_3,undefined4 param_4,uint param_5,
            undefined4 *param_6)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 unaff_retaddr;
  undefined2 local_38;
  ushort uStack_36;
  undefined2 local_34;
  undefined2 uStack_32;
  int local_30;
  uint local_2c;
  undefined2 local_28;
  undefined4 local_24;
  uint local_20;
  int local_1c;
  undefined4 local_18;
  undefined2 *local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  *param_6 = 0;
  param_6[1] = 0;
  param_6[2] = 0;
  uStack_36 = (ushort)*(byte *)((int)param_2 + 0xd);
  local_30 = *param_2;
  local_34 = (undefined2)local_30;
  uStack_32 = (undefined2)((uint)local_30 >> 0x10);
  local_2c = (uint)CONCAT12(*(byte *)(param_2 + 3),(*(byte *)(param_2 + 3) >> 3) * uStack_36);
  local_38 = 1;
  local_30 = (local_2c & 0xffff) * local_30;
  local_28 = 0x12;
  if (param_3 != 0) {
    param_5 = param_5 | 0x20010;
  }
  local_1c = param_2[2];
  local_14 = &local_38;
  local_24 = 0x24;
  local_18 = 0;
  puVar1 = &DAT_005d099c;
  if ((param_5 & 1) == 0) {
    puVar1 = &DAT_005d1144;
  }
  local_c = puVar1[1];
  local_10 = *puVar1;
  local_4 = puVar1[3];
  local_8 = puVar1[2];
  local_20 = param_5;
  iVar2 = (**(code **)(*param_1 + 0xc))(param_1,&local_24,param_6,0);
  if (-1 < iVar2) {
    iVar2 = FUN_005bbed0(param_6,param_3,unaff_retaddr);
    if (iVar2 != 0) {
      return 1;
    }
    (**(code **)(*(int *)*param_6 + 8))((int *)*param_6);
  }
  return 0;
}
