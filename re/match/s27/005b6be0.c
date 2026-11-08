// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b6be0: 31.6% (regs 35.1%), raw Ghidra shape (C++ fixes: function-pointer casts for the vtable stores). 102 vs 171 ins: the vtable and init block is not fully reproduced. Not iterated. Class: b.
/* MATCH-S27 0x005b6be0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(undefined1, DAT_00617ff8, 0x00617ff8);
MGLOBAL(undefined1, DAT_005e6ce0, 0x005e6ce0);
MGLOBAL(undefined1, FUN_005b6e60, 0x005b6e60);
MGLOBAL(undefined1, FUN_005b7040, 0x005b7040);
MGLOBAL(undefined1, FUN_005b7010, 0x005b7010);
MGLOBAL(undefined1, FUN_005b70e0, 0x005b70e0);
MGLOBAL(undefined1, FUN_004d7ff0, 0x004d7ff0);
undefined4 FUN_005a9e10(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_005aee20(uint p0);
// MATCH: 0x005b6be0 FUN_005b6be0
/* WARNING: Removing unreachable block (ram,0x005b6c74) */
/* WARNING: Removing unreachable block (ram,0x005b6c55) */
/* WARNING: Removing unreachable block (ram,0x005b6c7c) */
/* WARNING: Removing unreachable block (ram,0x005b6ce6) */
/* WARNING: Removing unreachable block (ram,0x005b6d28) */
/* WARNING: Removing unreachable block (ram,0x005b6cfc) */
/* WARNING: Removing unreachable block (ram,0x005b6c88) */
/* WARNING: Removing unreachable block (ram,0x005b6ccc) */
/* WARNING: Removing unreachable block (ram,0x005b6c9e) */
/* [C2 2026-06-01] Constructor/installer for a codec sink object: chains to a base initializer, then
   installs a function-pointer table? */

void FUN_005b6be0(int param_1)

{
  byte bVar1;
  int iVar2;
  char cVar4;
  uint uVar3;
  
  FUN_005a9e10(param_1,(undefined4)&DAT_005e6ce0,0);
  *(undefined4 *)(param_1 + 0x40) = 0x88;
  bVar1 = FUN_005aee20(0x28);
  if ((short)(1 << (bVar1 & 0x1f)) != 0) {
    FUN_005aee20(0x28);
  }
  bVar1 = FUN_005aee20(0x28);
  if ((short)(1 << (bVar1 & 0x1f)) == 0) {
    cVar4 = '\0';
  }
  else {
    bVar1 = FUN_005aee20(0x28);
    cVar4 = (char)((uint)(1 << (bVar1 & 0x1f)) >> 8);
  }
  if (cVar4 == '\0') {
    bVar1 = FUN_005aee20(0x28);
    if ((short)(1 << (bVar1 & 0x1f)) == 0) {
      uVar3 = 1;
    }
    else {
      bVar1 = FUN_005aee20(0x28);
      uVar3 = 1 << (bVar1 & 0x1f) & 0xffff;
    }
    iVar2 = (&DAT_00617ff8)[uVar3] + -1;
  }
  else {
    bVar1 = FUN_005aee20(0x28);
    if ((short)(1 << (bVar1 & 0x1f)) == 0) {
      iVar2 = DAT_00617ff8 + 7;
    }
    else {
      bVar1 = FUN_005aee20(0x28);
      iVar2 = (&DAT_00617ff8)[(int)(1 << (bVar1 & 0x1f) & 0xffffU) >> 8] + 7;
    }
  }
  *(code **)(param_1 + 0x28) = (code *)&FUN_005b6e60;
  *(code **)(param_1 + 0x34) = (code *)&FUN_005b7010;
  *(uint *)(param_1 + 0x40) = *(uint *)(param_1 + 0x40) | iVar2 << 0x1c;
  *(code **)(param_1 + 0x2c) = (code *)&FUN_005b7040;
  *(code **)(param_1 + 0x30) = (code *)&FUN_004d7ff0;
  *(code **)(param_1 + 0x3c) = (code *)&FUN_005b70e0;
  *(undefined2 *)(param_1 + 0x44) = 0x20;
  return;
}
