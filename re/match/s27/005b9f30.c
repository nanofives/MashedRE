// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b9f30: 29.2% (regs 32.7%), raw converter body kept. Constructor whose body repeats (1 << FUN_005aee20(0x28)) & 0xffff tests; the original expands a macro/inline that re-calls FUN_005aee20(0x28) at every use, ours folds to 110 vs 171 instr. Residual (b): macro form not found; no structural pass (budget).  C++ fixes: FUN_ addresses taken with (code *)&, (undefined4)& for the descriptor. */
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
typedef int code();
MGLOBAL(undefined1, DAT_00617ff8, 0x00617ff8);
MGLOBAL(undefined1, DAT_005e6e50, 0x005e6e50);
MGLOBAL(undefined1, FUN_005ba780, 0x005ba780);
MGLOBAL(undefined1, FUN_005ba7f0, 0x005ba7f0);
MGLOBAL(undefined1, FUN_005ba1d0, 0x005ba1d0);
MGLOBAL(undefined1, FUN_005bad30, 0x005bad30);
undefined4 FUN_005a9e10(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_005aee20(uint p0);
// MATCH: 0x005b9f30 FUN_005b9f30
/* WARNING: Removing unreachable block (ram,0x005b9fc4) */
/* WARNING: Removing unreachable block (ram,0x005b9fa5) */
/* WARNING: Removing unreachable block (ram,0x005b9fcc) */
/* WARNING: Removing unreachable block (ram,0x005ba036) */
/* WARNING: Removing unreachable block (ram,0x005ba078) */
/* WARNING: Removing unreachable block (ram,0x005ba04c) */
/* WARNING: Removing unreachable block (ram,0x005b9fd8) */
/* WARNING: Removing unreachable block (ram,0x005ba01c) */
/* WARNING: Removing unreachable block (ram,0x005b9fee) */
/* [C2 2026-06-02] `FUN_005a9e10(param_1, &DAT_005e6e50, 0)` ? base/sub-object constructor,
   passing the class-name/ descriptor global? */

void FUN_005b9f30(int param_1)

{
  byte bVar1;
  int iVar2;
  char cVar4;
  uint uVar3;
  
  FUN_005a9e10(param_1,(undefined4)&DAT_005e6e50,0);
  *(undefined4 *)(param_1 + 0x40) = 0xf8;
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
  *(code **)(param_1 + 0x28) = (code *)&FUN_005ba1d0;
  *(code **)(param_1 + 0x34) = (code *)&FUN_005ba780;
  *(uint *)(param_1 + 0x40) = *(uint *)(param_1 + 0x40) | iVar2 << 0x1c;
  *(code **)(param_1 + 0x30) = (code *)&FUN_005ba7f0;
  *(code **)(param_1 + 0x3c) = (code *)&FUN_005bad30;
  *(undefined2 *)(param_1 + 0x44) = 0x28;
  *(undefined2 *)(param_1 + 0x46) = 1;
  return;
}
